/**
 * @file cia402_master.c
 * @brief CiA 402 master layer – implementation.
 *
 * Contains the FSM implementation, internal helpers, and all public API
 * functions declared in cia402_master.h.
 *
 * @author
 * @date
 */
#include "cia402_master.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <stdarg.h>

/* ── module-level storage ────────────────────────────────────────────────── */

/** @brief Global CiA 402 master register instance (declared in cia402_master.h). */

cia402_master_regs_t cia402_master;
/** @brief Pointer to the master Object Dictionary; set by @ref cia402_master_init(). */
static co_dev_t  *g_dev  = NULL;
/** @brief Pointer to the client SDO; set by @ref cia402_master_init(). */
static co_csdo_t *g_csdo = NULL;
/* ── internal helpers ────────────────────────────────────────────────────── */

/**
 * @brief printf-style UART logger; formats into a 128-byte stack buffer.
 * @param[in] fmt  printf format string.
 * @param[in] ...  Format arguments.
 */
static void mlog(const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
}

/**
 * @brief APPLICATION POINT – returns the target position for the current SYNC cycle.
 *
 * Called on every FSM iteration while in @ref CIA402_M_RUNNING state.
 * Modify this function to implement custom motion profiles.
 *
 * Default behaviour: toggles between 0 and 100 pulses every 5000 SYNC frames.
 *
 * @return Target position [pulses] to be sent to the drive.
 */
static int32_t cia402_master_get_target(void)
{
    static bool toggle = false;
    static uint32_t last_sync = 0;
    if (cia402_master.sync_count - last_sync >= 5000)
    {
        toggle    = !toggle;
        last_sync = cia402_master.sync_count;
        mlog("[CIA402M] target -> %ld", (long)(toggle ? 1000 : 0));
    }
    return toggle ? 100 : 0;
}
/**
 * @brief SDO download confirmation callback.
 *
 * Registered as the completion handler for @c co_csdo_dn_val_req() in the
 * @ref CIA402_M_SDO_INIT state. On success sets @c sdo_done so the FSM can
 * advance to @ref CIA402_M_SHUTDOWN. On failure logs the abort code and
 * returns the FSM to @ref CIA402_M_IDLE.
 *
 * @param[in] sdo     Client SDO object (unused).
 * @param[in] idx     OD index of the downloaded object (unused).
 * @param[in] subidx  OD sub-index of the downloaded object (unused).
 * @param[in] ac      SDO abort code; 0 on success.
 * @param[in] data    User data pointer (unused).
 */
static void sdo_dn_con(co_csdo_t *sdo, co_unsigned16_t idx,
                       co_unsigned8_t subidx, co_unsigned32_t ac, void *data)
{
    (void)sdo; (void)idx; (void)subidx; (void)data;
    if (ac != 0u)
    {
    	mlog("[CIA402M] SDO FAILED 0x%08X", (unsigned)ac);
        cia402_master.state = CIA402_M_IDLE;
        return;
    }
    cia402_master.sdo_done = true;
}

/**
 * @brief Writes a Controlword value to the master OD and logs it.
 * @param[in] cw  Controlword value to write (OD 0x6040 sub 0).
 */
static void send_controlword(uint16_t cw)
{
    co_dev_set_val_u16(g_dev, CIA402_M_OD_CONTROLWORD, CIA402_M_OD_SUBIDX, cw);
    mlog("[CIA402M] CW -> 0x%04X", (unsigned)cw);
}
/**
 * @brief Writes a target position to the master OD and updates the register mirror.
 *
 * Does not trigger an immediate TPDO transmission; the value is picked up
 * by the next SYNC-triggered TPDO send.
 *
 * @param[in] pos  Target position [pulses] to write (OD 0x607A sub 0).
 */
