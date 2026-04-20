#include "cia402.h"
#include "usart.h"
#include <stdio.h>
cia402_regs_t cia402;
static co_dev_t *g_dev = NULL;
/*
 * cia402_init() — inicjalizacja warstwy CiA 402.
 * Wywoływana raz po canopen_init() gdy mamy dostęp do OD (canopen.dev).
 * Zeruje wszystkie rejestry i wpisuje wartości startowe do OD slave'a.
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
/*
 * cia402_on_running() — master wykryty, slave wszedł w OPERATIONAL.
 * Wywoływana z canopen.cfg.on_running gdy st_ind(MASTER, START).
 * Resetuje FSA do stanu początkowego żeby master mógł zacząć
 * sekwencję Shutdown → Switch On → Enable Op od zera.
 * BEZ tego slave miałby stary statusword z poprzedniej sesji
 * i master nie mógłby poprawnie przeprowadzić sekwencji.
 */
void cia402_on_running(void)
{
    /* slave wszedł w OP — reset sekwencji FSA */
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
/*
 * cia402_on_stopped() — master zniknął (HB timeout, NO_MASTER, BOOTUP).
 * Wywoływana z canopen.cfg.on_stopped.
 * Wykonuje Quick Stop: trzyma aktualną pozycję (target = actual),
 * zeruje controlword w OD żeby FSA nie przetwarzał starego CW
 * gdy master wróci na innym busie.
 * APPLICATION POINT: tutaj wstaw motor_hold_position().
 */
void cia402_on_stopped(void)
{
    /* master zniknął — quick stop */
    cia402.target_pos = cia402.actual_pos;
    co_dev_set_val_i32(g_dev, CIA402_OD_TARGET_POS, CIA402_OD_SUBIDX, cia402.target_pos);
    co_dev_set_val_u16(g_dev, CIA402_OD_CONTROLWORD, CIA402_OD_SUBIDX, 0x0000u);
    cia402.controlword      = 0x0000u;
    cia402.prev_controlword = 0x0000u;
    HAL_UART_Transmit(&huart2, (uint8_t *)"[CIA402] on_stopped\r\n", 21, 100);
}
/*
 * cia402_on_sync() — główna pętla CiA 402 FSA.
 * Wywoływana z canopen.cfg.on_sync przy każdej ramce SYNC (0x80) od mastera.
 * Kolejność operacji:
 *   1. Odczyt Controlword (0x6040) z OD — master wpisał go przez TPDO/SDO
 *   2. Odczyt Target Position (0x607A) z OD
 *   3. Automat stanów FSA — zmiana Statusword na podstawie Controlword
 *   4. Symulacja ruchu — actual_pos dąży do target_pos (10 pulse/SYNC)
 *   5. Zapis Statusword (0x6041) i Actual Position (0x6064) do OD
 *      → TPDO wyśle je automatycznie przy następnym SYNC
 *
 * APPLICATION POINT: zastąp symulację ruchu odczytem enkodera
 * i wysłaniem komendy do napędu.
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

    /* ── FSA ─────────────────────────────────────────────────────────────── */
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

    /* APPLICATION POINT:
     * if (cia402.statusword == CIA402_SW_OP_ENABLED) {
     *     motor_set_target(cia402.target_pos);
     *     cia402.actual_pos = encoder_read_position();
     * }
     */
    if (cia402.statusword != prev_sw) {
        char buf[64];
        int len = snprintf(buf, sizeof(buf),
            "[CIA402] SW: 0x%04X->0x%04X CW:0x%04X\r\n",
            (unsigned)prev_sw, (unsigned)cia402.statusword,
            (unsigned)cia402.controlword);
        HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
    }

    /* symulacja ruchu */
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
/*
 * cia402_on_tpdo() — potwierdzenie wysłania TPDO.
 * Wywoływana z canopen.cfg.on_tpdo po każdym wysłaniu TPDO przez lely.
 * Synchronizuje lokalne rejestry z tym co faktycznie poszło na magistralę.
 * Przydatne do diagnostyki — można tu sprawdzić czy TPDO wysyła
 * poprawne wartości.
 */
void cia402_on_tpdo(void)
{
    ++cia402.tpdo_count;
    cia402.statusword = co_dev_get_val_u16(g_dev, CIA402_OD_STATUSWORD, CIA402_OD_SUBIDX);
    cia402.actual_pos = co_dev_get_val_i32(g_dev, CIA402_OD_ACTUAL_POS, CIA402_OD_SUBIDX);
}
