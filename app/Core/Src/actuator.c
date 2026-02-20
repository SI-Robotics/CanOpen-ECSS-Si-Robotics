/**
 * @file actuator.c
 * @brief Implementation of actuator cache layer for CiA-402 over CANopen (RPDO1/TPDO1).
 *
 * This module keeps a local cache for both MASTER and SLAVE roles:
 * - MASTER: set Controlword (0x6040) + Target position (0x607A), transmit RPDO1,
 *           parse TPDO1 (Statusword 0x6041 + Position actual 0x6064).
 * - SLAVE : receive CW/Target via OD pull (RPDO1 mapped objects),
 *           transmit SW/ActualPos via OD push + TPDO event.
 *
 * The actual node_id/bus_id configuration and PDO mapping are handled by cia402_wrapper.
 */
#include "actuator.h"

/** @brief Local master-side cache. */
static Actuator_t g_master = {0};
/** @brief Local slave-side cache. */
static Actuator_t g_slave  = {0};

/**
 * @brief Initialize actuator module (zero both caches).
 *
 * @note This function does NOT configure node_id/bus_id. Do that elsewhere via
 *       cia402_wrapper (e.g. set role, node_id, bus_id, PDO mapping).
 */
void actuator_init(void)
{
  memset(&g_master, 0, sizeof(g_master));
  memset(&g_slave,  0, sizeof(g_slave));
}

/* Note: node_id/bus_id are configured outside this module, e.g. in cia402_wrapper. */


/* ========================= MASTER API ========================= */

/**
* @brief Set master-side Controlword (0x6040).
*
* Stores the value in the master cache and forwards it to the CiA-402 wrapper.
* The frame is not put on the bus until @ref actuator_master_tx_update() is called.
*
* @param cw Controlword value (e.g. Shutdown/Switch on/Enable operation).
*/
void actuator_master_set_controlword(uint16_t cw)
{
  g_master.cw = cw;
  cia402_master_set_controlword(cw);
}
/**
 * @brief Set master-side Target position (0x607A).
 *
 * Stores the value in the master cache and forwards it to the CiA-402 wrapper.
 * The frame is not put on the bus until @ref actuator_master_tx_update() is called.
 *
 * @param pos Target position (units depend on your drive configuration).
 */
void actuator_master_set_target_pos(int32_t pos)
{
  g_master.pos = pos;
  cia402_master_set_target_pos(pos);
}

/**
 * @brief Get the latest master-side Controlword (0x6040).
 *
 * Reads from the wrapper as the source of truth and updates the cache.
 *
 * @return Controlword (0x6040).
 */
uint16_t actuator_master_get_controlword(void)
{
  g_master.cw = cia402_master_get_controlword();
  return g_master.cw;
}

/**
 * @brief Get the latest master-side Target position (0x607A).
 *
 * Reads from the wrapper as the source of truth and updates the cache.
 *
 * @return Target position (0x607A).
 */
int32_t actuator_master_get_target_pos(void)
{
  g_master.pos = cia402_master_get_target_pos();
  return g_master.pos;
}

/**
 * @brief Get Statusword (0x6041) received by the master.
 *
 * Statusword is updated when TPDO1 is received and @ref actuator_master_rx_update()
 * has been called.
 *
 * @return Statusword (0x6041).
 */
uint16_t actuator_master_get_statusword(void)
{
  g_master.sw = cia402_master_get_statusword();
  return g_master.sw;
}

/**
 * @brief Get Position actual value (0x6064) received by the master.
 *
 * The value is updated when TPDO1 is received and @ref actuator_master_rx_update()
 * has been called.
 *
 * @return Position actual value (0x6064).
 */
int32_t actuator_master_get_pos_actual(void)
{
  g_master.act_pos = cia402_master_get_pos_actual();
  return g_master.act_pos;
}

/**
 * @brief Transmit master RPDO1 (Controlword + Target position).
 *
 * Performs an RPDO1 “flush” configured in the wrapper:
 * mapping: 0x6040:16 + 0x607A:32 (typically RPDO1 COB-ID = 0x200 + node).
 *
 * @retval 0  Success.
 * @retval <0 Error from wrapper/stack.
 */
