#include "cia402_master.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <stdarg.h>
cia402_master_regs_t cia402_master;

static co_dev_t  *g_dev  = NULL;
static co_csdo_t *g_csdo = NULL;

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

/*
 * cia402_master_get_target() — APPLICATION POINT.
 * Wywoływana w każdej iteracji stanu RUNNING z on_sync().
 * Zmień tę funkcję żeby sterować pozycją napędu.
 * Domyślnie: co 5000 SYNC toggleuje między 0 a 1000 pulse.
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

static void send_controlword(uint16_t cw)
{
    co_dev_set_val_u16(g_dev, CIA402_M_OD_CONTROLWORD, CIA402_M_OD_SUBIDX, cw);
    mlog("[CIA402M] CW -> 0x%04X", (unsigned)cw);
}

static void send_target_pos(int32_t pos)
{
    cia402_master.target_pos = pos;
    co_dev_set_val_i32(g_dev, CIA402_M_OD_TARGET_POS, CIA402_M_OD_SUBIDX, pos);
}

static uint16_t get_state(void)
{
    return cia402_master.statusword & CIA402_M_SW_MASK;
}

/* ── API ─────────────────────────────────────────────────────────────────── */
/*
 * cia402_master_init() — inicjalizacja warstwy CiA 402 mastera.
 * Wywoływana raz po canopen_init() z canopen.dev i canopen.csdo.
 * csdo potrzebne do wysłania SDO z trybem pracy (CSP = 0x08).
 */
void cia402_master_init(co_dev_t *dev, co_csdo_t *csdo)
{
    g_dev  = dev;
    g_csdo = csdo;

    memset(&cia402_master, 0, sizeof(cia402_master));
    cia402_master.state = CIA402_M_IDLE;
    mlog("[CIA402M] init OK");
}
/*
 * cia402_master_on_running() — slave wszedł w OPERATIONAL.
 * Wywoływana z canopen.cfg.on_running gdy st_ind(SLAVE, START).
 * Startuje sekwencję: SDO_INIT → SHUTDOWN → SWITCH_ON → ENABLE_OP → RUNNING.
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
/*
 * cia402_master_on_stopped() — slave zniknął (HB timeout, bus switch, BOOTUP).
 * Wywoływana z canopen.cfg.on_stopped.
 * Wysyła Quick Stop i wraca do IDLE — sekwencja zacznie się od nowa
 * gdy slave wróci i wywoła on_running().
 */
void cia402_master_on_stopped(void)
{
    send_controlword(CIA402_M_CW_QUICK_STOP);
    cia402_master.state = CIA402_M_IDLE;
    mlog("[CIA402M] on_stopped -> IDLE");
}

void cia402_master_on_rpdo(uint16_t statusword, int32_t actual_pos)
{
    cia402_master.statusword         = statusword;
    cia402_master.actual_pos         = actual_pos;
    cia402_master.rpdo_count++;
    cia402_master.rpdo_ever_received = true;
    /* last_rpdo_time aktualizowany z on_sync żeby nie potrzebować czasu tutaj */
}

/*
 * cia402_master_on_sync() — główna pętla FSM mastera CiA 402.
 * Wywoływana z canopen.cfg.on_sync przy każdej ramce SYNC.
 * Master jest producentem SYNC — wysyła go do slave'a, a slave
 * odpowiada przez TPDO (które master odbiera jako RPDO → on_rpdo).
 *
 * Sekwencja stanów:
 *   IDLE       — brak slave'a, czekamy
 *   SDO_INIT   — wysyła SDO: tryb pracy = 0x08 (CSP), czeka na ACK
 *   SHUTDOWN   — CW=0x0006, czeka aż SW=READY (0x0021)
 *   SWITCH_ON  — CW=0x0007, czeka aż SW=SWITCHED_ON (0x0023)
 *   ENABLE_OP  — CW=0x000F, czeka aż SW=OP_ENABLED (0x0027)
 *   RUNNING    — napęd aktywny, wysyła target position
 *   FAULT      — błąd napędu, wysyła Fault Reset i wraca do SHUTDOWN
 */
void cia402_master_set_controlword(uint16_t cw)
{
    co_dev_set_val_u16(g_dev,                    /* ← zamiast cia402_master_dev */
                       CIA402_M_OD_CONTROLWORD, 0x00u, cw);
}
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
                co_unsigned8_t mode = 0x08; /* CSP */
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
                /* anti-jump — ustaw target = actual przed enable */
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
                /* slave opuścił OP_ENABLED — zatrzymaj */
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
    /* log przy zmianie stanu FSM */
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

/*
 * cia402_master_set_target() — ustawia żądaną pozycję z zewnątrz.
 * Wywoływana z aplikacji (np. z przerwania, z innego modułu).
 * Zmiana pozycji jest bezpieczna tylko gdy state == CIA402_M_RUNNING.
 */
void cia402_master_set_target(int32_t pos)
{
    cia402_master.target_pos = pos;
    if (cia402_master.state == CIA402_M_RUNNING)
        send_target_pos(pos);
}
