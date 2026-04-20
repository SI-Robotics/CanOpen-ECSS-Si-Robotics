#include "canopen.h"
#include "usart.h"

//static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data);
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,void *data);
//static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,co_nmt_ecss_rdn_reason_t reason, void *data);
static void rpdo_ind(co_rpdo_t*, co_unsigned32_t ac, const void*, size_t, void*);
static void rpdo_err(co_rpdo_t*, co_unsigned16_t eec, co_unsigned8_t er, void*);
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data);
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);

static uint8_t  memory_pool[CANOPEN_MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static struct   mempool pool;
static bool duplicate_node_locked = false;
canopen_t canopen;

static void HAL_LOG(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart2, (uint8_t *)s, (uint16_t)strlen(s), 100);
  const char crlf[] = "\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t *)crlf, 2, 100);
}

static void HAL_LOGF(const char *fmt, ...)
{
  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  HAL_LOG(buf);
}


static int can_send_handler(const struct can_msg *const msg, const uint_least8_t bus_id, void *const data)
{
    (void)data;
    if (msg->id == 0x000u)   /* NMT frame */
    {
    	HAL_LOGF("[MASTER] NMT frame: id=0x%03lX data[0]=0x%02X data[1]=0x%02X bus=%d",(unsigned long)msg->id,msg->data[0], msg->data[1], (int)bus_id);
    }
    return canopen.cfg.send((uint8_t)bus_id, msg) ? 0 : -1;
}

static void enter_error(const char *reason)
{
    HAL_LOGF("%s ERROR: %s","[MASTER]", reason ? reason : "?");
    canopen.bus_state = CANOPEN_BUS_STOPPED;
    if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
}

//static void reinit_slave(void)
//{
//    HAL_LOGF("%s  NMT START -> 0x%02X","[MASTER]", CANOPEN_REMOTE_NODE);
//    if (co_nmt_cs_req(canopen.nmt, CO_NMT_CS_START, CANOPEN_REMOTE_NODE) != 0)
//    {
//        HAL_LOGF("%s  NMT START failed","[MASTER]");
//    }
//    canopen.slave_boot_pending = false;
//}

static void do_bus_switch(void)
{
    if (canopen.bus_switch_count >= CANOPEN_MAX_BUS_SWITCHES) {
        enter_error("max bus switches");
        return;
    }

    uint8_t new_bus = (canopen.bus_id == CANOPEN_BUS_A_ID)
                      ? CANOPEN_BUS_B_ID : CANOPEN_BUS_A_ID;

    HAL_LOGF("%s bus switch -> %s (%u/%u)","[MASTER]",(new_bus == CANOPEN_BUS_B_ID) ? "B" : "A",canopen.bus_switch_count + 1u, CANOPEN_MAX_BUS_SWITCHES);

    if (co_nmt_set_active_bus(canopen.nmt, new_bus) != 0) {
        enter_error("co_nmt_set_active_bus failed");
        return;
    }

    canopen.bus_id             = new_bus;
    canopen.bus_switch_count++;
    canopen.slave_hb_timeout   = false;
    canopen.slave_boot_pending = false;
    canopen.bus_state          = CANOPEN_BUS_PREOP;
}


