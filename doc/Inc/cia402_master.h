/**
 * @file cia402_master.h
 * @brief CiA 402 master layer – drive state machine (CSP mode).
 *
 * Implements the CiA 402 state machine for Cyclic Synchronous Position (CSP)
 * mode. Communication uses synchronous PDOs for cyclic data and SDO for
 * one-time drive configuration.
 *
 * State sequence:
 * @verbatim
 * IDLE → SDO_INIT → SHUTDOWN → SWITCH_ON → ENABLE_OP → RUNNING
 *                                                           ↓
 *                                                         FAULT → SHUTDOWN
 * @endverbatim
 *
 * @author
 * @date
 */
#ifndef INC_CIA402_MASTER_H_
#define INC_CIA402_MASTER_H_

#include <stdint.h>
#include <stdbool.h>
#include <lely/co/dev.h>
#include <lely/co/csdo.h>

/** @defgroup cia402_od Object Dictionary Indices
 *  @brief OD indices used by the CiA 402 master.
 *  @{
 */
#define CIA402_M_OD_CONTROLWORD  0x6040u  /**< Controlword (master output → drive). */
#define CIA402_M_OD_STATUSWORD   0x6041u  /**< Statusword  (master input  ← drive). */
#define CIA402_M_OD_MODE_OP      0x6060u  /**< Modes of Operation – write CSP (0x08) via SDO. */
#define CIA402_M_OD_ACTUAL_POS   0x6064u  /**< Position actual value [pulses]. */
#define CIA402_M_OD_TARGET_POS   0x607Au  /**< Target position [pulses]. */
#define CIA402_M_OD_SUBIDX       0x00u    /**< Default sub-index (sub 0). */
/** @} */

/** @defgroup cia402_cw Controlword Values
 *  @brief Controlword constants for CiA 402 state transitions.
 *  @{
 */
#define CIA402_M_CW_SHUTDOWN     0x0006u  /**< Transition to Ready-to-Switch-On. */
#define CIA402_M_CW_SWITCH_ON    0x0007u  /**< Transition to Switched-On. */
#define CIA402_M_CW_ENABLE_OP    0x000Fu  /**< Transition to Operation-Enabled. */
#define CIA402_M_CW_FAULT_RESET  0x0080u  /**< Fault Reset – clears the Fault state. */
#define CIA402_M_CW_QUICK_STOP   0x0002u  /**< Quick Stop – immediate deceleration. */
/** @} */

/** @defgroup cia402_sw Statusword Masks and Values
 *  @brief Bitmask and decoded state values of the drive Statusword.
 *  @{
 */
#define CIA402_M_SW_MASK         0x006Fu  /**< Relevant status bits mask (bits 0-3, 5-6). */
#define CIA402_M_SW_ON_DISABLED  0x0040u  /**< Drive state: Switch-On Disabled. */
#define CIA402_M_SW_READY        0x0021u  /**< Drive state: Ready-to-Switch-On. */
#define CIA402_M_SW_SWITCHED_ON  0x0023u  /**< Drive state: Switched-On. */
#define CIA402_M_SW_OP_ENABLED   0x0027u  /**< Drive state: Operation-Enabled (drive active). */
#define CIA402_M_SW_FAULT        0x000Fu  /**< Drive state: Fault. */
/** @} */


/** @defgroup cia402_timing Timing Constants
 *  @brief Timeout and watchdog values used by the master FSM.
 *  @{
 */
#define CIA402_M_FAULT_RESET_WAIT_S  1  /**< Delay after issuing Fault Reset [s]. */
#define CIA402_M_RPDO_WATCHDOG_S     5  /**< RPDO absence watchdog threshold [s]. */
/** @} */

/**
 * @brief Internal FSM states of the CiA 402 master.
 *
 * Transitions are driven by the drive Statusword and by system events
 * (@ref cia402_master_on_running, @ref cia402_master_on_stopped).
 */
typedef enum {
	CIA402_M_IDLE,       /**< No active slave; FSM is dormant. */
	CIA402_M_SDO_INIT,   /**< Sending SDO to set Modes of Operation to CSP (0x08). */
	CIA402_M_SHUTDOWN,   /**< Waiting for SW = READY after CW = 0x0006. */
	CIA402_M_SWITCH_ON,  /**< Waiting for SW = SWITCHED_ON after CW = 0x0007. */
	CIA402_M_ENABLE_OP,  /**< Waiting for SW = OP_ENABLED after CW = 0x000F. */
	CIA402_M_RUNNING,    /**< Drive active – cyclic target position sent every SYNC. */
    CIA402_M_FAULT,      /**< Drive fault detected – executing Fault Reset sequence. */

} cia402_master_state_t;

