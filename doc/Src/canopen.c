/**
 * @file canopen.c
 * @brief CANopen slave layer – implementation.
 *
 * Contains the initialisation sequence, main processing loop, ECSS
 * redundancy helpers, and all internal Lely stack callbacks (NMT command,
 * Heartbeat, ECSS redundancy, NMT state, SYNC, TPDO, RPDO error).
 * The public API is declared in canopen.h.
 *
 * @author
 * @date
 */

#include "canopen.h"
#include "usart.h"
/* ── forward declarations of internal callbacks ─────────────────────────── */
static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data);
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,void *data);
static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,co_nmt_ecss_rdn_reason_t reason, void *data);
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data);
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data);
/* ── module-level storage ────────────────────────────────────────────────── */

/** @brief Static memory pool backing all Lely CANopen allocations. */
static uint8_t  memory_pool[CANOPEN_MEMORY_POOL_SIZE] __attribute__((aligned(16)));

/** @brief Lely mempool handle; initialised in @ref canopen_init(). */
static struct   mempool pool;
/** @brief Global CANopen slave state instance (declared in canopen.h). */
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

/* ── ECSS helpers ────────────────────────────────────────────────────────── */

/**
 * @brief Writes the default active bus to the ECSS redundancy OD entry (0x2000 sub 1).
 *
 * Converts the bus index to the value expected by the Lely ECSS layer:
 * 0 for bus A, 1 for bus B.
 *
 * @param[in] bus_id  Bus index to set as default (0 = A, 1 = B).
 */
static void set_bdefault(uint8_t bus_id)
{
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_BDEFAULT,
                      (bus_id == CANOPEN_BUS_B_ID) ? 1u : 0u);
}
/**
 * @brief Writes the ECSS toggle timeout and toggle count to the OD (0x2000 sub 2/3).
 *
 * Values are taken from the compile-time constants @ref ECSS_TTOGGLE and
 * @ref ECSS_NTOGGLE. Called during initialisation and after every node/comm reset
 * to ensure the OD reflects the intended redundancy timing.
 */
static void set_ecss_timings(void)
{
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_TTOGGLE, ECSS_TTOGGLE);
    co_dev_set_val_u8(canopen.dev, OD_IDX_RDN, OD_RDN_NTOGGLE, ECSS_NTOGGLE);
}
/**
 * @brief Restarts the Heartbeat producer by writing the period to OD 0x1017.
 *
 * Performed via an internal SDO download so the Lely NMT layer picks up
 * the new value immediately. Used after a bus switch to re-synchronise the
 * HB timer to the new active bus.
 */
static void restart_hb(void)
{
    const co_unsigned16_t hb = HB_PRODUCER_MS;
    co_dev_dn_val_req(canopen.dev, 0x1017u, 0x00u,
                      CO_DEFTYPE_UNSIGNED16, &hb, NULL, NULL, NULL);
}

/**
 * @brief Lely CAN send callback registered with @c can_net_set_send_func().
 *
 * Forwards every outgoing CAN frame to the application HAL layer via
 * @c canopen.cfg.send().
 *
 * @param[in] msg     CAN frame to transmit.
 * @param[in] bus_id  Bus index on which the frame should be sent.
 * @param[in] data    User data pointer (unused).
 * @return 0 on success, -1 if the HAL send function reports failure.
 */

static int can_send_handler(const struct can_msg *const msg, const uint_least8_t bus_id, void *const data)
{
    (void)data;
    return canopen.cfg.send((uint8_t)bus_id, msg) ? 0 : -1;
}
/* ── public API ──────────────────────────────────────────────────────────── */
/**
 * @brief Initialises the CANopen slave stack.
 *
 * Allocates and configures: memory pool, can_net, Object Dictionary, NMT,
 * RPDO and TPDO objects. Registers all internal stack callbacks and writes
 * the ECSS redundancy parameters (OD 0x2000) to the OD.
 * Ends by issuing NMT RESET_NODE, transitioning the slave to Pre-Operational
 * and then to Operational once the master issues NMT START.
 *
 * @param[in] conf  Layer configuration (HAL functions + application callbacks).
 * @return @ref CANOPEN_OK on success, @ref CANOPEN_ERROR on any failure.
 */
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