canopen_status canopen_init(canopen_config conf)
{
    canopen.cfg                    = conf;
    canopen.dev                    = NULL;
    canopen.nmt                    = NULL;
    canopen.net                    = NULL;
    canopen.bus_state          = CANOPEN_BUS_PREOP;
    canopen.bus_id             = CANOPEN_BUS_A_ID;
    canopen.bus_switch_count   = 0u;
    canopen.slave_boot_pending = false;
    canopen.slave_hb_timeout   = false;

	//
	alloc_t * const alloc = mempool_init(&pool, memory_pool, sizeof(memory_pool));
	if(alloc == NULL)
	{
		return CANOPEN_ERROR;
	}

	canopen.net = can_net_create(alloc, CANOPEN_BUS_A_ID);
	if(canopen.net == NULL)
	{
		return CANOPEN_ERROR;
	}
	can_net_set_send_func(canopen.net,&can_send_handler , NULL);

	struct timespec zero = {0,0};
	int rc = can_net_set_time(canopen.net, &zero);
	if(rc != 0)
	{
		return CANOPEN_ERROR;
	}
	(void)rc;

	canopen.dev = od_master_init();
	if(canopen.dev == NULL)
	{
		return CANOPEN_ERROR;
	}

	//////////////////// todo: zrobic tak aby tego nie modyfikowac nie zaleznie od rejestrow
//	co_dev_set_val_u16(canopen.dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
//	co_dev_set_val_i32(canopen.dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);
//	co_dev_set_val_i32(canopen.dev, OD_IDX_TARGET_POS, OD_SUBIDX, g_target_pos);
	////////////////////


	canopen.nmt = co_nmt_create(canopen.net,canopen.dev);
	if(canopen.nmt == NULL)
	{
		return CANOPEN_ERROR;
	}

	/* ── debug OD ────────────────────────────────────────────────────── */
	uint32_t nmt_startup = co_dev_get_val_u32(canopen.dev, 0x1F80u, 0x00u);
	HAL_LOGF("[MASTER] 0x1F80 = 0x%08lX", nmt_startup);
	for (uint8_t i = 1u; i <= 10u; i++)
	{
	    uint32_t val = co_dev_get_val_u32(canopen.dev, 0x1F81u, i);
	    if (val != 0u)
	        HAL_LOGF("[MASTER] 0x1F81 sub=0x%02X = 0x%08lX", i, val);
	}
	/* ─────────────────────────────────────────────────────────────────── */
	canopen.csdo = co_csdo_create(canopen.net, canopen.dev, 1u);
	if (canopen.csdo == NULL)
	{
	    return CANOPEN_ERROR;
	}
	co_csdo_start(canopen.csdo);
	//calbacki
//	co_nmt_set_cs_ind(canopen.nmt, &cs_ind, NULL);
	co_nmt_set_hb_ind(canopen.nmt, &hb_ind, NULL);
//	co_nmt_set_ecss_rdn_ind(canopen.nmt, &rdn_ind, NULL);
	co_nmt_set_st_ind(canopen.nmt, &st_ind, NULL);
	co_nmt_set_sync_ind(canopen.nmt, &nmt_sync_ind, NULL);

	co_tpdo_t * tpdo = co_nmt_get_tpdo(canopen.nmt, PDO_NUM);
	if(tpdo == NULL)
	{
		return CANOPEN_ERROR;
	}
	co_tpdo_set_ind(tpdo, &tpdo_ind, NULL);
	co_rpdo_t * rpdo = co_nmt_get_rpdo(canopen.nmt, PDO_NUM);
	if(rpdo == NULL)
	{
		return CANOPEN_ERROR;
	}
    co_rpdo_set_ind(rpdo, &rpdo_ind, NULL);
    co_rpdo_set_err(rpdo, &rpdo_err, NULL);

//	int arc = co_nmt_set_alternate_bus_id(canopen.nmt,CANOPEN_BUS_B_ID);
//	if(arc != 0)
//	{
//		return CANOPEN_ERROR;
//	}
//	(void)arc;

//	set_ecss_timings();
	int ret = co_nmt_cs_ind(canopen.nmt, CO_NMT_CS_RESET_NODE);
	if(ret != 0 )
	{
		return CANOPEN_ERROR;
	}
	ret  = co_nmt_cs_ind(canopen.nmt, CO_NMT_CS_START);
	if(ret != 0 )
	{
		return CANOPEN_ERROR;
	}
	canopen.bus_state = CANOPEN_BUS_OP;
    canopen.cfg.time(&canopen.slave_boot_time);
    HAL_LOGF("%s init OK (ID=0x%02X)","[MASTER]", co_dev_get_id(canopen.dev));
	return CANOPEN_OK;
}






void canopen_process(void)
{
    struct timespec now;
    canopen.cfg.time(&now);
    can_net_set_time(canopen.net, &now);

    struct can_msg msg;
    while (canopen.cfg.recv(CANOPEN_BUS_A_ID, &msg))
    {
        can_net_recv(canopen.net, &msg, CANOPEN_BUS_A_ID);
    }
    while (canopen.cfg.recv(CANOPEN_BUS_B_ID, &msg))
    {
        can_net_recv(canopen.net, &msg, CANOPEN_BUS_B_ID);
    }

    if (canopen.slave_hb_timeout)
    {
        canopen.slave_hb_timeout = false;
        do_bus_switch();
        return;
    }

    switch (canopen.bus_state)
    {
        case CANOPEN_BUS_RESET_NODE:
        	break;
        case CANOPEN_BUS_RESET_COMM:
        	break;
        case CANOPEN_BUS_PREOP:
        {
//            long e = now.tv_sec - canopen.slave_boot_time.tv_sec;
//            if (e >= CANOPEN_SLAVE_BOOT_WAIT_S && canopen.slave_boot_pending)
//            {
//                reinit_slave();
//            }
            break;
        }
        case CANOPEN_BUS_OP:
        	break;
        case CANOPEN_BUS_STOPPED:
            break;
    }


}



