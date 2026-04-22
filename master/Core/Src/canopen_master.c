
/**
 * @file canopen_master.c
 * @brief CANopen master layer – implementation.
 *
 * Contains the initialisation sequence, main processing loop, and all
 * internal Lely stack callbacks (NMT state, Heartbeat, SYNC, RPDO, TPDO).
 * The public API is declared in canopen_master.h.
 *
 * @author  YourName
 * @date    2025
 */
#include <canopen_master.h>
#include "usart.h"

/* ── forward declarations of internal callbacks ─────────────────────────── */
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,void *data);
static void rpdo_ind(co_rpdo_t*, co_unsigned32_t ac, const void*, size_t, void*);
static void rpdo_err(co_rpdo_t*, co_unsigned16_t eec, co_unsigned8_t er, void*);
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data);
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
/* ── module-level storage ────────────────────────────────────────────────── */
/** @brief Static memory pool backing all Lely CANopen allocations. */
static uint8_t  memory_pool[CANOPEN_MEMORY_POOL_SIZE] __attribute__((aligned(16)));
/** @brief Lely mempool handle; initialised in @ref canopen_init(). */
static struct   mempool pool;
/**
 * @brief Duplicate Node-ID guard flag.
 *
 * Set to @c true when two BOOTUP frames from the same node arrive within
 * 2000 ms. Once locked the master ignores all subsequent NMT START and
 * PRE-OP events from that node to prevent uncontrolled drive enable.
 */
static bool duplicate_node_locked = false;
/** @brief Global CANopen master state instance (defined in canopen_master.h). */
canopen_t canopen;
/* ── internal logging helpers ────────────────────────────────────────────── */

/**
 * @brief Transmits a NUL-terminated string over UART2 followed by CR+LF.
 * @param[in] s  String to transmit. Silently returns if @c NULL.
 */
static void HAL_LOG(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart2, (uint8_t *)s, (uint16_t)strlen(s), 100);
  const char crlf[] = "\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t *)crlf, 2, 100);
}
/**
 * @brief printf-style UART logger; formats into a 256-byte stack buffer.
 * @param[in] fmt  printf format string.
 * @param[in] ...  Format arguments.
 */
static void HAL_LOGF(const char *fmt, ...)
{
  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  HAL_LOG(buf);
}

/* ── internal helpers ────────────────────────────────────────────────────── */

/**
 * @brief Lely CAN send callback registered with @c can_net_set_send_func().
 *
 * Forwards every outgoing CAN frame to the application HAL layer via
 * @c canopen.cfg.send(). NMT frames (CAN-ID 0x000) are additionally
 * logged for debugging.
 *
 * @param[in] msg     CAN frame to transmit.
 * @param[in] bus_id  Bus index on which the frame should be sent.
 * @param[in] data    User data pointer (unused).
 * @return 0 on success, -1 if the HAL send function reports failure.
 */
static int can_send_handler(const struct can_msg *const msg, const uint_least8_t bus_id, void *const data)
{
    (void)data;
    if (msg->id == 0x000u)
    {
    	HAL_LOGF("[MASTER] NMT frame: id=0x%03lX data[0]=0x%02X data[1]=0x%02X bus=%d",(unsigned long)msg->id,msg->data[0], msg->data[1], (int)bus_id);
    }
    return canopen.cfg.send((uint8_t)bus_id, msg) ? 0 : -1;
}
/**
 * @brief Enters the fatal error state.
 *
 * Logs @p reason, sets @c bus_state to @ref CANOPEN_BUS_STOPPED and
 * fires the @c on_stopped application callback. No recovery is attempted.
 *
 * @param[in] reason  Human-readable description of the failure.
 */
static void enter_error(const char *reason)
{
    HAL_LOGF("%s ERROR: %s","[MASTER]", reason ? reason : "?");
    canopen.bus_state = CANOPEN_BUS_STOPPED;
    if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
}


/**
 * @brief Switches the active CAN bus between A and B.
 *
 * Increments @c bus_switch_count and calls @c co_nmt_set_active_bus().
 * If the maximum number of switches (@ref CANOPEN_MAX_BUS_SWITCHES) is
 * reached, @ref enter_error() is called instead.
 * On success the master transitions to @ref CANOPEN_BUS_PREOP and waits
 * for the slave to re-announce itself via Heartbeat or BOOTUP.
 */
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

/* ── public API ──────────────────────────────────────────────────────────── */

/**
 * @brief Initialises the CANopen master stack.
 *
 * Allocates and configures: memory pool, can_net, Object Dictionary, NMT,
 * CSDO, RPDO and TPDO objects. Registers all internal stack callbacks.
 * Ends by issuing NMT RESET_NODE followed by NMT START, transitioning
 * the master to @c CANOPEN_BUS_OP.
 *
 * @param[in] conf  Layer configuration (HAL functions + application callbacks).
 * @return @ref CANOPEN_OK on success, @ref CANOPEN_ERROR on any allocation
 *         or stack initialisation failure.
 */
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





/**
 * @brief Processes all pending CANopen events – call from the main loop.
 *
 * Responsibilities (in order):
 * -# Updates the stack's internal clock via @c can_net_set_time().
 * -# Drains all pending CAN frames from bus A and bus B.
 * -# Handles a deferred bus switchover when @c slave_hb_timeout is set.
 * -# Runs per-state logic for the current @c bus_state.
 *
 * @note Not thread-safe; must be called from a single execution context.
 */
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