/**
 * @brief Processes all pending CANopen events – call from the main loop.
 *
 * Responsibilities (in order):
 * -# Advances the Lely stack clock to the current monotonic time.
 * -# Drains all pending CAN frames from bus A then bus B.
 * -# Executes a deferred communication reset if @c reset_comm_pending is set.
 *
 * @note Not thread-safe; call only from a single execution context.
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

    if (canopen.bus_state == CANOPEN_BUS_STOPPED &&
        canopen.reset_comm_pending) {
        canopen.reset_comm_pending = false;
        co_nmt_cs_ind(canopen.nmt, CO_NMT_CS_RESET_COMM);
    }
}


/* ── internal callbacks ──────────────────────────────────────────────────── */


/**
 * @brief NMT command service indication callback.
 *
 * Registered via @c co_nmt_set_cs_ind(). Called by the Lely stack whenever
 * the slave node receives an NMT command (START, STOP, ENTER_PREOP,
 * RESET_NODE, RESET_COMM).
 *
 * State handling:
 * - @c CO_NMT_CS_START       : transitions to @ref CANOPEN_BUS_OP and fires
 *                              @c on_running.
 * - @c CO_NMT_CS_STOP        : fires @c on_stopped.
 * - @c CO_NMT_CS_RESET_NODE  : resets all runtime fields to defaults, resets
 *                              the active bus to A, and re-applies ECSS timings.
 * - @c CO_NMT_CS_RESET_COMM  : either a normal comm reset (resets bus to A) or
 *                              a recovery reset (restores @c saved_bus_id), depending
 *                              on @c reset_comm_is_recovery.
 *
 * @param[in] nmt_  NMT slave object (unused).
 * @param[in] cs    NMT command specifier (@c CO_NMT_CS_* value).
 * @param[in] data  User data pointer (unused).
 */
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


/**
 * @brief Heartbeat consumer indication callback.
 *
 * Registered via @c co_nmt_set_hb_ind(). Called by the Lely stack when the
 * HB consumer (OD 0x1016) detects an event for a monitored node.
 *
 * Behaviour:
 * - @c CO_NMT_EC_TIMEOUT / @c CO_NMT_EC_OCCURRED : master contact lost –
 *   fires @c on_stopped to enter safe state.
 * - @c CO_NMT_EC_TIMEOUT / resolved : if the master comes back while the
 *   slave is in @ref CANOPEN_BUS_STOPPED and the NMT state is already
 *   START, recovers directly to @ref CANOPEN_BUS_OP without a RESET_COMM.
 * - @c CO_NMT_EC_STATE : logs the state change only.
 *
 * @param[in] nmt_      NMT slave object (forwarded to @c co_nmt_on_hb()).
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
    else if (reason == CO_NMT_EC_STATE)
    {
        cmd_name = "STATE CHANGE";
        HAL_LOGF("[SLAVE] hb_ind STATE CHANGE id=0x%02X new_st=%d",
                 id, (int)ec_state);
    }
	HAL_LOGF("%s,HB command: %s","[SLAVE]", cmd_name);
	co_nmt_on_hb(nmt_, id, ec_state, reason);
}

/**
 * @brief ECSS redundancy indication callback.
 *
 * Registered via @c co_nmt_set_ecss_rdn_ind(). Called by the Lely stack
 * when the ECSS redundancy manager reports an event.
 *
 * Behaviour:
 * - @c CO_NMT_ECSS_RDN_BUS_SWITCH : updates the active bus index, writes
 *   the new default bus to the OD, restarts the HB producer, and transitions
 *   to @ref CANOPEN_BUS_PREOP. The slave waits for the master's NMT START
 *   before re-entering OPERATIONAL.
 * - @c CO_NMT_ECSS_RDN_NO_MASTER : saves the current bus index, schedules a
 *   recovery RESET_COMM (via @c reset_comm_pending), enters
 *   @ref CANOPEN_BUS_STOPPED, and fires @c on_stopped. The actual RESET_COMM
 *   is executed from @ref canopen_process().
 *
 * @param[in] nmt_    NMT slave object (unused).
 * @param[in] bus_id  Bus index reported by the redundancy manager.
 * @param[in] reason  Redundancy event (@c CO_NMT_ECSS_RDN_* value).
 * @param[in] data    User data pointer (unused).
 */
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
			break;
		case CO_NMT_ECSS_RDN_NO_MASTER:
			cmd_name = "NO MASTER";
            canopen.saved_bus_id           = canopen.bus_id;
            canopen.reset_comm_is_recovery = true;
            canopen.reset_comm_pending     = true;
            canopen.bus_state = CANOPEN_BUS_STOPPED;
            if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
			break;
  }
	HAL_LOGF("%s,RDN command: %s","[SLAVE]", cmd_name);
}



