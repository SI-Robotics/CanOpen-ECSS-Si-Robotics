/**
 * @file cia402.c
 * @brief CiA 402 slave layer – implementation.
 *
 * Contains the drive FSA (Finite State Automaton) implementation, motion
 * simulation, and all public API functions declared in cia402.h.
 *
 * @author
 * @date
 */
#include "cia402.h"
#include "usart.h"
#include <stdio.h>
/* ── module-level storage ────────────────────────────────────────────────── */

/** @brief Global CiA 402 slave register instance (declared in cia402.h). */
cia402_regs_t cia402;
/** @brief Pointer to the slave Object Dictionary; set by @ref cia402_init(). */
static co_dev_t *g_dev = NULL;

/* ── public API ──────────────────────────────────────────────────────────── */

/**
 * @brief Initialises the CiA 402 slave module.
 *
 * Must be called once after @c canopen_init().
 * Clears all registers, sets the initial Statusword to
 * @ref CIA402_SW_ON_DISABLED, and writes it to the OD.
 *
 * @param[in] dev  Pointer to the CANopen device object (slave OD).
 */
void cia402_init(co_dev_t *dev)
{
    g_dev = dev;

    cia402.controlword      = 0x0000u;
    cia402.prev_controlword = 0x0000u;
    cia402.statusword       = CIA402_SW_ON_DISABLED;
    cia402.actual_pos       = 0;
    cia402.target_pos       = 0;
    cia402.sim_fault        = false;
    cia402.sync_count       = 0u;
    cia402.tpdo_count       = 0u;

    co_dev_set_val_u16(g_dev, CIA402_OD_STATUSWORD, CIA402_OD_SUBIDX, cia402.statusword);
    co_dev_set_val_i32(g_dev, CIA402_OD_ACTUAL_POS, CIA402_OD_SUBIDX, cia402.actual_pos);
    co_dev_set_val_i32(g_dev, CIA402_OD_TARGET_POS, CIA402_OD_SUBIDX, cia402.target_pos);
}
/**
 * @brief Notifies the CiA 402 layer that the slave has entered OPERATIONAL.
 *
 * Called from @c canopen.cfg.on_running. Resets the FSM to the initial
 * Switch-On Disabled state so the master can start a fresh enable sequence.
 */
void cia402_on_running(void)
{
	  if (cia402.statusword == CIA402_SW_ON_DISABLED && cia402.controlword == 0u)
	  {
		  return;
	  }

    cia402.controlword      = 0x0000u;
    cia402.prev_controlword = 0x0000u;
    cia402.statusword       = CIA402_SW_ON_DISABLED;

    co_dev_set_val_u16(g_dev, CIA402_OD_STATUSWORD,  CIA402_OD_SUBIDX, cia402.statusword);
    co_dev_set_val_u16(g_dev, CIA402_OD_CONTROLWORD, CIA402_OD_SUBIDX, 0x0000u);

    HAL_UART_Transmit(&huart2, (uint8_t *)"[CIA402] on_running\r\n", 21, 100);
}
/**
 * @brief Notifies the CiA 402 layer that the slave has left OPERATIONAL.
 *
 * Called from @c canopen.cfg.on_stopped (NMT STOP, HB timeout, bus error).
 * Issues a Quick Stop and resets the Statusword to Switch-On Disabled.
 */
