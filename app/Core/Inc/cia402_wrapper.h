#ifndef INC_CIA402_WRAPPER_H_
#define INC_CIA402_WRAPPER_H_
#include "canopen_space.h"

/**
 * @file cia402_wrapper.h
 * @brief Minimal CiA-402 helper wrapper for RPDO1/TPDO1 (master + actuator roles).
 *
 * This wrapper maintains two register sets:
 * - MASTER side registers (commands + feedback parsed from TPDO1),
 * - ACTUATOR side registers (commands pulled from OD + feedback pushed to OD/TPDO1).
 *
 * Intended mapping:
 * - RPDO1 (Master -> Actuator): 0x6040:16 (Controlword) + 0x607A:32 (Target position)
 * - TPDO1 (Actuator -> Master): 0x6041:16 (Statusword)  + 0x6064:32 (Position actual)
 *
 * The transport uses canopen_space helpers:
 * - Raw CAN send for master RPDO/NMT,
 * - Lely OD access + TPDO event trigger for actuator.
 */

/**
 * @brief Simple register bank for CiA-402 RPDO1/TPDO1 exchange.
 */
typedef struct
{
  uint8_t  node_id; /**< CANopen node ID. */
  uint8_t  bus_id;	/**< Logical bus ID used by canopen_space. */

  /* RPDO1 (master -> actuator) */
  uint16_t controlword;   /**< 0x6040:00 Controlword */
  int32_t  target_pos;    /**< 0x607A:00 Target position */

  /* TPDO1 (actuator -> master) */
  uint16_t statusword;    /**< 0x6041:00 Statusword */
  int32_t  pos_actual;    /**< 0x6064:00 Position actual value */

  uint32_t last_rx_ms; /**< Timestamp of last command pull (ms), actuator side. */
} cia402_regs_t;


/* ========================= MASTER API ========================= */
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
int cia402_send_nmt(uint8_t cs, uint8_t node_id, uint8_t bus_id);
/**
 * @brief Configure master target node_id and bus_id used for RPDO/TPDO handling.
 *
 * @param node_id Target node ID.
 * @param bus_id  Bus identifier used by canopen_space.
 */
void     cia402_master_set_node_bus(uint8_t node_id, uint8_t bus_id);
/**
 * @brief Set master-side Controlword (0x6040:00).
 * @param cw Controlword value.
 */
void     cia402_master_set_controlword(uint16_t cw);

/**
 * @brief Set master-side Target position (0x607A:00).
 * @param pos Target position value.
 */
void     cia402_master_set_target_pos(int32_t pos);
/**
 * @brief Get master-side cached Controlword (0x6040:00).
 * @return Controlword value.
 */
uint16_t cia402_master_get_controlword(void);
/**
 * @brief Get master-side cached Target position (0x607A:00).
 * @return Target position value.
 */
int32_t  cia402_master_get_target_pos(void);
/**
 * @brief Transmit RPDO1 (0x200 + node): 0x6040:16 + 0x607A:32.
 *
 * Payload format (6 bytes):
 * - bytes 0..1: Controlword (little-endian)
 * - bytes 2..5: Target position (little-endian)
 *
 * @return 0 on success, <0 on error.
 */
int cia402_master_pdo_flush_rpdo1(void);

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
void cia402_master_pdo_parse_tpdo1(uint32_t cob_id,const uint8_t* data,uint8_t len);

/**
 * @brief Get last parsed Statusword (0x6041:00) on master side.
 * @return Statusword.
 */
uint16_t cia402_master_get_statusword(void);
/**
 * @brief Get last parsed Position actual value (0x6064:00) on master side.
 * @return Actual position.
 */
int32_t  cia402_master_get_pos_actual(void);

/* ========================= ACTUATOR API ========================= */

/**
 * @brief Configure actuator node_id and bus_id (stored for completeness).
 *
 * @param node_id Local node ID.
 * @param bus_id  Bus identifier used by canopen_space.
 */
void cia402_actuator_set_node_bus(uint8_t node_id, uint8_t bus_id);

/**
 * @brief Pull RPDO1-mapped objects from Object Dictionary into actuator registers.
 *
 * Reads:
 * - 0x6040:00 -> controlword
 * - 0x607A:00 -> target_pos
 *
 * @param ms Current time in milliseconds (stored in last_rx_ms).
 */
void cia402_actuator_pdo_pull_rpdo1_from_od(uint32_t ms);
/**
 * @brief Set actuator-side Statusword (0x6041:00) to be published via TPDO1.
 * @param sw Statusword value.
 */
void cia402_actuator_set_statusword(uint16_t sw);
/**
 * @brief Set actuator-side Position actual value (0x6064:00) to be published via TPDO1.
 * @param pos Actual position value.
 */
void cia402_actuator_set_pos_actual(int32_t pos);
/**
 * @brief Push TPDO1-mapped objects into OD and trigger TPDO1 event.
 *
 * Writes:
 * - 0x6064:00 <- pos_actual
 * - 0x6041:00 <- statusword
 * Then triggers TPDO1 event via co_tpdo_event().
 */
void cia402_actuator_pdo_push_tpdo1_to_od_and_send(void);

/**
 * @brief (Optional) Get actuator-side cached Controlword (0x6040:00).
 * @return Controlword value.
 */
uint16_t cia402_actuator_get_controlword(void);
/**
 * @brief (Optional) Get actuator-side cached Target position (0x607A:00).
 * @return Target position value.
 */
int32_t  cia402_actuator_get_target_pos(void);

#endif /* INC_CIA402_WRAPPER_H_ */
