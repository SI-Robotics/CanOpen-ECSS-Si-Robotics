/**
 * @file cia402.h
 * @brief CiA 402 slave layer – drive state machine simulation.
 *
 * Implements the CiA 402 drive state machine on the slave side.
 * Processes Controlword commands received via RPDO and updates the
 * Statusword and position data exposed via TPDO.
 *
 * State sequence:
 * @verbatim
 * SWITCH_ON_DISABLED → READY_TO_SWITCH_ON → SWITCHED_ON → OPERATION_ENABLED
 *                                                                  ↓
 *                                                               FAULT
 * @endverbatim
 *
 * @author
 * @date
 */

#ifndef INC_CIA402_H_
#define INC_CIA402_H_

#include <stdint.h>
#include <stdbool.h>
#include <lely/co/dev.h>

/** @defgroup cia402_od Object Dictionary Indices
 *  @brief OD indices used by the CiA 402 slave.
 *  @{
 */
#define CIA402_OD_CONTROLWORD  0x6040u  /**< Controlword (slave input  ← master). */
#define CIA402_OD_STATUSWORD   0x6041u  /**< Statusword  (slave output → master). */
#define CIA402_OD_ACTUAL_POS   0x6064u  /**< Position actual value [pulses]. */
#define CIA402_OD_TARGET_POS   0x607Au  /**< Target position [pulses]. */
#define CIA402_OD_SUBIDX       0x00u    /**< Default sub-index (sub 0). */
/** @} */

/** @defgroup cia402_cw Controlword Masks and Values
 *  @brief Bitmask and command values for the drive Controlword.
 *  @{
 */
#define CIA402_CW_MASK        0x008Fu  /**< Relevant Controlword bits mask. */
#define CIA402_CW_SHUTDOWN    0x0006u  /**< Command: transition to Ready-to-Switch-On. */
#define CIA402_CW_SWITCH_ON   0x0007u  /**< Command: transition to Switched-On. */
#define CIA402_CW_ENABLE_OP   0x000Fu  /**< Command: transition to Operation-Enabled. */
#define CIA402_CW_QUICK_STOP  0x0002u  /**< Command: Quick Stop – immediate deceleration. */
#define CIA402_CW_FAULT_RESET 0x0080u  /**< Command: Fault Reset – clears the Fault state. */
/** @} */

/** @defgroup cia402_sw Statusword Values
 *  @brief Statusword values representing each CiA 402 drive state.
 *  @{
 */
#define CIA402_SW_ON_DISABLED  0x0040u  /**< Drive state: Switch-On Disabled. */
#define CIA402_SW_READY        0x0021u  /**< Drive state: Ready-to-Switch-On. */
#define CIA402_SW_SWITCHED_ON  0x0023u  /**< Drive state: Switched-On. */
#define CIA402_SW_OP_ENABLED   0x0027u  /**< Drive state: Operation-Enabled (drive active). */
#define CIA402_SW_FAULT        0x000Fu  /**< Drive state: Fault. */
/** @} */

/**
 * @brief Runtime registers of the CiA 402 slave – visible in the debugger.
 *
 * The global instance @ref cia402 is updated on every SYNC cycle and on
 * every TPDO transmission. All fields reflect the current state of the
 * simulated drive.
 */
typedef struct {
    uint16_t controlword;       /**< Last Controlword received from the master (OD 0x6040). */
    uint16_t prev_controlword;  /**< Controlword from the previous SYNC cycle (edge detection). */
    uint16_t statusword;        /**< Current Statusword sent to the master (OD 0x6041). */
    int32_t  actual_pos;        /**< Current actual position [pulses] (OD 0x6064). */
    int32_t  target_pos;        /**< Last target position received from the master (OD 0x607A). */
    bool     sim_fault;         /**< When @c true, the FSM will enter the Fault state on the
                                     next SYNC cycle. Set externally to inject a fault. */
    uint32_t sync_count;        /**< Total number of SYNC frames processed since start. */
    uint32_t tpdo_count;        /**< Total number of TPDOs transmitted since start. */
} cia402_regs_t;
/**
 * @brief Global CiA 402 slave register instance.
 * @note  @c sim_fault may be set externally to trigger a fault injection.
 *        All other fields are managed by the CiA 402 layer.
 */
extern cia402_regs_t cia402;

/** @defgroup cia402_api Public API
 *  @brief Public functions of the CiA 402 slave module.
 *  @{
 */

/**
 * @brief Initialises the CiA 402 slave module.
 *
 * Must be called once after @c canopen_init().
 * Clears all registers, sets the initial Statusword to
 * @ref CIA402_SW_ON_DISABLED, and writes it to the OD.
 *
 * @param[in] dev  Pointer to the CANopen device object (slave OD).
 */
void cia402_init   (co_dev_t *dev);
/**
 * @brief Main FSM tick – called on every SYNC frame.
 *
 * Reads the current Controlword from the OD, advances the CiA 402 state
 * machine, updates the Statusword and actual position in the OD.
 * If @c sim_fault is set the FSM transitions to the Fault state regardless
 * of the Controlword.
 *
 * @param[in] cnt  SYNC counter (0–240) forwarded by the CANopen stack.
 */
void cia402_on_sync(uint8_t cnt);
/**
 * @brief Notifies the CiA 402 layer that the slave has entered OPERATIONAL.
 *
 * Called from @c canopen.cfg.on_running. Resets the FSM to the initial
 * Switch-On Disabled state so the master can start a fresh enable sequence.
 */
void cia402_on_running(void);
/**
 * @brief Notifies the CiA 402 layer that the slave has left OPERATIONAL.
 *
 * Called from @c canopen.cfg.on_stopped (NMT STOP, HB timeout, bus error).
 * Issues a Quick Stop and resets the Statusword to Switch-On Disabled.
 */
void cia402_on_stopped(void);
/**
 * @brief Called after each TPDO transmission.
 *
 * Called from @c canopen.cfg.on_tpdo. Increments the TPDO counter and can
 * be used to prepare the next TPDO payload in the OD before the following
 * SYNC cycle.
 */
void cia402_on_tpdo(void);

/** @} */ /* cia402_api */
#endif /* INC_CIA402_H_ */