int actuator_master_tx_update(void)
{
  return cia402_master_pdo_flush_rpdo1();
}
/**
 * @brief Process a received CAN frame on the master side (TPDO1).
 *
 * The frame is forwarded to the wrapper parser. If it matches TPDO1
 * for the configured node (typically COB-ID = 0x180 + node,
 * mapping 0x6041:16 + 0x6064:32), the wrapper updates its values and
 * this module pulls them into the master cache.
 *
 * @param cob_id CAN identifier (COB-ID).
 * @param data   Payload pointer.
 * @param len    Payload length (expected 6 bytes for 16+32 mapping).
 */
void actuator_master_rx_update(uint32_t cob_id, const uint8_t* data, uint8_t len)
{
  cia402_master_pdo_parse_tpdo1(cob_id, data, len);
  g_master.sw      = cia402_master_get_statusword();
  g_master.act_pos = cia402_master_get_pos_actual();
}


/* ========================= SLAVE API ========================= */

/**
 * @brief Get slave-side Controlword (0x6040) received via RPDO1.
 *
 * Expects that @ref actuator_slave_rx_update() has been called earlier
 * (OD pull).
 *
 * @return Controlword (0x6040).
 */
uint16_t actuator_slave_get_controlword(void)
{
  g_slave.cw = cia402_actuator_get_controlword();
  return g_slave.cw;
}
/**
 * @brief Get slave-side Target position (0x607A) received via RPDO1.
 *
 * Expects that @ref actuator_slave_rx_update() has been called earlier
 * (OD pull).
 *
 * @return Target position (0x607A).
 */
int32_t actuator_slave_get_target_pos(void)
{
  g_slave.pos = cia402_actuator_get_target_pos();
  return g_slave.pos;
}
/**
 * @brief Set slave-side Statusword (0x6041) to be transmitted in TPDO1.
 *
 * Stores the value in the slave cache and forwards it to the wrapper.
 * Actual TPDO1 transmit happens when @ref actuator_slave_tx_update() is called.
 *
 * @param sw Statusword (0x6041).
 */
void actuator_slave_set_statusword(uint16_t sw)
{
  g_slave.sw = sw;
  cia402_actuator_set_statusword(sw);
}

/**
 * @brief Set slave-side Position actual value (0x6064) to be transmitted in TPDO1.
 *
 * Stores the value in the slave cache and forwards it to the wrapper.
 * Actual TPDO1 transmit happens when @ref actuator_slave_tx_update() is called.
 *
 * @param act_pos Position actual value (0x6064).
 */
void actuator_slave_set_pos_actual(int32_t act_pos)
{
  g_slave.act_pos = act_pos;
  cia402_actuator_set_pos_actual(act_pos);
}
/**
 * @brief Get slave-side Statusword (0x6041) from the local cache.
 *
 * On the slave side, Statusword is typically produced by the application
 * (state machine) and written via @ref actuator_slave_set_statusword().
 *
 * @return Cached Statusword (0x6041).
 */
uint16_t actuator_slave_get_statusword(void)
{
  return g_slave.sw;
}
/**
 * @brief Get slave-side Position actual value (0x6064) from the local cache.
 *
 * On the slave side, actual position is typically produced by the application
 * and written via @ref actuator_slave_set_pos_actual().
 *
 * @return Cached Position actual value (0x6064).
 */
int32_t actuator_slave_get_pos_actual(void)
{
  return g_slave.act_pos;
}
/**
 * @brief Update slave receive path (RPDO1 -> OD -> wrapper -> cache).
 *
 * Performs OD pull of the RPDO1-mapped objects (Controlword + Target position)
 * and updates the slave cache accordingly. Call this periodically (e.g. every 1 ms)
 * from your main loop or timer tick.
 *
 * @param ms Current time in milliseconds (used by the underlying stack for timing).
 */
void actuator_slave_rx_update(uint32_t ms)
{
  cia402_actuator_pdo_pull_rpdo1_from_od(ms);
  g_slave.cw  = cia402_actuator_get_controlword();
  g_slave.pos = cia402_actuator_get_target_pos();
}
/**
 * @brief Update slave transmit path (cache -> wrapper -> OD -> TPDO1 event).
 *
 * Ensures the latest Statusword and Position actual value are present in the wrapper,
 * pushes them into the OD and triggers TPDO1 transmission (event-driven TPDO).
 * Call this whenever you want to publish new feedback to the master.
 */
void actuator_slave_tx_update(void)
{
  cia402_actuator_set_statusword(g_slave.sw);
  cia402_actuator_set_pos_actual(g_slave.act_pos);
  cia402_actuator_pdo_push_tpdo1_to_od_and_send();
}