/**
 * @brief NMT state change indication callback for remote nodes.
 *
 * Registered via @c co_nmt_set_st_ind(). Called when a monitored node
 * broadcasts a new NMT state. Only events from @ref CANOPEN_MASTER_NODE
 * are processed; all others are silently ignored.
 *
 * State handling:
 * - @c CO_NMT_ST_BOOTUP : master has rebooted – if slave was OPERATIONAL,
 *   transitions to @ref CANOPEN_BUS_PREOP and fires @c on_stopped.
 * - @c CO_NMT_ST_STOP   : master stopped – same transition as BOOTUP.
 * - @c CO_NMT_ST_START  : master is OPERATIONAL – if slave is Pre-Op,
 *   issues NMT START to itself and fires @c on_running; if slave is in
 *   STOPPED (fallback path), recovers directly to @ref CANOPEN_BUS_OP.
 * - @c CO_NMT_ST_PREOP  : logged only, no action.
 *
 * @param[in] nmt_  NMT slave object (used to issue NMT CS_START to self).
 * @param[in] id    Node-ID of the node whose state changed.
 * @param[in] st    New NMT state (@c CO_NMT_ST_* value).
 * @param[in] data  User data pointer (unused).
 */
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
				if (canopen.cfg.on_stopped) canopen.cfg.on_stopped();
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
/**
 * @brief NMT SYNC indication callback.
 *
 * Registered via @c co_nmt_set_sync_ind(). Forwards the SYNC counter
 * directly to the application via @c canopen.cfg.on_sync().
 *
 * @param[in] nmt_  NMT slave object (unused).
 * @param[in] cnt   SYNC counter value (0–240).
 * @param[in] data  User data pointer (unused).
 */
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
    (void)nmt_; (void)data;
    if (canopen.cfg.on_sync) canopen.cfg.on_sync(cnt);
}




/**
 * @brief TPDO transmission confirmation callback.
 *
 * Registered via @c co_tpdo_set_ind(). Called after a TPDO has been handed
 * to the CAN driver. Forwards the event to the application via
 * @c canopen.cfg.on_tpdo() so it can update the next TPDO payload in the OD.
 *
 * @param[in] pdo   TPDO object (unused).
 * @param[in] ac    SDO abort code; non-zero indicates a mapping error –
 *                  silently ignored here.
 * @param[in] ptr   Pointer to the transmitted PDO payload (unused).
 * @param[in] n     Number of bytes transmitted (unused).
 * @param[in] data  User data pointer (unused).
 */
static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data)
{
    (void)pdo; (void)ptr; (void)data; (void)n;
    if (ac != 0u) return;
    if (canopen.cfg.on_tpdo) canopen.cfg.on_tpdo();
}
/**
 * @brief RPDO error callback.
 *
 * Registered via @c co_rpdo_set_err(). Called when the Lely stack detects
 * a PDO length mismatch or other protocol-level RPDO error. Logs the
 * emergency error code and error register for diagnostics.
 *
 * @param[in] pdo   RPDO object (unused).
 * @param[in] eec   Emergency error code reported by the stack.
 * @param[in] er    Error register value.
 * @param[in] data  User data pointer (unused).
 */
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec,co_unsigned8_t er, void *data)
{
    (void)pdo; (void)data;
    HAL_LOGF("[SLAVE] RPDO ERROR eec=0x%04X er=0x%02X",
             (unsigned)eec, (unsigned)er);
}
