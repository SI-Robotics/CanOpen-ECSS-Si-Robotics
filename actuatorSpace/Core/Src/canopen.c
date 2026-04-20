#include "canopen.h"
#include "usart.h"

static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data);
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,void *data);
static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,co_nmt_ecss_rdn_reason_t reason, void *data);
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data);
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data);

static uint8_t  memory_pool[CANOPEN_MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static struct   mempool pool;

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

/* ── ECSS helpers ────────────────────────────────────────────────────────── */
static void set_bdefault(uint8_t bus_id)
{
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_BDEFAULT,
                      (bus_id == CANOPEN_BUS_B_ID) ? 1u : 0u);
}

static void set_ecss_timings(void)
{
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_TTOGGLE, ECSS_TTOGGLE);
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_NTOGGLE, ECSS_NTOGGLE);
}

static void restart_hb(void)
{
    const co_unsigned16_t hb = HB_PRODUCER_MS;
    co_dev_dn_val_req(canopen.dev, 0x1017u, 0x00u,
                      CO_DEFTYPE_UNSIGNED16, &hb, NULL, NULL, NULL);
}

//void HAL_Clock_GetTime(struct timespec *tp)
//{
//  if (!tp) return;
//  uint32_t ms = HAL_GetTick();
//  tp->tv_sec  = (time_t)(ms / 1000u);
//  tp->tv_nsec = (long)((ms % 1000u) * 1000000L);
//}
//

static int can_send_handler(const struct can_msg *const msg, const uint_least8_t bus_id, void *const data)
{
    (void)data;
    return canopen.cfg.send((uint8_t)bus_id, msg) ? 0 : -1;
}


canopen_status canopen_init(canopen_config conf)
{
    canopen.cfg                    = conf;
    canopen.dev                    = NULL;
    canopen.nmt                    = NULL;
    canopen.net                    = NULL;
    canopen.bus_state              = CANOPEN_BUS_RESET_NODE;
    canopen.bus_id                 = CANOPEN_BUS_A_ID;
    canopen.reset_comm_pending     = false;
    canopen.reset_comm_is_recovery = false;
    canopen.saved_bus_id           = CANOPEN_BUS_A_ID;

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

	canopen.dev = od_slave_init();
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
	//calbacki
	co_nmt_set_cs_ind(canopen.nmt, &cs_ind, NULL);
	co_nmt_set_hb_ind(canopen.nmt, &hb_ind, NULL);
	co_nmt_set_ecss_rdn_ind(canopen.nmt, &rdn_ind, NULL);
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
	co_rpdo_set_err(rpdo, &rpdo_err, NULL);
	int arc = co_nmt_set_alternate_bus_id(canopen.nmt,CANOPEN_BUS_B_ID);
	if(arc != 0)
	{
		return CANOPEN_ERROR;
	}
	(void)arc;

	set_ecss_timings();
	int ret = co_nmt_cs_ind(canopen.nmt, CO_NMT_CS_RESET_NODE);
	if(ret != 0 )
	{
		return CANOPEN_ERROR;
	}
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

    if (canopen.bus_state == CANOPEN_BUS_STOPPED &&
        canopen.reset_comm_pending) {
        canopen.reset_comm_pending = false;
        co_nmt_cs_ind(canopen.nmt, CO_NMT_CS_RESET_COMM);
    }
}











/* ══════════════════════════════════════════════════════════════════════════
 * callbacks
 * ══════════════════════════════════════════════════════════════════════════ */

//Wywoływany gdy węzeł dostaje komendę NMT
static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data)
{
	(void)nmt_;
	(void)data;
	const char *cmd_name = "unknown";
	switch (cs)
	{
		case CO_NMT_CS_START:
			cmd_name = "START";
            if (canopen.bus_state == CANOPEN_BUS_STOPPED || canopen.bus_state == CANOPEN_BUS_PREOP)
            {
                canopen.reset_comm_pending     = false;
                canopen.reset_comm_is_recovery = false;
                canopen.bus_state = CANOPEN_BUS_OP;
                if (canopen.cfg.on_running) canopen.cfg.on_running();
            }
			break;
		case CO_NMT_CS_STOP:
			cmd_name = "STOP";
			if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
			break;
		case CO_NMT_CS_ENTER_PREOP:
			cmd_name = "ENTER PRE-OP";
			break;
		case CO_NMT_CS_RESET_NODE:
			cmd_name = "RESET NODE";
            canopen.bus_state              = CANOPEN_BUS_PREOP;
            canopen.bus_id                 = CANOPEN_BUS_A_ID;
            canopen.reset_comm_pending     = false;
            canopen.reset_comm_is_recovery = false;
//            co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_BDEFAULT,(bus_id == CANOPEN_BUS_B_ID) ? 1u : 0u);
            set_bdefault(CANOPEN_BUS_A_ID);
//            co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_TTOGGLE, ECSS_TTOGGLE);
//            co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_NTOGGLE, ECSS_NTOGGLE);
            set_ecss_timings();
			break;
		case CO_NMT_CS_RESET_COMM:

            if (canopen.reset_comm_is_recovery)
            {
            	cmd_name = "RESET COMM RECOVERY";
                canopen.reset_comm_is_recovery = false;
                canopen.reset_comm_pending     = false;
                canopen.bus_state = CANOPEN_BUS_PREOP;
                set_bdefault(canopen.saved_bus_id);
                set_ecss_timings();
            }
            else
            {
            	cmd_name = "RESET COMM NORMAL";
                canopen.bus_state              = CANOPEN_BUS_PREOP;
                canopen.bus_id                 = CANOPEN_BUS_A_ID;
                canopen.reset_comm_pending     = false;
                canopen.reset_comm_is_recovery = false;
                set_bdefault(CANOPEN_BUS_A_ID);
                set_ecss_timings();
            }
			break;
	};
	HAL_LOGF("%s,NMT command: %s (0x%02X)","[SLAVE]", cmd_name, cs);

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
			cmd_name = "NO CONTACT WITH MASTER";
			uint32_t tick = HAL_GetTick();
            HAL_LOGF("[SLAVE] t=%lums HB TIMEOUT id=0x%02X bus_state=%d", tick, id,(int)canopen.bus_state);
		    if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
            HAL_LOGF("[SLAVE] t=%lums safe state entered", HAL_GetTick());
		    (void)id;
		}
		else
		{
			cmd_name = "MASTER COME BACK";
            /* HB resolved — fallback gdy RESET_COMM nie zdążył */
            if (id == CANOPEN_MASTER_NODE && canopen.bus_state == CANOPEN_BUS_STOPPED)
            {
            	cmd_name = "MASTER COME BACK WITHOUT RESET COMM";
                if (co_nmt_get_st(canopen.nmt) == CO_NMT_ST_START)
                {
                    canopen.reset_comm_pending     = false;
                    canopen.reset_comm_is_recovery = false;
                    canopen.bus_state = CANOPEN_BUS_OP;
                }
            }
		}
	}
    else if (reason == CO_NMT_EC_STATE)   /* ← to generuje "unknown" */
    {
        cmd_name = "STATE CHANGE";
        HAL_LOGF("[SLAVE] hb_ind STATE CHANGE id=0x%02X new_st=%d",
                 id, (int)ec_state);
    }
	HAL_LOGF("%s,HB command: %s","[SLAVE]", cmd_name);
	co_nmt_on_hb(nmt_, id, ec_state, reason);
}