/* ── internal callbacks ──────────────────────────────────────────────────── */

/**
 * @brief Heartbeat consumer indication callback.
 *
 * Registered via @c co_nmt_set_hb_ind(). Called by the Lely stack whenever
 * the HB consumer (OD 0x1016) detects an event for a monitored node.
 *
 * Behaviour:
 * - @c CO_NMT_EC_TIMEOUT / @c CO_NMT_EC_OCCURRED : sets
 *   @c slave_hb_timeout and fires @c on_stopped so the main loop can
 *   trigger a bus switch on the next @ref canopen_process() call.
 * - @c CO_NMT_EC_TIMEOUT / resolved : resets @c bus_switch_count to 0.
 * - @c CO_NMT_EC_STATE : logs the state change only.
 *
 * @param[in] nmt_      NMT master object (forwarded to @c co_nmt_on_hb()).
 * @param[in] id        Node-ID of the node that triggered the event.
 * @param[in] ec_state  Whether the event occurred or was resolved.
 * @param[in] reason    Reason: timeout or state change.
 * @param[in] data      User data pointer (unused).
 */
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


/**
 * @brief NMT state change indication callback.
 *
 * Registered via @c co_nmt_set_st_ind(). Called by the Lely stack when
 * a monitored node broadcasts a new NMT state. Only events from
 * @ref CANOPEN_REMOTE_NODE are processed; all others are silently ignored.
 *
 * State handling:
 * - @c CO_NMT_ST_BOOTUP : records boot time, detects duplicate Node-IDs
 *   (two BOOTUPs within 2 s → system locked), fires @c on_stopped.
 * - @c CO_NMT_ST_STOP : transitions to @ref CANOPEN_BUS_PREOP, fires
 *   @c on_stopped.
 * - @c CO_NMT_ST_START : if not locked, transitions to @ref CANOPEN_BUS_OP
 *   and fires @c on_running.
 * - @c CO_NMT_ST_PREOP : if not locked, sends NMT START to the slave.
 *
 * @param[in] nmt_  NMT master object (used to issue NMT CS requests).
 * @param[in] id    Node-ID of the node whose state changed.
 * @param[in] st    New NMT state (@c CO_NMT_ST_* value).
 * @param[in] data  User data pointer (unused).
 */
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
		        break;
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
		        break;
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
/**
 * @brief NMT SYNC indication callback.
 *
 * Registered via @c co_nmt_set_sync_ind(). Forwards the SYNC counter
 * directly to the application via @c canopen.cfg.on_sync().
 *
 * @param[in] nmt_  NMT master object (unused).
 * @param[in] cnt   SYNC counter value (0–240).
 * @param[in] data  User data pointer (unused).
 */
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
    (void)nmt_; (void)data;
    if (canopen.cfg.on_sync) canopen.cfg.on_sync(cnt);
}


/**
 * @brief RPDO received indication callback.
 *
 * Registered via @c co_rpdo_set_ind(). Called by the Lely stack after a
 * valid RPDO has been mapped into the master OD. Reads the Statusword
 * (OD 0x6041) and actual position (OD 0x6064) from the OD and forwards
 * them to the application via @c canopen.cfg.on_rpdo().
 *
 * @param[in] pdo   RPDO object (unused).
 * @param[in] ac    SDO abort code; non-zero indicates a mapping error.
 * @param[in] ptr   Pointer to raw PDO payload (unused – values read from OD).
 * @param[in] n     Number of bytes in the raw payload.
 * @param[in] data  User data pointer (unused).
 */
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
/**
 * @brief RPDO error callback.
 *
 * Registered via @c co_rpdo_set_err(). Called when the Lely stack detects
 * a PDO length mismatch or other protocol-level RPDO error.
 *
 * @param[in] pdo   RPDO object (unused).
 * @param[in] eec   Emergency error code reported by the stack.
 * @param[in] er    Error register value.
 * @param[in] data  User data pointer (unused).
 */
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data)
{
    (void)pdo; (void)data;
    HAL_LOGF("%s RPDO err eec=0x%04X er=0x%02X","[MASTER]", (unsigned)eec, (unsigned)er);
}
/**
 * @brief TPDO transmission confirmation callback.
 *
 * Registered via @c co_tpdo_set_ind(). Called by the Lely stack after a
 * TPDO has been handed to the CAN driver. Can be used as an application
 * hook to update the next TPDO payload.
 *
 * @param[in] pdo   TPDO object (unused).
 * @param[in] ac    SDO abort code; non-zero indicates a mapping error.
 * @param[in] ptr   Pointer to the transmitted PDO payload (unused).
 * @param[in] n     Number of bytes transmitted.
 * @param[in] data  User data pointer (unused).
 */
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data)
{
    (void)pdo; (void)ptr; (void)data; (void)n;
    if (ac != 0u)
    {
        HAL_LOGF("%s TPDO error 0x%08X","[MASTER]", (unsigned)ac);
        return;
    }
    HAL_LOGF("%s TPDO sent (n=%u)","[MASTER]", (unsigned)n);
}
