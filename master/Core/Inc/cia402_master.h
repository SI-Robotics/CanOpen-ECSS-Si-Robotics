#ifndef INC_CIA402_MASTER_H_
#define INC_CIA402_MASTER_H_

#include <stdint.h>
#include <stdbool.h>
#include <lely/co/dev.h>
#include <lely/co/csdo.h>

/* ── OD indices ──────────────────────────────────────────────────────────── */
#define CIA402_M_OD_CONTROLWORD  0x6040u
#define CIA402_M_OD_STATUSWORD   0x6041u
#define CIA402_M_OD_MODE_OP      0x6060u
#define CIA402_M_OD_ACTUAL_POS   0x6064u
#define CIA402_M_OD_TARGET_POS   0x607Au
#define CIA402_M_OD_SUBIDX       0x00u

/* ── Controlword ─────────────────────────────────────────────────────────── */
#define CIA402_M_CW_SHUTDOWN     0x0006u
#define CIA402_M_CW_SWITCH_ON    0x0007u
#define CIA402_M_CW_ENABLE_OP    0x000Fu
#define CIA402_M_CW_FAULT_RESET  0x0080u
#define CIA402_M_CW_QUICK_STOP   0x0002u

/* ── Statusword ──────────────────────────────────────────────────────────── */
#define CIA402_M_SW_MASK         0x006Fu
#define CIA402_M_SW_ON_DISABLED  0x0040u
#define CIA402_M_SW_READY        0x0021u
#define CIA402_M_SW_SWITCHED_ON  0x0023u
#define CIA402_M_SW_OP_ENABLED   0x0027u
#define CIA402_M_SW_FAULT        0x000Fu

/* ── timingsy ────────────────────────────────────────────────────────────── */
#define CIA402_M_FAULT_RESET_WAIT_S  1
#define CIA402_M_RPDO_WATCHDOG_S     5

/* ── stany FSM ───────────────────────────────────────────────────────────── */
typedef enum {
    CIA402_M_IDLE,
    CIA402_M_SDO_INIT,
    CIA402_M_SHUTDOWN,
    CIA402_M_SWITCH_ON,
    CIA402_M_ENABLE_OP,
    CIA402_M_RUNNING,
    CIA402_M_FAULT,
} cia402_master_state_t;

/* ── rejestry widoczne w debuggerze ──────────────────────────────────────── */
typedef struct {
    uint16_t statusword;
    int32_t  actual_pos;
    int32_t  target_pos;
    uint32_t rpdo_count;
    uint32_t sync_count;
    bool     rpdo_ever_received;
    struct timespec last_rpdo_time;
    struct timespec fault_time;
    cia402_master_state_t state;
    bool     sdo_done;
    bool     sdo_sent;
    bool     fault_reset_sent;
} cia402_master_regs_t;

extern cia402_master_regs_t cia402_master;

/* ── API ─────────────────────────────────────────────────────────────────── */
void cia402_master_init    (co_dev_t *dev, co_csdo_t *csdo);
void cia402_master_on_running(void);
void cia402_master_on_stopped(void);
void cia402_master_on_sync (uint8_t cnt);
void cia402_master_on_rpdo (uint16_t statusword, int32_t actual_pos);
void cia402_master_set_controlword(uint16_t cw);
void cia402_master_set_target(int32_t pos);

#endif /* INC_CIA402_MASTER_H_ */
