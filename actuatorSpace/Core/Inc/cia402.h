#ifndef INC_CIA402_H_
#define INC_CIA402_H_

#include <stdint.h>
#include <stdbool.h>
#include <lely/co/dev.h>

/* ── OD indices ──────────────────────────────────────────────────────────── */
#define CIA402_OD_CONTROLWORD   0x6040u
#define CIA402_OD_STATUSWORD    0x6041u
#define CIA402_OD_ACTUAL_POS    0x6064u
#define CIA402_OD_TARGET_POS    0x607Au
#define CIA402_OD_SUBIDX        0x00u

/* ── Controlword masks ───────────────────────────────────────────────────── */
#define CIA402_CW_MASK          0x008Fu
#define CIA402_CW_SHUTDOWN      0x0006u
#define CIA402_CW_SWITCH_ON     0x0007u
#define CIA402_CW_ENABLE_OP     0x000Fu
#define CIA402_CW_QUICK_STOP    0x0002u
#define CIA402_CW_FAULT_RESET   0x0080u

/* ── Statusword values ───────────────────────────────────────────────────── */
#define CIA402_SW_ON_DISABLED   0x0040u
#define CIA402_SW_READY         0x0021u
#define CIA402_SW_SWITCHED_ON   0x0023u
#define CIA402_SW_OP_ENABLED    0x0027u
#define CIA402_SW_FAULT         0x000Fu

/* ── Rejestry widoczne w debuggerze ──────────────────────────────────────── */
typedef struct {
    uint16_t controlword;
    uint16_t prev_controlword;
    uint16_t statusword;
    int32_t  actual_pos;
    int32_t  target_pos;
    bool     sim_fault;
    uint32_t sync_count;
    uint32_t tpdo_count;
} cia402_regs_t;

extern cia402_regs_t cia402;

/* ── API ─────────────────────────────────────────────────────────────────── */
void cia402_init   (co_dev_t *dev);
void cia402_on_sync(uint8_t cnt);
void cia402_on_running(void);
void cia402_on_stopped(void);
void cia402_on_tpdo(void);


#endif /* INC_CIA402_H_ */
