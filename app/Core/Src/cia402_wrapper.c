/**
 * @file cia402_wrapper.c
 * @brief Implementation of minimal CiA-402 helper wrapper for RPDO1/TPDO1.
 */

#include "cia402_wrapper.h"

/* Master-side registers (commands + feedback). */
static cia402_regs_t g_m = {0};
/* Actuator-side registers (commands from OD + feedback to OD). */
static cia402_regs_t g_a = {0};
/* ========================= Little-endian helpers ========================= */

/**
 * @brief Write 16-bit unsigned value in little-endian order.
 * @param p Destination buffer (at least 2 bytes).
 * @param v Value to write.
 */
static inline void u16_le(uint8_t* p, uint16_t v)
{
  p[0] = (uint8_t)(v & 0xFF);
  p[1] = (uint8_t)((v >> 8) & 0xFF);
}
/**
 * @brief Write 32-bit unsigned value in little-endian order.
 * @param p Destination buffer (at least 4 bytes).
 * @param v Value to write.
 */
static inline void u32_le(uint8_t* p, uint32_t v)
{
  p[0] = (uint8_t)(v & 0xFF);
  p[1] = (uint8_t)((v >> 8) & 0xFF);
  p[2] = (uint8_t)((v >> 16) & 0xFF);
  p[3] = (uint8_t)((v >> 24) & 0xFF);
}
/**
 * @brief Write 32-bit signed value in little-endian order.
 * @param p Destination buffer (at least 4 bytes).
 * @param v Value to write.
 */
static inline void i32_le(uint8_t* p, int32_t v)
{
  u32_le(p, (uint32_t)v);
}
/**
 * @brief Read 16-bit unsigned little-endian value.
 * @param p Source buffer (at least 2 bytes).
 * @return Decoded value.
 */
static inline uint16_t le_u16(const uint8_t* p)
{
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}
/**
 * @brief Read 32-bit signed little-endian value.
 * @param p Source buffer (at least 4 bytes).
 * @return Decoded value.
 */
static inline int32_t le_i32(const uint8_t* p)
{
  uint32_t v = (uint32_t)p[0]
            | ((uint32_t)p[1] << 8)
            | ((uint32_t)p[2] << 16)
            | ((uint32_t)p[3] << 24);
  return (int32_t)v;
}
/* ========================= MASTER ========================= */
/**
 * @brief Send an NMT command (COB-ID 0x000).
 *
 * Builds a 2-byte payload: [cs, node_id] and sends it via canopen_space_send_raw().
 *
 * @param cs      NMT command specifier (e.g. Start/Stop/Reset node).
 * @param node_id Destination node (0 = broadcast).
 * @param bus_id  Bus identifier used by canopen_space.
 * @return 0 on success, <0 on error.
 */
int cia402_send_nmt(uint8_t cs, uint8_t node_id, uint8_t bus_id)
{
  uint8_t d[2] = { cs, node_id };
  return canopen_space_send_raw(bus_id, 0x000u, false, false, d, 2);
}

/**
 * @brief Configure master target node_id and bus_id used for RPDO/TPDO handling.
 *
 * @param node_id Target node ID.
 * @param bus_id  Bus identifier used by canopen_space.
 */
void cia402_master_set_node_bus(uint8_t node_id, uint8_t bus_id)
{
  g_m.node_id = node_id;
  g_m.bus_id  = bus_id;
}
/**
 * @brief Set master-side Controlword (0x6040:00).
 * @param cw Controlword value.
 */
void cia402_master_set_controlword(uint16_t cw)
{
	g_m.controlword = cw;
}
/**
 * @brief Set master-side Target position (0x607A:00).
 * @param pos Target position value.
 */
void cia402_master_set_target_pos(int32_t pos)
{
	g_m.target_pos  = pos;
}
/**
 * @brief Get master-side cached Controlword (0x6040:00).
 * @return Controlword value.
 */
uint16_t cia402_master_get_controlword(void)
{
	return g_m.controlword;
}
/**
 * @brief Get master-side cached Target position (0x607A:00).
 * @return Target position value.
 */
int32_t  cia402_master_get_target_pos(void)
{
	return g_m.target_pos;
}
/**
 * @brief Get last parsed Statusword (0x6041:00) on master side.
 * @return Statusword.
 */
uint16_t cia402_master_get_statusword(void)
{
	return g_m.statusword;
}
/**
 * @brief Get last parsed Position actual value (0x6064:00) on master side.
 * @return Actual position.
 */