static void send_target_pos(int32_t pos)
{
    cia402_master.target_pos = pos;
    co_dev_set_val_i32(g_dev, CIA402_M_OD_TARGET_POS, CIA402_M_OD_SUBIDX, pos);
}
/**
 * @brief Returns the masked drive state extracted from the last Statusword.
 *
 * Applies @ref CIA402_M_SW_MASK to @c cia402_master.statusword so the
 * result can be compared directly against @ref CIA402_M_SW_READY,
 * @ref CIA402_M_SW_SWITCHED_ON, etc.
 *
 * @return Masked Statusword value representing the current drive state.
 */
static uint16_t get_state(void)
{
    return cia402_master.statusword & CIA402_M_SW_MASK;
}

/* ── public API ──────────────────────────────────────────────────────────── */

/**
 * @brief Initialises the CiA 402 master module.
 *
 * Must be called once after @c canopen_init().
 * Clears all registers and sets the FSM state to @ref CIA402_M_IDLE.
 *
 * @param[in] dev   Pointer to the CANopen device object (master OD).
 * @param[in] csdo  Pointer to the client SDO used to write Modes of Operation.
 */
void cia402_master_init(co_dev_t *dev, co_csdo_t *csdo)
{
    g_dev  = dev;
    g_csdo = csdo;

    memset(&cia402_master, 0, sizeof(cia402_master));
    cia402_master.state = CIA402_M_IDLE;
    mlog("[CIA402M] init OK");
}
/**
 * @brief Notifies the FSM that the slave has entered the OPERATIONAL state.
 *
 * Called from @c canopen.cfg.on_running when @c st_ind() reports
 * @c CO_NMT_ST_START for the monitored node.
 * Starts the drive enable sequence:
 * SDO_INIT → SHUTDOWN → SWITCH_ON → ENABLE_OP → RUNNING.
 */
void cia402_master_on_running(void)
{
    cia402_master.sdo_done         = false;
    cia402_master.sdo_sent         = false;
    cia402_master.fault_reset_sent = false;
    cia402_master.statusword       = 0u;
    cia402_master.state            = CIA402_M_SDO_INIT;
    mlog("[CIA402M] on_running -> SDO_INIT");
}
/**
 * @brief Notifies the FSM that the slave has left the OPERATIONAL state.
 *
 * Called from @c canopen.cfg.on_stopped on HB timeout, BOOTUP, or
 * NMT STOPPED. Issues a Quick Stop Controlword and returns to
 * @ref CIA402_M_IDLE so the sequence restarts when the slave recovers.
 */
void cia402_master_on_stopped(void)
{
    send_controlword(CIA402_M_CW_QUICK_STOP);
    cia402_master.state = CIA402_M_IDLE;
    mlog("[CIA402M] on_stopped -> IDLE");
}
/**
 * @brief Updates Statusword and actual position from a received RPDO.
 *
 * Called from @c canopen.cfg.on_rpdo after every RPDO received from
 * the monitored slave.
 *
 * @param[in] statusword  Drive Statusword from RPDO (OD 0x6041).
 * @param[in] actual_pos  Actual position from RPDO (OD 0x6064) [pulses].
 */
void cia402_master_on_rpdo(uint16_t statusword, int32_t actual_pos)
{
    cia402_master.statusword         = statusword;
    cia402_master.actual_pos         = actual_pos;
    cia402_master.rpdo_count++;
    cia402_master.rpdo_ever_received = true;
    /* last_rpdo_time aktualizowany z on_sync żeby nie potrzebować czasu tutaj */
}

/**
 * @brief Writes a Controlword value directly to the master OD.
 *
 * Bypasses FSM logic – use with care (e.g. from an external debug tool).
 *
 * @param[in] cw  Controlword value to write (OD 0x6040 sub 0).
 */
void cia402_master_set_controlword(uint16_t cw)
{
    co_dev_set_val_u16(g_dev, CIA402_M_OD_CONTROLWORD, 0x00u, cw);
}
/**
 * @brief Main FSM tick – called on every SYNC frame.
 *
 * Advances the FSM based on the current drive Statusword.
 * In @ref CIA402_M_RUNNING state, this is the application point where
 * the target position is written to the OD and sent via TPDO.
 *
 * @param[in] cnt  SYNC counter (0–240) forwarded by the CANopen stack.
 */