void cia402_on_stopped(void)
{
    cia402.target_pos = cia402.actual_pos;
    co_dev_set_val_i32(g_dev, CIA402_OD_TARGET_POS, CIA402_OD_SUBIDX, cia402.target_pos);
    co_dev_set_val_u16(g_dev, CIA402_OD_CONTROLWORD, CIA402_OD_SUBIDX, 0x0000u);
    cia402.controlword      = 0x0000u;
    cia402.prev_controlword = 0x0000u;
    HAL_UART_Transmit(&huart2, (uint8_t *)"[CIA402] on_stopped\r\n", 21, 100);
}
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
void cia402_on_sync(uint8_t cnt)
{
    (void)cnt;
    ++cia402.sync_count;

    cia402.controlword = co_dev_get_val_u16(g_dev, CIA402_OD_CONTROLWORD, CIA402_OD_SUBIDX);
    cia402.target_pos  = co_dev_get_val_i32(g_dev, CIA402_OD_TARGET_POS,  CIA402_OD_SUBIDX);

    uint16_t prev_sw        = cia402.statusword;
    uint16_t cw_cmd         = cia402.controlword & CIA402_CW_MASK;
    bool     fault_rst_edge = (cia402.controlword      & CIA402_CW_FAULT_RESET) &&
                             !(cia402.prev_controlword  & CIA402_CW_FAULT_RESET);

    if (fault_rst_edge && cia402.statusword == CIA402_SW_FAULT)
    {
        cia402.statusword = CIA402_SW_ON_DISABLED;
    }
    else if (cia402.statusword == CIA402_SW_ON_DISABLED && cw_cmd == CIA402_CW_SHUTDOWN)
    {
    	cia402.statusword = CIA402_SW_READY;
    }
    else if (cia402.statusword == CIA402_SW_READY       && cw_cmd == CIA402_CW_SWITCH_ON)
    {
    	cia402.statusword = CIA402_SW_SWITCHED_ON;
    }
    else if (cia402.statusword == CIA402_SW_SWITCHED_ON && cw_cmd == CIA402_CW_ENABLE_OP)
    {
        cia402.statusword = CIA402_SW_OP_ENABLED;
        cia402.actual_pos = cia402.target_pos;
    }
    else if (cia402.statusword == CIA402_SW_OP_ENABLED  && cw_cmd == CIA402_CW_SHUTDOWN)
    {
    	cia402.statusword = CIA402_SW_READY;
    }
    else if (cia402.statusword == CIA402_SW_OP_ENABLED  && cw_cmd == CIA402_CW_QUICK_STOP)
    {
    	cia402.statusword = CIA402_SW_ON_DISABLED;
    }
    else if (cia402.statusword == CIA402_SW_OP_ENABLED &&(cia402.controlword & 0x0002u) == 0u)
    {
        cia402.statusword = CIA402_SW_ON_DISABLED;
    }
    if (cia402.sim_fault && cia402.statusword == CIA402_SW_OP_ENABLED)
    {
        cia402.statusword = CIA402_SW_FAULT;
        cia402.sim_fault  = false;
    }


    if (cia402.statusword != prev_sw) {
        char buf[64];
        int len = snprintf(buf, sizeof(buf),
            "[CIA402] SW: 0x%04X->0x%04X CW:0x%04X\r\n",
            (unsigned)prev_sw, (unsigned)cia402.statusword,
            (unsigned)cia402.controlword);
        HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
    }

    if (cia402.statusword == CIA402_SW_OP_ENABLED)
    {
        int32_t diff = cia402.target_pos - cia402.actual_pos;
        if      (diff >  10) cia402.actual_pos += 10;
        else if (diff < -10) cia402.actual_pos -= 10;
        else                 cia402.actual_pos  = cia402.target_pos;
    }

    cia402.prev_controlword = cia402.controlword;

    co_dev_set_val_u16(g_dev, CIA402_OD_STATUSWORD, CIA402_OD_SUBIDX, cia402.statusword);
    co_dev_set_val_i32(g_dev, CIA402_OD_ACTUAL_POS, CIA402_OD_SUBIDX, cia402.actual_pos);

}
/**
 * @brief Called after each TPDO transmission.
 *
 * Called from @c canopen.cfg.on_tpdo. Increments the TPDO counter and can
 * be used to prepare the next TPDO payload in the OD before the following
 * SYNC cycle.
 */
void cia402_on_tpdo(void)
{
    ++cia402.tpdo_count;
    cia402.statusword = co_dev_get_val_u16(g_dev, CIA402_OD_STATUSWORD, CIA402_OD_SUBIDX);
    cia402.actual_pos = co_dev_get_val_i32(g_dev, CIA402_OD_ACTUAL_POS, CIA402_OD_SUBIDX);
}