int32_t  cia402_master_get_pos_actual(void)
{
	return g_m.pos_actual;
}
/**
 * @brief Transmit RPDO1 (0x200 + node): 0x6040:16 + 0x607A:32.
 *
 * Payload format (6 bytes):
 * - bytes 0..1: Controlword (little-endian)
 * - bytes 2..5: Target position (little-endian)
 *
 * @return 0 on success, <0 on error.
 */
int cia402_master_pdo_flush_rpdo1(void)
{
  uint32_t cobid = 0x200u + (uint32_t)g_m.node_id;

  uint8_t d[8] = {0};
  u16_le(&d[0], g_m.controlword);
  i32_le(&d[2], g_m.target_pos);

  return canopen_space_send_raw(g_m.bus_id, cobid, false, false, d, 6);
}
/**
 * @brief Parse TPDO1 (0x180 + node) into master registers.
 *
 * Expects at least 6 bytes payload:
 * - bytes 0..1: Statusword (little-endian)
 * - bytes 2..5: Position actual (little-endian)
 *
 * @param cob_id CAN identifier (COB-ID).
 * @param data   Payload pointer.
 * @param len    Payload length.
 */
void cia402_master_pdo_parse_tpdo1(uint32_t cob_id,const uint8_t* data,uint8_t len)
{
  if (!data) return;

  uint32_t expect = 0x180u + (uint32_t)g_m.node_id;
  if (cob_id != expect) return;
  if (len < 6) return;

  g_m.statusword = le_u16(&data[0]);
  g_m.pos_actual = le_i32(&data[2]);
}
/* ========================= ACTUATOR API ========================= */

/**
 * @brief Configure actuator node_id and bus_id (stored for completeness).
 *
 * @param node_id Local node ID.
 * @param bus_id  Bus identifier used by canopen_space.
 */
void cia402_actuator_set_node_bus(uint8_t node_id, uint8_t bus_id)
{
  g_a.node_id = node_id;
  g_a.bus_id  = bus_id;
}
/**
 * @brief (Optional) Get actuator-side cached Controlword (0x6040:00).
 * @return Controlword value.
 */
uint16_t cia402_actuator_get_controlword(void)
{
	return g_a.controlword;
}
/**
 * @brief (Optional) Get actuator-side cached Target position (0x607A:00).
 * @return Target position value.
 */
int32_t  cia402_actuator_get_target_pos(void)
{
	return g_a.target_pos;
}
/**
 * @brief Set actuator-side Statusword (0x6041:00) to be published via TPDO1.
 * @param sw Statusword value.
 */
void cia402_actuator_set_statusword(uint16_t sw)
{
	g_a.statusword = sw;
}

/**
 * @brief Set actuator-side Position actual value (0x6064:00) to be published via TPDO1.
 * @param pos Actual position value.
 */
void cia402_actuator_set_pos_actual(int32_t pos)
{
	g_a.pos_actual = pos;
}

/**
 * @brief Pull RPDO1-mapped objects from Object Dictionary into actuator registers.
 *
 * Reads:
 * - 0x6040:00 -> controlword
 * - 0x607A:00 -> target_pos
 *
 * @param ms Current time in milliseconds (stored in last_rx_ms).
 */
void cia402_actuator_pdo_pull_rpdo1_from_od(uint32_t ms)
{
  co_dev_t* dev = canopen_space_dev();
  if (!dev) return;

  const void* p_cw = co_dev_get_val(dev, 0x6040, 0x00);
  const void* p_tp = co_dev_get_val(dev, 0x607A, 0x00);

  if (p_cw) g_a.controlword = *(const uint16_t*)p_cw;
  if (p_tp) g_a.target_pos  = *(const int32_t*)p_tp;

  g_a.last_rx_ms = ms;
}
/**
 * @brief Push TPDO1-mapped objects into OD and trigger TPDO1 event.
 *
 * Writes:
 * - 0x6064:00 <- pos_actual
 * - 0x6041:00 <- statusword
 * Then triggers TPDO1 event via co_tpdo_event().
 */
void cia402_actuator_pdo_push_tpdo1_to_od_and_send(void)
{
  co_dev_t* dev   = canopen_space_dev();
  co_tpdo_t* tpdo = canopen_space_tpdo1();
  if (!dev) return;

  (void)co_dev_set_val(dev, 0x6064, 0x00, &g_a.pos_actual, sizeof(g_a.pos_actual));
  (void)co_dev_set_val(dev, 0x6041, 0x00, &g_a.statusword, sizeof(g_a.statusword));

  if (tpdo) (void)co_tpdo_event(tpdo);
}