void cia402_master_on_sync(uint8_t cnt)
{
    (void)cnt;
    ++cia402_master.sync_count;

    if (g_dev == NULL || g_csdo == NULL) return;

    cia402_master_state_t prev_state = cia402_master.state;

    switch (cia402_master.state)
    {
        case CIA402_M_IDLE:
            break;

        case CIA402_M_SDO_INIT:
            if (!cia402_master.sdo_sent)
            {
                co_unsigned8_t mode = 0x08;
                if (co_csdo_dn_val_req(g_csdo, CIA402_M_OD_MODE_OP, CIA402_M_OD_SUBIDX, CO_DEFTYPE_INTEGER8, &mode, NULL, &sdo_dn_con, NULL) == 0)
                    cia402_master.sdo_sent = true;
                mlog("[CIA402M] SDO CSP sent");
            }
            else if (cia402_master.sdo_done)
            {
                cia402_master.sdo_done = false;
                cia402_master.sdo_sent = false;
                send_controlword(CIA402_M_CW_SHUTDOWN);
                cia402_master.state = CIA402_M_SHUTDOWN;
            }
            break;

        case CIA402_M_SHUTDOWN:
            if (get_state() == CIA402_M_SW_READY)
            {
                send_controlword(CIA402_M_CW_SWITCH_ON);
                cia402_master.state = CIA402_M_SWITCH_ON;
            }
            break;

        case CIA402_M_SWITCH_ON:
            if (get_state() == CIA402_M_SW_SWITCHED_ON) {

                send_target_pos(cia402_master.actual_pos);
                send_controlword(CIA402_M_CW_ENABLE_OP);
                cia402_master.state = CIA402_M_ENABLE_OP;
            }
            break;

        case CIA402_M_ENABLE_OP:
            if (get_state() == CIA402_M_SW_OP_ENABLED)
                cia402_master.state = CIA402_M_RUNNING;
            break;

        case CIA402_M_RUNNING:
            /* APPLICATION POINT */
            send_target_pos(cia402_master_get_target());

            if (get_state() == CIA402_M_SW_FAULT)
            {
                cia402_master.fault_reset_sent = false;
                cia402_master.state = CIA402_M_FAULT;
            }
            else if (get_state() == CIA402_M_SW_ON_DISABLED ||
                     get_state() == CIA402_M_SW_READY        ||
                     get_state() == CIA402_M_SW_SWITCHED_ON)
            {
                mlog("[CIA402M] unexpected SW=0x%04X in RUNNING -> IDLE",
                     (unsigned)cia402_master.statusword);
                cia402_master.state = CIA402_M_IDLE;
            }
            break;

        case CIA402_M_FAULT:
            if (!cia402_master.fault_reset_sent)
            {
                send_controlword(CIA402_M_CW_FAULT_RESET);
                cia402_master.fault_reset_sent = true;
            }
            else
            {
                send_controlword(CIA402_M_CW_SHUTDOWN);
                cia402_master.fault_reset_sent = false;
                cia402_master.state = CIA402_M_SHUTDOWN;
            }
            break;
    }
    if (cia402_master.state != prev_state) {
        const char *names[] = {
            "IDLE", "SDO_INIT", "SHUTDOWN",
            "SWITCH_ON", "ENABLE_OP", "RUNNING", "FAULT"
        };
        mlog("[CIA402M] %s -> %s SW:0x%04X",
             names[prev_state], names[cia402_master.state],
             (unsigned)cia402_master.statusword);
    }
}

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
void cia402_master_set_target(int32_t pos)
{
    cia402_master.target_pos = pos;
    if (cia402_master.state == CIA402_M_RUNNING)
        send_target_pos(pos);
}
