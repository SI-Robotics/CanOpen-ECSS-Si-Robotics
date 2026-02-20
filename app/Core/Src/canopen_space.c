/**
 * @file canopen_space.c
 * @brief Implementation of lightweight CANopen/Lely-core wrapper (init/start/tick/rx/tx).
 */
#include "canopen_space.h"
/* Single static instance. */
static canopen_space_ctx_t g_ctx = {0};

/**
 * @brief Trap into a fatal error state (debug helper).
 * @param code Custom failure code stored in g_ctx.fail_code.
 */
static void die(uint32_t code)
{
  g_ctx.fail_code = code;
  volatile int errc = get_errc();
  (void)errc;
//  __BKPT(0);
  while (1) {}
}
/**
 * @brief Initialize the OD depending on role (master vs actuator).
 * @param role canopen_space role
 * @return co_dev_t* pointer to initialized OD or NULL.
 */
static co_dev_t* od_init_for_role(canopen_space_role_t role)
{
  if (role == CO_ROLE_ACTUATOR) {
    return od_slave_init();
  } else {
    return od_master_init();
  }
}
/**
 * @brief Add 1ms to timespec (bare-metal timebase).
 * @param ts Timespec to update.
 */
static void time_add_1ms(struct timespec* ts)
{
  ts->tv_nsec += 1000000L;
  if (ts->tv_nsec >= 1000000000L) {
    ts->tv_nsec -= 1000000000L;
    ts->tv_sec += 1;
  }
}
/**
 * @brief Lely "next deadline" callback (not used in this bare-metal integration).
 *
 * tp == NULL means "no deadline". If you later integrate with a HW timer,
 * you can use *tp here.
 */
static int can_next_func(const struct timespec* tp, void* data)
{
  (void)data;
  (void)tp;
  return 0;
}
/**
 * @brief Lely CAN send callback, forwards frames to user transport function.
 *
 * @param msg   CAN frame prepared by Lely.
 * @param bus_id Bus identifier.
 * @param data  Unused user data (we use g_ctx).
 * @return 0 on success, <0 on error.
 */
static int can_send_func(const struct can_msg* msg, uint_least8_t bus_id, void* data)
{
  (void)data;

  if (!msg || !g_ctx.send) {
    g_ctx.fail_code = 0xE100;
//    __BKPT(0);
    return -1;
  }

  g_ctx.last_tx_id = msg->id;
  return g_ctx.send(bus_id, msg, g_ctx.send_user);
}
/**
 * @brief Configure PDO mapping for ACTUATOR at runtime (RPDO1 and TPDO1).
 *
 * This sets:
 * - TPDO1 COB-ID = 0x180 + node, mapping: 0x6041:16 + 0x6064:32, event-driven
 * - RPDO1 COB-ID = 0x200 + node, mapping: 0x6040:16 + 0x607A:32
 *
 * @param dev     Object Dictionary.
 * @param node_id Local node id used to compute COB-IDs.
 */
static void pdo_config_slave_runtime(co_dev_t* dev, uint8_t node_id)
{
  uint32_t tpdo_cobid     = 0x180u + node_id;
  uint32_t tpdo_cobid_dis = tpdo_cobid | 0x80000000u;
  uint8_t  tt_event = 255;
  uint16_t inhibit = 0;
  uint16_t event_ms = 0;

  (void)co_dev_set_val(dev, 0x1800, 0x01, &tpdo_cobid_dis, sizeof(tpdo_cobid_dis));
  uint8_t n0 = 0;
  (void)co_dev_set_val(dev, 0x1A00, 0x00, &n0, sizeof(n0));
  uint32_t map1 = 0x60410010u; // statusword
  uint32_t map2 = 0x60640020u; // position actual
  (void)co_dev_set_val(dev, 0x1A00, 0x01, &map1, sizeof(map1));
  (void)co_dev_set_val(dev, 0x1A00, 0x02, &map2, sizeof(map2));
  uint8_t n2 = 2;
  (void)co_dev_set_val(dev, 0x1A00, 0x00, &n2, sizeof(n2));
  (void)co_dev_set_val(dev, 0x1800, 0x02, &tt_event, sizeof(tt_event));
  (void)co_dev_set_val(dev, 0x1800, 0x03, &inhibit, sizeof(inhibit));
  (void)co_dev_set_val(dev, 0x1800, 0x05, &event_ms, sizeof(event_ms));
  (void)co_dev_set_val(dev, 0x1800, 0x01, &tpdo_cobid, sizeof(tpdo_cobid));
  uint32_t rpdo_cobid     = 0x200u + node_id;
  uint32_t rpdo_cobid_dis = rpdo_cobid | 0x80000000u;
  (void)co_dev_set_val(dev, 0x1400, 0x01, &rpdo_cobid_dis, sizeof(rpdo_cobid_dis));
  (void)co_dev_set_val(dev, 0x1600, 0x00, &n0, sizeof(n0));
  uint32_t rmap1 = 0x60400010u; // controlword
  uint32_t rmap2 = 0x607A0020u; // target position
  (void)co_dev_set_val(dev, 0x1600, 0x01, &rmap1, sizeof(rmap1));
  (void)co_dev_set_val(dev, 0x1600, 0x02, &rmap2, sizeof(rmap2));
  (void)co_dev_set_val(dev, 0x1600, 0x00, &n2, sizeof(n2));
  (void)co_dev_set_val(dev, 0x1400, 0x01, &rpdo_cobid, sizeof(rpdo_cobid));
}

