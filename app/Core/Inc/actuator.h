#ifndef INC_ACTUATOR_H_
#define INC_ACTUATOR_H_
#include "cia402_wrapper.h"
/**
 * @file actuator.h
 * @brief Small driver/cache layer for CiA-402 over CANopen (RPDO1/TPDO1).
 *
 * This module keeps a local cache for both MASTER and SLAVE roles and provides
 * a unified API to:
 * - MASTER: set Controlword (0x6040) and Target position (0x607A),
 *           transmit RPDO1 and parse TPDO1 (Statusword 0x6041, Position actual 0x6064).
 * - SLAVE : receive CW/Target via OD pull (RPDO1 mapped objects),
 *           and transmit SW/ActualPos via OD push + TPDO event.
 *
 * The actual node_id/bus_id configuration and PDO mapping are handled by cia402_wrapper.
 */

/**
 * @brief Cache structure for one CiA-402 channel (master or slave).
 *
 * Fields correspond to typical CiA-402 objects:
 * - cw      : Controlword (0x6040)
 * - pos     : Target position (0x607A)
 * - sw      : Statusword (0x6041)
 * - act_pos : Position actual value (0x6064)
 */
typedef struct{
	uint16_t cw; 		/**< Controlword (0x6040) */
	int32_t pos; 		/**< Target position (0x607A) */
	uint16_t sw;		/**< Statusword (0x6041) */
	int32_t act_pos;	/**< Position actual value (0x6064) */
}Actuator_t;

/**
 * @brief Initialize actuator module (zero the caches).
 *
 * This function does NOT set node_id/bus_id. That must be done elsewhere
 * via cia402_wrapper (e.g. selecting role, setting node_id, bus_id).
 */
void actuator_init();
/* ========================= MASTER API ========================= */

/**
 * @brief Set master-side Controlword (0x6040).
 *
 * Stores the value in the master cache and forwards it to the CiA-402 wrapper.
 * The frame is not put on the bus until @ref actuator_master_tx_update() is called.
 *
 * @param cw Controlword value (e.g. Shutdown/Switch on/Enable operation).
 */
void actuator_master_set_controlword(uint16_t cw);
/**
 * @brief Set master-side Target position (0x607A).
 *
 * Stores the value in the master cache and forwards it to the CiA-402 wrapper.
 * The frame is not put on the bus until @ref actuator_master_tx_update() is called.
 *
 * @param pos Target position (units depend on your drive configuration).
 */
void actuator_master_set_target_pos(int32_t pos);
/**
 * @brief Get the latest master-side Controlword (0x6040).
 *
 * Reads from the wrapper as the source of truth and updates the cache.
 *
 * @return Controlword (0x6040).
 */
uint16_t actuator_master_get_controlword(void);
/**
 * @brief Get the latest master-side Target position (0x607A).
 *
 * Reads from the wrapper as the source of truth and updates the cache.
 *
 * @return Target position (0x607A).
 */
int32_t  actuator_master_get_target_pos(void);

/**
 * @brief Get Statusword (0x6041) received by the master.
 *
 * Statusword is updated when TPDO1 is received and @ref actuator_master_rx_update()
 * has been called.
 *
 * @return Statusword (0x6041).
 */
uint16_t actuator_master_get_statusword(void);
/**
 * @brief Get Position actual value (0x6064) received by the master.
 *
 * The value is updated when TPDO1 is received and @ref actuator_master_rx_update()
 * has been called.
 *
 * @return Position actual value (0x6064).
 */
int32_t  actuator_master_get_pos_actual(void);

/**
 * @brief Transmit master RPDO1 (Controlword + Target position).
 *
 * Performs an RPDO1 “flush” configured in the wrapper:
 * mapping: 0x6040:16 + 0x607A:32 (typically RPDO1 COB-ID = 0x200 + node).
 *
 * @retval 0  Success.
 * @retval <0 Error from wrapper/stack.
 */
int actuator_master_tx_update();
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
void actuator_master_rx_update(uint32_t cob_id,const uint8_t* data,uint8_t len);
/* ========================= SLAVE API ========================= */
/**
 * @brief Get slave-side Controlword (0x6040) received via RPDO1.
 *
 * Expects that @ref actuator_slave_rx_update() has been called earlier
 * (OD pull).
 *
 * @return Controlword (0x6040).
 */
uint16_t actuator_slave_get_controlword(void);

/**
 * @brief Get slave-side Target position (0x607A) received via RPDO1.
 *
 * Expects that @ref actuator_slave_rx_update() has been called earlier
 * (OD pull).
 *
 * @return Target position (0x607A).
 */
int32_t  actuator_slave_get_target_pos(void);

/**
 * @brief Set slave-side Statusword (0x6041) to be transmitted in TPDO1.
 *
 * Stores the value in the slave cache and forwards it to the wrapper.
 * Actual TPDO1 transmit happens when @ref actuator_slave_tx_update() is called.
 *
 * @param sw Statusword (0x6041).
 */
void actuator_slave_set_statusword(uint16_t sw);
/**
 * @brief Set slave-side Position actual value (0x6064) to be transmitted in TPDO1.
 *
 * Stores the value in the slave cache and forwards it to the wrapper.
 * Actual TPDO1 transmit happens when @ref actuator_slave_tx_update() is called.
 *
 * @param act_pos Position actual value (0x6064).
 */
void actuator_slave_set_pos_actual(int32_t act_pos);
/**
 * @brief Get slave-side Statusword (0x6041) from the local cache.
 *
 * On the slave side, Statusword is typically produced by the application
 * (state machine) and written via @ref actuator_slave_set_statusword().
 *
 * @return Cached Statusword (0x6041).
 */
uint16_t actuator_slave_get_statusword(void);
/**
 * @brief Get slave-side Position actual value (0x6064) from the local cache.
 *
 * On the slave side, actual position is typically produced by the application
 * and written via @ref actuator_slave_set_pos_actual().
 *
 * @return Cached Position actual value (0x6064).
 */
int32_t  actuator_slave_get_pos_actual(void);

/**
 * @brief Update slave receive path (RPDO1 -> OD -> wrapper -> cache).
 *
 * Performs OD pull of the RPDO1-mapped objects (Controlword + Target position)
 * and updates the slave cache accordingly. Call this periodically (e.g. every 1 ms)
 * from your main loop or timer tick.
 *
 * @param ms Current time in milliseconds (used by the underlying stack for timing).
 */
void actuator_slave_rx_update(uint32_t ms);
/**
 * @brief Update slave transmit path (cache -> wrapper -> OD -> TPDO1 event).
 *
 * Ensures the latest Statusword and Position actual value are present in the wrapper,
 * pushes them into the OD and triggers TPDO1 transmission (event-driven TPDO).
 * Call this whenever you want to publish new feedback to the master.
 */
void actuator_slave_tx_update(void);

#endif /* INC_ACTUATOR_H_ */