/**
 * @brief Runtime registers of the CiA 402 master – visible in the debugger.
 *
 * The global instance @ref cia402_master is updated on every SYNC cycle
 * and on every received RPDO.
 */
typedef struct {
    uint16_t statusword;            /**< Last received drive Statusword. */
    int32_t  actual_pos;            /**< Last received actual position [pulses]. */
    int32_t  target_pos;            /**< Currently commanded target position [pulses]. */
    uint32_t rpdo_count;            /**< Total number of RPDOs received since start. */
    uint32_t sync_count;            /**< Total number of SYNC frames processed since start. */
    bool     rpdo_ever_received;    /**< @c true once at least one RPDO has been received. */
    struct timespec last_rpdo_time; /**< Monotonic timestamp of the last received RPDO. */
    struct timespec fault_time;     /**< Monotonic timestamp of the last FAULT entry. */
    cia402_master_state_t state;    /**< Current FSM state. */
    bool     sdo_done;              /**< Flag: SDO download acknowledged by the drive. */
    bool     sdo_sent;              /**< Flag: SDO download request sent, awaiting ACK. */
    bool     fault_reset_sent;      /**< Flag: Fault Reset Controlword already sent. */
} cia402_master_regs_t;

/**
 * @brief Global CiA 402 master register instance.
 * @note  Treat as read-only from outside this module; use the API to write.
 */
extern cia402_master_regs_t cia402_master;
/** @defgroup cia402_api Public API
 *  @brief Public functions of the CiA 402 master module.
 *  @{
 */

/**
 * @brief Initialises the CiA 402 master module.
 *
 * Must be called once after @c canopen_init().
 * Clears all registers and sets the FSM state to @ref CIA402_M_IDLE.
 *
 * @param[in] dev   Pointer to the CANopen device object (master OD).
 * @param[in] csdo  Pointer to the client SDO used to write Modes of Operation.
 */
void cia402_master_init    (co_dev_t *dev, co_csdo_t *csdo);
/**
 * @brief Notifies the FSM that the slave has entered the OPERATIONAL state.
 *
 * Called from @c canopen.cfg.on_running when @c st_ind() reports
 * @c CO_NMT_ST_START for the monitored node.
 * Starts the drive enable sequence:
 * SDO_INIT → SHUTDOWN → SWITCH_ON → ENABLE_OP → RUNNING.
 */
void cia402_master_on_running(void);
/**
 * @brief Notifies the FSM that the slave has left the OPERATIONAL state.
 *
 * Called from @c canopen.cfg.on_stopped on HB timeout, BOOTUP, or
 * NMT STOPPED. Issues a Quick Stop Controlword and returns to
 * @ref CIA402_M_IDLE so the sequence restarts when the slave recovers.
 */
void cia402_master_on_stopped(void);
/**
 * @brief Main FSM tick – called on every SYNC frame.
 *
 * Advances the FSM based on the current drive Statusword.
 * In @ref CIA402_M_RUNNING state, this is the application point where
 * the target position is written to the OD and sent via TPDO.
 *
 * @param[in] cnt  SYNC counter (0–240) forwarded by the CANopen stack.
 */
void cia402_master_on_sync (uint8_t cnt);

/**
 * @brief Updates Statusword and actual position from a received RPDO.
 *
 * Called from @c canopen.cfg.on_rpdo after every RPDO received from
 * the monitored slave.
 *
 * @param[in] statusword  Drive Statusword from RPDO (OD 0x6041).
 * @param[in] actual_pos  Actual position from RPDO (OD 0x6064) [pulses].
 */
void cia402_master_on_rpdo (uint16_t statusword, int32_t actual_pos);
/**
 * @brief Writes a Controlword value directly to the master OD.
 *
 * Bypasses FSM logic – use with care (e.g. from an external debug tool).
 *
 * @param[in] cw  Controlword value to write (OD 0x6040 sub 0).
 */
void cia402_master_set_controlword(uint16_t cw);
/**
 * @brief Sets the desired target position from outside the module.
 *
 * Safe to call from an interrupt or another module.
 * The value is forwarded to the drive immediately only when the FSM is in
 * @ref CIA402_M_RUNNING; otherwise it is stored and applied on the next
 * RUNNING entry.
 *
 * @param[in] pos  Desired target position [pulses].
 */
void cia402_master_set_target(int32_t pos);
/** @} */ /* cia402_api */
#endif /* INC_CIA402_MASTER_H_ */