/**
 * @brief Initialize the CANopen/Lely stack instance using the provided configuration.
 *
 * Creates mempool, can_net, OD (master or slave), configures heartbeat objects,
 * creates NMT and (optionally) PDO objects for ACTUATOR role.
 *
 * @param cfg Pointer to init configuration.
 * @retval 0  Success.
 * @retval <0 Parameter/initialization error.
 */
int canopen_space_init(const canopen_space_cfg_t* cfg)
{
  if (!cfg) return -1;
  if (!cfg->send) return -2;
  if (!cfg->mem || !cfg->mem_size) return -3;
  memset(&g_ctx, 0, sizeof(g_ctx));
  g_ctx.role      = cfg->role;
  g_ctx.node_id   = cfg->node_id;
  g_ctx.send      = cfg->send;
  g_ctx.send_user = cfg->send_user;
  g_ctx.pool_mem  = (uint8_t*)cfg->mem;
  g_ctx.pool_size = cfg->mem_size;

  alloc_t* alloc = mempool_init(&g_ctx.pool, g_ctx.pool_mem, g_ctx.pool_size);
  if (!alloc) die(1);
  g_ctx.net = can_net_create(alloc, 0);
  if (!g_ctx.net) die(2);
  can_net_set_send_func(g_ctx.net, &can_send_func, NULL);
  can_net_set_next_func(g_ctx.net, &can_next_func, NULL);
  g_ctx.ts.tv_sec  = 0;
  g_ctx.ts.tv_nsec = 1000000L;
  (void)can_net_set_time(g_ctx.net, &g_ctx.ts);
  g_ctx.dev = od_init_for_role(g_ctx.role);
  if (!g_ctx.dev) die(3);
  (void)co_dev_set_id(g_ctx.dev, (co_unsigned8_t)g_ctx.node_id);
  if (g_ctx.role == CO_ROLE_ACTUATOR)
  {
    const co_unsigned16_t hb_ms = 100;
    if (co_dev_set_val(g_ctx.dev, 0x1017, 0x00, &hb_ms, sizeof(hb_ms)) != sizeof(hb_ms))
      die(34);
  }
  else
  {
    const co_unsigned8_t  n = 1;
    const co_unsigned32_t hb_cons = ((co_unsigned32_t)300 << 16) | (co_unsigned32_t)2;

    if (co_dev_set_val(g_ctx.dev, 0x1016, 0x00, &n, sizeof(n)) != sizeof(n))
      die(40);
    if (co_dev_set_val(g_ctx.dev, 0x1016, 0x01, &hb_cons, sizeof(hb_cons)) != sizeof(hb_cons))
      die(41);
  }
  set_errc(0);
  g_ctx.nmt = co_nmt_create(g_ctx.net, g_ctx.dev);
  if (!g_ctx.nmt) die(6);
  if (g_ctx.role == CO_ROLE_ACTUATOR)
  {
    pdo_config_slave_runtime(g_ctx.dev, g_ctx.node_id);

    g_ctx.rpdo1 = co_rpdo_create(g_ctx.net, g_ctx.dev, 1);
    g_ctx.tpdo1 = co_tpdo_create(g_ctx.net, g_ctx.dev, 1);
    if (!g_ctx.rpdo1 || !g_ctx.tpdo1) die(62);
  } else {
    g_ctx.rpdo1 = NULL;
    g_ctx.tpdo1 = NULL;
  }

  return 0;
}
/**
 * @brief Start the stack services depending on role.
 *
 * For ACTUATOR role this typically starts RPDO/TPDO objects and transitions NMT states
 * (Reset comm -> Pre-op -> Operational).
 * For MASTER role it performs minimal local NMT init (implementation-defined).
 *
 * @return 0 on success, <0 on error.
 */
int canopen_space_start(void)
{
  if (!g_ctx.net || !g_ctx.dev || !g_ctx.nmt) return -1;
  if (g_ctx.role == CO_ROLE_ACTUATOR)
  {
    if (g_ctx.rpdo1) (void)co_rpdo_start(g_ctx.rpdo1);
    if (g_ctx.tpdo1) (void)co_tpdo_start(g_ctx.tpdo1);
    (void)co_nmt_cs_ind(g_ctx.nmt, CO_NMT_CS_RESET_COMM);
    (void)co_nmt_cs_ind(g_ctx.nmt, CO_NMT_CS_ENTER_PREOP);
    (void)co_nmt_cs_ind(g_ctx.nmt, CO_NMT_CS_START);
  }
  else
  {
	  (void)co_nmt_cs_ind(g_ctx.nmt, CO_NMT_CS_RESET_COMM);
  }
  return 0;
}
/**
 * @brief Advance stack time by 1 ms (must be called every 1 ms).
 *
 * Updates internal timespec and calls can_net_set_time().
 * This is critical on bare-metal targets without POSIX clocks.
 */