// ECSS Redundancy indication
static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,co_nmt_ecss_rdn_reason_t reason, void *data)
{
	(void)nmt_;
	(void)data;
	const char *cmd_name = "unknown";

	switch (reason)
	{
		case CO_NMT_ECSS_RDN_BUS_SWITCH:
			cmd_name = "BUS SWITCH";
            canopen.bus_id    = bus_id;
            canopen.bus_state = CANOPEN_BUS_PREOP;
            set_bdefault(bus_id);
            restart_hb();
            /* nie wchodzimy w OP — czekamy na st_ind(MASTER, OP) */
			break;
		case CO_NMT_ECSS_RDN_NO_MASTER:
			cmd_name = "NO MASTER";
            canopen.saved_bus_id           = canopen.bus_id;
            canopen.reset_comm_is_recovery = true;
            canopen.reset_comm_pending     = true;
            canopen.bus_state = CANOPEN_BUS_STOPPED;
            if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
            /* RESET_COMM wykona się z canopen_process() */
			break;
  }
	HAL_LOGF("%s,RDN command: %s","[SLAVE]", cmd_name);
}



// zmiana stanu innego węzła
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data)
{
	(void)data;

	const char *cmd_name = "unknown";
    if (id != CANOPEN_MASTER_NODE)
    {
    	return;
    }
	switch (st)
	{
		case CO_NMT_ST_BOOTUP:
			cmd_name = "BOOT-UP";
			if(canopen.bus_state == CANOPEN_BUS_OP)
			{
				canopen.bus_state = CANOPEN_BUS_PREOP;
				if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();  /* ← brakuje */
			}
			break;
		case CO_NMT_ST_STOP:
			cmd_name = "STOPPED";
			if(canopen.bus_state == CANOPEN_BUS_OP)
			{
				canopen.bus_state = CANOPEN_BUS_PREOP;
				if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
			}
			break;
		case CO_NMT_ST_START:
			cmd_name = "START";
	        if (co_nmt_get_st(canopen.nmt) == CO_NMT_ST_PREOP)
	        {
	            co_nmt_cs_ind(nmt_, CO_NMT_CS_START);
	            canopen.reset_comm_pending     = false;
	            canopen.reset_comm_is_recovery = false;
	            canopen.bus_state = CANOPEN_BUS_OP;
	            if (canopen.cfg.on_running) canopen.cfg.on_running();
	        }
	        else if (canopen.bus_state == CANOPEN_BUS_STOPPED)
	        {
	            /* fallback */
	            canopen.reset_comm_pending     = false;
	            canopen.reset_comm_is_recovery = false;
	            canopen.bus_state = CANOPEN_BUS_OP;
	            if (canopen.cfg.on_running) canopen.cfg.on_running();
	        }


			break;
		case CO_NMT_ST_PREOP:
			cmd_name = "PRE-OPERATIONAL";
			break;
	}

	HAL_LOGF("[SLAVE] Node 0x%02X state -> %s (0x%02X)", id, cmd_name, st);
}

static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
    (void)nmt_; (void)data;
    if (canopen.cfg.on_sync) canopen.cfg.on_sync(cnt);
}





static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data)
{
    (void)pdo; (void)ptr; (void)data; (void)n;
    if (ac != 0u) return;
    if (canopen.cfg.on_tpdo) canopen.cfg.on_tpdo();
}

static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data)
{
    (void)pdo; (void)data;
    HAL_LOGF("[SLAVE] RPDO ERROR eec=0x%04X er=0x%02X",
             (unsigned)eec, (unsigned)er);
}