// Heartbeat consumer indication  Wywoływany gdy HB consumer (skonfigurowany w OD pod 0x1016) wykrywa zdarzenie dla obserwowanego węzła.
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,void *data)
{
	(void)data;
	const char *cmd_name = "unknown";
	if (reason == CO_NMT_EC_TIMEOUT)
	{

		if (ec_state == CO_NMT_EC_OCCURRED)
		{
            cmd_name = "TIMEOUT";
            if (id == CANOPEN_REMOTE_NODE && canopen.bus_state == CANOPEN_BUS_OP)
            {
                canopen.slave_hb_timeout = true;
                if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
            }
		}
		else
		{
            cmd_name = "RESOLVED";
            if (id == CANOPEN_REMOTE_NODE)
            {
                canopen.bus_switch_count = 0u;
            }
		}
	}
	else if(reason == CO_NMT_EC_STATE)
	{
		 cmd_name = "STATE CHANGE";
	}
	HAL_LOGF("%s,HB command: %s node 0x%02X","[MASTER]", cmd_name,id);
	co_nmt_on_hb(nmt_, id, ec_state, reason);
}


// zmiana stanu innego węzła
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data)
{
	(void)data;

	const char *cmd_name = "unknown";
    if (id != CANOPEN_REMOTE_NODE)
    {
    	return;
    }
	switch (st)
	{
		case CO_NMT_ST_BOOTUP:
		    cmd_name = "BOOT-UP";
		    canopen.slave_boot_pending = true;
		    canopen.slave_hb_timeout   = false;
		    canopen.bus_state          = CANOPEN_BUS_PREOP;
		    canopen.cfg.time(&canopen.slave_boot_time);
		    {
		        static uint32_t last_bootup_tick = 0u;
		        if (!duplicate_node_locked &&
		            HAL_GetTick() - last_bootup_tick < 2000u)
		        {
		            duplicate_node_locked = true;
		            HAL_LOGF("[MASTER] DUPLICATE NODE ID 0x%02X — system LOCKED", id);
		        }
		        last_bootup_tick = HAL_GetTick();
		    }
		    if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
		    break;

		case CO_NMT_ST_STOP:
			cmd_name = "STOPPED";
            canopen.bus_state = CANOPEN_BUS_PREOP;
            if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
			break;
		case CO_NMT_ST_START:
			cmd_name = "OPERATIONAL";
		    if (duplicate_node_locked)
		    {
		        HAL_LOGF("[MASTER] LOCKED — ignoring START from 0x%02X", id);
		        break;   /* ← nie wchodź w RUNNING */
		    }

            canopen.slave_hb_timeout  = false;
            canopen.bus_switch_count  = 0u;
            canopen.bus_state         = CANOPEN_BUS_OP;
            if (canopen.cfg.on_running) canopen.cfg.on_running();

			break;
		case CO_NMT_ST_PREOP:
			cmd_name = "PRE-OPERATIONAL";
		    if (duplicate_node_locked)
		    {
		        HAL_LOGF("[MASTER] LOCKED — ignoring PRE-OP from 0x%02X", id);
		        break;   /* ← nie wysyłaj NMT START */
		    }
		    HAL_LOGF("%s slave Pre-OP -> START", "[MASTER]");
		    if (co_nmt_cs_req(nmt_, CO_NMT_CS_START, CANOPEN_REMOTE_NODE) != 0)
		    {

		        HAL_LOGF("%s NMT START failed", "[MASTER]");
		    }
			break;
	}

	HAL_LOGF("[MASTER] Node 0x%02X state -> %s (0x%02X)", id, cmd_name, st);
}

static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
    (void)nmt_; (void)data;
    if (canopen.cfg.on_sync) canopen.cfg.on_sync(cnt);
}



static void rpdo_ind(co_rpdo_t *pdo, co_unsigned32_t ac,const void *ptr, size_t n, void *data)
{
    (void)pdo; (void)ptr; (void)data;
    if (ac != 0u)
    {
        HAL_LOGF("%s RPDO error 0x%08X","[MASTER]", (unsigned)ac);
        return;
    }
    HAL_LOGF("%s RPDO (n=%u)","[MASTER]", (unsigned)n);
    if (canopen.cfg.on_rpdo)
    {
        uint16_t sw  = co_dev_get_val_u16(canopen.dev, 0x6041u, 0x00u);
        int32_t  pos = co_dev_get_val_i32(canopen.dev, 0x6064u, 0x00u);
        canopen.cfg.on_rpdo(sw, pos);
    }
}
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data)
{
    (void)pdo; (void)data;
    HAL_LOGF("%s RPDO err eec=0x%04X er=0x%02X","[MASTER]", (unsigned)eec, (unsigned)er);
}

static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data)
{
    (void)pdo; (void)ptr; (void)data; (void)n;
    if (ac != 0u)
    {
        HAL_LOGF("%s TPDO error 0x%08X","[MASTER]", (unsigned)ac);
        return;
    }
    HAL_LOGF("%s TPDO sent (n=%u)","[MASTER]", (unsigned)n);
    /* APPLICATION POINT */
}