void canopen_space_tick_1ms(void)
{
  if (!g_ctx.net) return;

  time_add_1ms(&g_ctx.ts);
  if (can_net_set_time(g_ctx.net, &g_ctx.ts) < 0) {
    g_ctx.fail_code = 0xE200;
//    __BKPT(0);
  }
}
/**
 * @brief Feed an incoming CAN frame into the Lely stack.
 *
 * Call this from your CAN RX interrupt/callback (after copying payload).
 *
 * @param bus_id Bus identifier.
 * @param id     CAN identifier.
 * @param ext    Extended ID flag.
 * @param rtr    Remote transmission request flag.
 * @param data   Payload pointer (may be NULL if len=0).
 * @param len    Payload length (0..8).
 */
void canopen_space_on_rx(uint_least8_t bus_id, uint32_t id, bool ext, bool rtr, const uint8_t* data, uint8_t len)
{
  if (!g_ctx.net) return;

  g_ctx.last_rx_id = id;
  struct can_msg m = CAN_MSG_INIT;
  m.id = id;
  if (ext) m.flags |= CAN_FLAG_IDE;
  if (rtr) m.flags |= CAN_FLAG_RTR;

  if (len > 8) len = 8;
  m.len = len;

  if (len && data) {
    memcpy(m.data, data, len);
  }
  (void)can_net_recv(g_ctx.net, &m, bus_id);
}
/**
 * @brief Send an expedited SDO download (u32) to a node (COB-ID 0x600 + node_id).
 *
 * Builds an 8-byte "Download Initiate" request with command specifier 0x23:
 * - expedited transfer
 * - size indicated
 * - 4 data bytes (u32)
 *
 * Payload layout:
 * - byte 0 : 0x23 (SDO Download Initiate, expedited, size=4)
 * - byte 1 : index low
 * - byte 2 : index high
 * - byte 3 : subindex
 * - byte 4..7 : value (little-endian)
 *
 * @param bus_id  Bus identifier used by canopen_space transport.
 * @param node_id Destination node ID.
 * @param idx     Object dictionary index.
 * @param sub     Object dictionary subindex.
 * @param val     32-bit value to write.
 *
 * @retval 0  Success.
 * @retval <0 Transport error.
 */
int canopen_space_sdo_write_u32(uint8_t bus_id,uint8_t node_id,uint16_t idx,uint8_t sub,uint32_t val)
{
  uint32_t cobid = 0x600u + (uint32_t)node_id;

  uint8_t d[8];
  d[0] = 0x23;
  d[1] = (uint8_t)(idx & 0xFF);
  d[2] = (uint8_t)((idx >> 8) & 0xFF);
  d[3] = sub;
  d[4] = (uint8_t)(val & 0xFF);
  d[5] = (uint8_t)((val >> 8) & 0xFF);
  d[6] = (uint8_t)((val >> 16) & 0xFF);
  d[7] = (uint8_t)((val >> 24) & 0xFF);

  return canopen_space_send_raw(bus_id, cobid, false, false, d, 8);
}
/**
 * @brief Send a raw CAN frame using the user TX callback.
 *
 * @param bus_id Bus identifier.
 * @param id     CAN identifier.
 * @param ext    Extended ID flag.
 * @param rtr    RTR flag.
 * @param data   Payload pointer (may be NULL if len=0).
 * @param len    Payload length (0..8).
 * @return 0 on success, <0 on error.
 */
int canopen_space_send_raw(uint_least8_t bus_id,uint32_t id,bool ext,bool rtr, const uint8_t* data,uint8_t len)
{
  if (!g_ctx.send) return -1;

  struct can_msg m = CAN_MSG_INIT;
  m.id = id;
  if (ext) m.flags |= CAN_FLAG_IDE;
  if (rtr) m.flags |= CAN_FLAG_RTR;

  if (len > 8) len = 8;
  m.len = len;

  if (len && data) memcpy(m.data, data, len);

  g_ctx.last_tx_id = id;
  return g_ctx.send(bus_id, &m, g_ctx.send_user);
}
/**
 * @brief Get pointer to the initialized Object Dictionary (OD).
 * @return OD pointer or NULL if not initialized.
 */
co_dev_t*  canopen_space_dev(void)
{
	return g_ctx.dev;
}
/**
 * @brief Get pointer to the NMT object.
 * @return NMT pointer or NULL if not initialized.
 */
co_nmt_t*  canopen_space_nmt(void)
{
	return g_ctx.nmt;
}
/**
 * @brief Get pointer to TPDO1 (actuator role).
 * @return TPDO1 pointer or NULL if not created.
 */
co_tpdo_t* canopen_space_tpdo1(void)
{
	return g_ctx.tpdo1;
}
/**
 * @brief Get pointer to RPDO1 (actuator role).
 * @return RPDO1 pointer or NULL if not created.
 */
co_rpdo_t* canopen_space_rpdo1(void)
{
	return g_ctx.rpdo1;
}



