/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c  [SLAVE]
  * @brief          : Autonomiczny węzeł CANopen + CiA 402 FSA
  *
  * Architektura:
  *   - Slave działa autonomicznie i zawsze utrzymuje heartbeat
  *   - Master to zewnętrzne urządzenie które się podłącza i steruje
  *   - Utrata mastera → Quick Stop + RESET_COMM (restartuje HB consumer)
  *   - Powrót mastera → automatyczne wznowienie bez NMT RESET
  *   - ECSS oscylacja wyeliminowana przez Ttoggle=30 + Bdefault dynamiczny
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "fdcan.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <lely/config.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <lely/can/msg.h>
#include <lely/can/net.h>
#include <lely/co/csdo.h>
#include <lely/co/dev.h>
#include <lely/co/nmt.h>
#include <lely/co/ssdo.h>
#include <lely/co/rpdo.h>
#include <lely/co/tpdo.h>
#include <lely/util/error.h>
#include <lely/util/mempool.h>

#include "od_slave_cia402.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define MASTER_NODE_ID     0x01u

#define BUS_A_ID           0x00u
#define BUS_B_ID           0x01u
#define BUS_A_TOKEN        0
#define BUS_B_TOKEN        1

/* CiA 402 OD indices */
#define OD_IDX_CONTROLWORD 0x6040u
#define OD_IDX_STATUSWORD  0x6041u
#define OD_IDX_ACTUAL_POS  0x6064u
#define OD_IDX_TARGET_POS  0x607Au
#define OD_SUBIDX          0x00u

/*
 * ECSS Redundancy object 0x2000:
 *   sub1 = Bdefault  — który bus jest "domem" (0=A, 1=B)
 *   sub2 = Ttoggle   — co ile heartbeat-periods testować drugi bus
 *   sub3 = Ntoggle   — ile razy próbować zanim NO_MASTER
 *
 * Ttoggle=30, HB_consumer=550ms → test co ~16s (zamiast co ~1.5s)
 * Ntoggle=2  → NO_MASTER po 2 nieudanych próbach (~32s)
 */
#define OD_IDX_RDN         0x2000u
#define OD_RDN_BDEFAULT    0x01u
#define OD_RDN_TTOGGLE     0x02u
#define OD_RDN_NTOGGLE     0x03u
#define ECSS_TTOGGLE       30u   /* 30 × 550ms ≈ 16s między testami */
#define ECSS_NTOGGLE       2u    /* 2 nieudane próby → NO_MASTER     */

/* CiA 402 FSA controlword bits */
#define CW_BITS_MASK         0x008Fu
#define CW_CMD_SHUTDOWN      0x0006u
#define CW_CMD_SWITCH_ON     0x0007u
#define CW_CMD_ENABLE_OP     0x000Fu
#define CW_CMD_QUICK_STOP    0x0002u
#define CW_BIT_FAULT_RESET   0x0080u

/* CiA 402 statusword values */
#define SW_SW_ON_DISABLED    0x0040u
#define SW_READY             0x0021u
#define SW_SWITCHED_ON       0x0023u
#define SW_OP_ENABLED        0x0027u
#define SW_FAULT             0x000Fu

#define PDO_NUM            1u
#define HB_PRODUCER_MS     500u
#define TPDO_EXPECTED_DLC  6u

#ifndef MEMORY_POOL_SIZE
#define MEMORY_POOL_SIZE (90U * 1024U)
#endif
/* USER CODE END PD */

/* USER CODE BEGIN PV */
static uint32_t mp_guard1 = 0x11223344u;
static uint8_t  memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static uint32_t mp_guard2 = 0x55667788u;
static struct   mempool pool;

static co_dev_t  *dev = NULL;
static co_nmt_t  *nmt = NULL;
static can_net_t *net = NULL;

/* CiA 402 FSA state */
static uint16_t g_statusword       = SW_SW_ON_DISABLED;
static int32_t  g_actual_pos       = 0;
static uint16_t g_controlword      = 0x0000u;
static uint16_t g_prev_controlword = 0x0000u;
static int32_t  g_target_pos       = 0;
static bool     g_sim_fault        = false;

/* Diagnostics */
static uint32_t g_sync_count = 0u;
static uint32_t g_tpdo_count = 0u;

/*
 * Stan aplikacji slave:
 *   ST_WAIT_START    — Pre-OP, czeka na NMT START od mastera
 *   ST_OPERATIONAL   — działa na bus A
 *   ST_RUNNING_BUS_B — działa na bus B
 *   ST_NO_MASTER     — master zniknął, czeka na powrót (bez NMT RESET)
 */
enum slave_state {
  ST_WAIT_START,
  ST_OPERATIONAL,
  ST_RUNNING_BUS_B,
  ST_NO_MASTER,
};
static enum slave_state g_state = ST_WAIT_START;

/* Aktywny bus — aktualizowany przez rdn_ind */
static uint8_t g_active_bus_id = BUS_A_ID;

/*
 * Flagi do operacji NMT poza callbackami.
 * co_nmt_cs_ind() wywołane z callbacku NMT jest ignorowane przez lely.
 */
static bool     g_reset_comm_pending        = false;
static bool     g_reset_comm_from_no_master = false;

/* Stan FSA zapisywany przed RESET_COMM — przywracany po resecie */
static uint16_t g_saved_statusword = SW_SW_ON_DISABLED;
static int32_t  g_saved_actual_pos = 0;
static uint8_t  g_saved_bus_id     = BUS_A_ID;

static int g_fd_a = BUS_A_TOKEN;
static int g_fd_b = BUS_B_TOKEN;
/* USER CODE END PV */

void SystemClock_Config(void);

/* USER CODE BEGIN 0 */

/* ── Logging ─────────────────────────────────────────────────────────────── */

static void HAL_LOG(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart2, (uint8_t *)s, (uint16_t)strlen(s), 100);
  const char crlf[] = "\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t *)crlf, 2, 100);
}

static void HAL_LOGF(const char *fmt, ...)
{
  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  HAL_LOG(buf);
}

void HAL_Clock_GetTime(struct timespec *tp)
{
  if (!tp) return;
  uint32_t ms = HAL_GetTick();
  tp->tv_sec  = (time_t)(ms / 1000u);
  tp->tv_nsec = (long)((ms % 1000u) * 1000000L);
}

/* ── DLC helpers ─────────────────────────────────────────────────────────── */

static uint32_t len_to_fdcan_dlc(uint8_t len)
{
  switch (len) {
    case 0:  return FDCAN_DLC_BYTES_0;
    case 1:  return FDCAN_DLC_BYTES_1;
    case 2:  return FDCAN_DLC_BYTES_2;
    case 3:  return FDCAN_DLC_BYTES_3;
    case 4:  return FDCAN_DLC_BYTES_4;
    case 5:  return FDCAN_DLC_BYTES_5;
    case 6:  return FDCAN_DLC_BYTES_6;
    case 7:  return FDCAN_DLC_BYTES_7;
    default: return FDCAN_DLC_BYTES_8;
  }
}

static uint8_t fdcan_dlc_to_len(uint32_t dlc)
{
  switch (dlc) {
    case FDCAN_DLC_BYTES_0: return 0;
    case FDCAN_DLC_BYTES_1: return 1;
    case FDCAN_DLC_BYTES_2: return 2;
    case FDCAN_DLC_BYTES_3: return 3;
    case FDCAN_DLC_BYTES_4: return 4;
    case FDCAN_DLC_BYTES_5: return 5;
    case FDCAN_DLC_BYTES_6: return 6;
    case FDCAN_DLC_BYTES_7: return 7;
    case FDCAN_DLC_BYTES_8: return 8;
    default:                return 8;
  }
}

static inline FDCAN_HandleTypeDef *fd_to_fdcan(int fd)
{
  if (fd == BUS_B_TOKEN) return &hfdcan2;
  return &hfdcan1;
}

/* ── CAN send / recv ─────────────────────────────────────────────────────── */

static bool can_send_bus(int fd, const struct can_msg *msg)
{
  if (!msg) return false;
  FDCAN_HandleTypeDef  *h = fd_to_fdcan(fd);
  FDCAN_TxHeaderTypeDef th;
  uint8_t data[8] = {0};
  memset(&th, 0, sizeof(th));
  if ((msg->id & 0x1FFFFFFFu) > 0x7FFu) {
    th.IdType     = FDCAN_EXTENDED_ID;
    th.Identifier = (uint32_t)(msg->id & 0x1FFFFFFFu);
  } else {
    th.IdType     = FDCAN_STANDARD_ID;
    th.Identifier = (uint32_t)(msg->id & 0x7FFu);
  }
  th.TxFrameType         = FDCAN_DATA_FRAME;
  th.DataLength          = len_to_fdcan_dlc((uint8_t)((msg->len > 8) ? 8 : msg->len));
  th.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  th.BitRateSwitch       = FDCAN_BRS_OFF;
  th.FDFormat            = FDCAN_CLASSIC_CAN;
  th.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
  th.MessageMarker       = 0;
  memcpy(data, msg->data, (msg->len > 8) ? 8 : msg->len);
  return (HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) == HAL_OK);
}

static bool can_recv_bus(int fd, struct can_msg *msg)
{
  if (!msg) return false;
  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);
  if (HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0) == 0u) return false;
  FDCAN_RxHeaderTypeDef rh;
  uint8_t data[8];
  if (HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &rh, data) != HAL_OK) return false;
  memset(msg, 0, sizeof(*msg));
  msg->id  = (rh.IdType == FDCAN_EXTENDED_ID)
             ? (uint_least32_t)(rh.Identifier & 0x1FFFFFFFu)
             : (uint_least32_t)(rh.Identifier & 0x7FFu);
  msg->len = fdcan_dlc_to_len(rh.DataLength);
  if (msg->len > 8) msg->len = 8;
  memcpy(msg->data, data, msg->len);
  return true;
}

static int can_send_handler(const struct can_msg *const msg,
                            const uint_least8_t bus_id, void *const data)
{
  (void)data;
  if (bus_id == BUS_B_ID) return can_send_bus(g_fd_b, msg) ? 0 : -1;
  return can_send_bus(g_fd_a, msg) ? 0 : -1;
}

/* ── ECSS helpers ────────────────────────────────────────────────────────── */

/*
 * set_bdefault() — mówi ECSS który bus jest teraz domem.
 * Dzięki temu ECSS nie wraca non-stop na stary bus.
 */
static void set_bdefault(uint8_t bus_id)
{
  const co_unsigned8_t bdef = (bus_id == BUS_B_ID) ? 1u : 0u;
  co_dev_set_val_u8(dev, OD_IDX_RDN, OD_RDN_BDEFAULT, bdef);
  HAL_LOGF("[SLAVE] ECSS Bdefault -> bus %s", (bus_id == BUS_B_ID) ? "B" : "A");
}

/*
 * set_ecss_timings() — zwiększa interwał między testami alternatywnego busa.
 * Domyślnie Ttoggle jest bardzo małe (1-3) co powoduje oscylację co ~1.5s.
 * Ttoggle=30 → test co ~16s, system pracuje stabilnie.
 */
static void set_ecss_timings(void)
{
  co_dev_set_val_u8(dev, OD_IDX_RDN, OD_RDN_TTOGGLE, ECSS_TTOGGLE);
  co_dev_set_val_u8(dev, OD_IDX_RDN, OD_RDN_NTOGGLE, ECSS_NTOGGLE);
  HAL_LOGF("[SLAVE] ECSS timings: Ttoggle=%u, Ntoggle=%u",
           ECSS_TTOGGLE, ECSS_NTOGGLE);
}

/* ── Restart heartbeat ───────────────────────────────────────────────────── */

static void restart_heartbeat(void)
{
  const co_unsigned16_t hb = HB_PRODUCER_MS;
  const int rc = co_dev_dn_val_req(dev, 0x1017u, 0x00u,
                                   CO_DEFTYPE_UNSIGNED16,
                                   &hb, NULL, NULL, NULL);
  if (rc != 0) HAL_LOG("[SLAVE] Failed to restart HB");
  else HAL_LOGF("[SLAVE] HB RESTARTED (%u ms)", HB_PRODUCER_MS);
}

/* ── Quick stop ──────────────────────────────────────────────────────────── */
/*
 * APPLICATION POINT: tutaj wstaw fizyczne zatrzymanie silnika.
 */
static void do_quick_stop(void)
{
  HAL_LOG("[SLAVE] Quick stop: holding current position");
  g_target_pos = g_actual_pos;
  co_dev_set_val_i32(dev, OD_IDX_TARGET_POS, OD_SUBIDX, g_target_pos);
  /* motor_hold_position(); */
}

/* ── Enter no-master state ───────────────────────────────────────────────── */
/*
 * Wywoływane z rdn_ind (callback NMT) — NIE wolno tu wołać co_nmt_cs_ind().
 * Ustawiamy flagę, RESET_COMM wykona się z main loop.
 */
static void enter_no_master(const char *reason)
{
  HAL_LOGF("[SLAVE] Master utracony: %s", reason ? reason : "?");
  HAL_LOG("[SLAVE] Quick stop. Czekam na powrót mastera...");
  do_quick_stop();
  g_state = ST_NO_MASTER;

  g_saved_statusword          = g_statusword;
  g_saved_actual_pos          = g_actual_pos;
  g_saved_bus_id              = g_active_bus_id;
  g_reset_comm_from_no_master = true;
  g_reset_comm_pending        = true;
}

/* ── TPDO indication ─────────────────────────────────────────────────────── */

static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr,
                     size_t n, void *data)
{
  (void)pdo; (void)ptr; (void)data;
  if (ac != 0) {
    HAL_LOGF("[SLAVE] TPDO error abort=0x%08X", (unsigned)ac);
    return;
  }
  ++g_tpdo_count;
  if (n != TPDO_EXPECTED_DLC)
    HAL_LOGF("[SLAVE] TPDO WARNING: DLC=%u (expected %u)",
             (unsigned)n, TPDO_EXPECTED_DLC);

  g_statusword = co_dev_get_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX);
  g_actual_pos = co_dev_get_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX);
  HAL_LOGF("[SLAVE] TPDO #%u: SW=0x%04X pos=%ld",
           g_tpdo_count, (unsigned)g_statusword, (long)g_actual_pos);
}

/* ── SYNC indication ─────────────────────────────────────────────────────── */

static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
  (void)nmt_; (void)data;
  ++g_sync_count;

  /* ST_NO_MASTER: tylko odśwież OD, brak ruchu */
  if (g_state == ST_NO_MASTER) {
    co_dev_set_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
    co_dev_set_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);
    HAL_LOGF("[SLAVE] SYNC #%u (NO MASTER): SW=0x%04X pos=%ld",
             g_sync_count, (unsigned)g_statusword, (long)g_actual_pos);
    return;
  }

  g_controlword = co_dev_get_val_u16(dev, OD_IDX_CONTROLWORD, OD_SUBIDX);
  g_target_pos  = co_dev_get_val_i32(dev, OD_IDX_TARGET_POS,  OD_SUBIDX);

  /* ── CiA 402 FSA ──────────────────────────────────────────────────────────
   *
   * APPLICATION POINT (SLAVE):
   *   g_target_pos → komenda do napędu
   *   g_actual_pos ← odczyt z enkodera
   *   g_statusword ← odczyt stanu HW
   *
   * Docelowy kod:
   *   if (g_statusword == SW_OP_ENABLED) {
   *     motor_set_target(g_target_pos);
   *     g_actual_pos = encoder_read_position();
   *   }
   * ────────────────────────────────────────────────────────────────────── */

  uint16_t cw_cmd         = g_controlword & CW_BITS_MASK;
  bool     fault_rst_edge = (g_controlword      & CW_BIT_FAULT_RESET) &&
                            !(g_prev_controlword & CW_BIT_FAULT_RESET);

  if (fault_rst_edge && g_statusword == SW_FAULT) {
    g_statusword = SW_SW_ON_DISABLED;
    HAL_LOG("[SLAVE] FSA: Fault reset -> SW_ON_DISABLED");
  }
  else if (g_statusword == SW_SW_ON_DISABLED && cw_cmd == CW_CMD_SHUTDOWN)
    g_statusword = SW_READY;
  else if (g_statusword == SW_READY          && cw_cmd == CW_CMD_SWITCH_ON)
    g_statusword = SW_SWITCHED_ON;
  else if (g_statusword == SW_SWITCHED_ON    && cw_cmd == CW_CMD_ENABLE_OP) {
    g_statusword = SW_OP_ENABLED;
    g_actual_pos = g_target_pos;
    HAL_LOG("[SLAVE] FSA: Operation ENABLED");
  }
  else if (g_statusword == SW_OP_ENABLED     && cw_cmd == CW_CMD_SHUTDOWN)
    g_statusword = SW_READY;
  else if (g_statusword == SW_OP_ENABLED     && cw_cmd == CW_CMD_QUICK_STOP) {
    g_statusword = SW_SW_ON_DISABLED;
    HAL_LOG("[SLAVE] FSA: Quick stop -> SW_ON_DISABLED");
  }

  if (g_sim_fault && g_statusword == SW_OP_ENABLED) {
    g_statusword = SW_FAULT;
    g_sim_fault  = false;
    HAL_LOG("[SLAVE] FSA: FAULT simulated");
  }

  /* Symulacja ruchu — 10 pulse/SYNC dąży do target */
  if (g_statusword == SW_OP_ENABLED) {
    int32_t diff = g_target_pos - g_actual_pos;
    if      (diff >  10) g_actual_pos += 10;
    else if (diff < -10) g_actual_pos -= 10;
    else                  g_actual_pos  = g_target_pos;
  }

  g_prev_controlword = g_controlword;

  co_dev_set_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
  co_dev_set_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);

  HAL_LOGF("[SLAVE] SYNC #%u (cnt=%u): CW=0x%04X SW=0x%04X tgt=%ld act=%ld",
           g_sync_count, cnt,
           (unsigned)g_controlword, (unsigned)g_statusword,
           (long)g_target_pos, (long)g_actual_pos);
}

/* ── NMT command indication ──────────────────────────────────────────────── */

static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data)
{
  (void)nmt_; (void)data;
  const char *cmd = "unknown";
  switch (cs) {
    case CO_NMT_CS_START:       cmd = "START";        break;
    case CO_NMT_CS_STOP:        cmd = "STOP";         break;
    case CO_NMT_CS_ENTER_PREOP: cmd = "ENTER PRE-OP"; break;
    case CO_NMT_CS_RESET_NODE:  cmd = "RESET NODE";   break;
    case CO_NMT_CS_RESET_COMM:  cmd = "RESET COMM";   break;
  }
  HAL_LOGF("[SLAVE] NMT: %s (0x%02X)", cmd, cs);

  /*
   * NMT START gdy byliśmy w ST_NO_MASTER — master wykrył slave w Pre-OP
   * i wysłał START. Wznów pracę bez resetu FSA.
   */
  if (cs == CO_NMT_CS_START && g_state == ST_NO_MASTER) {
    HAL_LOG("[SLAVE] NMT START w NO_MASTER -> wznawiam ST_OPERATIONAL");
    g_state                     = ST_OPERATIONAL;
    g_prev_controlword          = g_controlword;
    g_reset_comm_pending        = false;
    g_reset_comm_from_no_master = false;
    return;
  }

  if (cs == CO_NMT_CS_RESET_NODE || cs == CO_NMT_CS_RESET_COMM) {

    if (cs == CO_NMT_CS_RESET_COMM && g_reset_comm_from_no_master) {
      /*
       * Nasz RESET_COMM po utracie mastera.
       * 1. Przywróć stan FSA (statusword, pozycja)
       * 2. Ustaw Bdefault na bus gdzie był ostatnio master
       * 3. Przywróć Ttoggle/Ntoggle (RESET_COMM je resetuje do OD defaults)
       * Slave jest teraz w Pre-OP i szuka mastera na właściwym busie.
       */
      g_reset_comm_from_no_master = false;
      g_reset_comm_pending        = false;
      g_state                     = ST_NO_MASTER;
      g_statusword                = g_saved_statusword;
      g_actual_pos                = g_saved_actual_pos;
      g_prev_controlword          = 0x0000u;
      co_dev_set_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
      co_dev_set_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);

      /* Ustaw Bdefault i przywróć timingsy po RESET_COMM */
      set_bdefault(g_saved_bus_id);
      set_ecss_timings();

      HAL_LOGF("[SLAVE] RESET_COMM done. FSA: SW=0x%04X pos=%ld. Pre-OP.",
               (unsigned)g_statusword, (long)g_actual_pos);

    } else {
      /* Zewnętrzny RESET_NODE lub RESET_COMM — pełny reset */
      g_state                     = ST_WAIT_START;
      g_sync_count                = 0u;
      g_tpdo_count                = 0u;
      g_statusword                = SW_SW_ON_DISABLED;
      g_controlword               = 0x0000u;
      g_prev_controlword          = 0x0000u;
      g_target_pos                = 0;
      g_actual_pos                = 0;
      g_sim_fault                 = false;
      g_reset_comm_pending        = false;
      g_reset_comm_from_no_master = false;
      g_active_bus_id             = BUS_A_ID;
      co_dev_set_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
      co_dev_set_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);
      co_dev_set_val_i32(dev, OD_IDX_TARGET_POS, OD_SUBIDX, g_target_pos);
      set_bdefault(BUS_A_ID);
      set_ecss_timings();
      HAL_LOG("[SLAVE] FSA reset. Czekam na master START.");
    }
  }
}

/* ── Heartbeat indication ────────────────────────────────────────────────── */

static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,
                   void *data)
{
  (void)data;

  if (reason == CO_NMT_EC_TIMEOUT) {
    if (ec_state == CO_NMT_EC_OCCURRED) {
      HAL_LOGF("[SLAVE] HB TIMEOUT node 0x%02X", id);
      if (id == MASTER_NODE_ID && g_state != ST_WAIT_START) {
        HAL_LOG("[SLAVE] Master HB lost — quick stop, czekam na ECSS...");
        /*
         * Natychmiastowy quick stop przy utracie mastera.
         * Zerujemy controlword w OD żeby FSA nie przetwarzał
         * starego CW=0x000F gdy ECSS przełączy bus.
         * Napęd trzyma aktualną pozycję.
         */
        do_quick_stop();
        g_controlword      = 0x0000u;
        g_prev_controlword = 0x0000u;
        co_dev_set_val_u16(dev, OD_IDX_CONTROLWORD, OD_SUBIDX, 0x0000u);
      }
    } else {
      HAL_LOGF("[SLAVE] HB RESOLVED node 0x%02X", id);
      /* Fallback: RESET_COMM nie zadziałał, slave w OP, master wrócił */
      if (id == MASTER_NODE_ID && g_state == ST_NO_MASTER) {
        const co_unsigned8_t my_st = co_nmt_get_st(nmt);
        if (my_st == CO_NMT_ST_START) {
          HAL_LOG("[SLAVE] Master powrócił (HB RESOLVED fallback) -> ST_OPERATIONAL");
          g_state                     = ST_OPERATIONAL;
          g_prev_controlword          = g_controlword;
          g_reset_comm_pending        = false;
          g_reset_comm_from_no_master = false;
        }
      }
    }
  } else if (reason == CO_NMT_EC_STATE) {
    HAL_LOGF("[SLAVE] HB state change node 0x%02X", id);
  }

  co_nmt_on_hb(nmt_, id, ec_state, reason);
}

/* ── Redundancy indication ───────────────────────────────────────────────── */

static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,
                    co_nmt_ecss_rdn_reason_t reason, void *data)
{
  (void)nmt_; (void)data;
  const char *bus_name = (bus_id == BUS_B_ID) ? "B" : "A";

  switch (reason) {
    case CO_NMT_ECSS_RDN_BUS_SWITCH:
      HAL_LOGF("[SLAVE] REDUNDANCY: przełączono na bus %s", bus_name);
      if (bus_id == BUS_B_ID) {
        g_active_bus_id = BUS_B_ID;
        g_state         = ST_RUNNING_BUS_B;
        restart_heartbeat();
        /*
         * Ustaw Bdefault=B — ECSS traktuje B jako dom.
         * Teraz ECSS testuje A tylko co Ttoggle*HB_timeout (~16s),
         * a nie non-stop. Brak oscylacji.
         */
        set_bdefault(BUS_B_ID);
        HAL_LOG("[SLAVE] Działa na bus B.");
      } else {
        g_active_bus_id = BUS_A_ID;
        g_state         = ST_OPERATIONAL;
        set_bdefault(BUS_A_ID);
        HAL_LOG("[SLAVE] Działa na bus A.");
      }
      break;

    case CO_NMT_ECSS_RDN_NO_MASTER:
      HAL_LOGF("[SLAVE] REDUNDANCY: brak mastera (ostatni: bus %s)", bus_name);
      enter_no_master("no master on any bus");
      break;
  }
}

/* ── NMT state change ────────────────────────────────────────────────────── */

static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_unsigned8_t st, void *data)
{
  (void)nmt_; (void)data;
  const char *sn = "unknown";
  switch (st) {
    case CO_NMT_ST_BOOTUP: sn = "BOOT-UP";         break;
    case CO_NMT_ST_STOP:   sn = "STOPPED";         break;
    case CO_NMT_ST_START:  sn = "OPERATIONAL";     break;
    case CO_NMT_ST_PREOP:  sn = "PRE-OPERATIONAL"; break;
  }
  HAL_LOGF("[SLAVE] Node 0x%02X -> %s (0x%02X)", id, sn, st);

  if (id == MASTER_NODE_ID) {

    if (st == CO_NMT_ST_START) {
      const co_unsigned8_t my_st = co_nmt_get_st(nmt);

      if (my_st == CO_NMT_ST_PREOP) {
        /* Slave w Pre-OP (po RESET_COMM) — master wrócił */
        HAL_LOG("[SLAVE] Master wykryty (slave Pre-OP) -> Operational.");
        co_nmt_cs_ind(nmt, CO_NMT_CS_START);
        if (g_state == ST_WAIT_START || g_state == ST_NO_MASTER) {
          g_state                     = ST_OPERATIONAL;
          g_prev_controlword          = g_controlword;
          g_reset_comm_pending        = false;
          g_reset_comm_from_no_master = false;
          HAL_LOG("[SLAVE] ST_OPERATIONAL. Gotowy na komendy mastera.");
        }
      } else if (my_st == CO_NMT_ST_START && g_state == ST_NO_MASTER) {
        /* Fallback: slave w OP, master wrócił szybko */
        HAL_LOG("[SLAVE] Master wykryty (slave OP, fallback) -> ST_OPERATIONAL.");
        g_state                     = ST_OPERATIONAL;
        g_prev_controlword          = g_controlword;
        g_reset_comm_pending        = false;
        g_reset_comm_from_no_master = false;
      }
    }

    if (st == CO_NMT_ST_BOOTUP && g_state != ST_WAIT_START) {
      HAL_LOG("[SLAVE] Master BOOTUP — quick stop, czekam na OP.");
      do_quick_stop();
    }
  }
}

/* USER CODE END 0 */

/* ── main ────────────────────────────────────────────────────────────────── */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  assert(HAL_FDCAN_Start(&hfdcan1) == HAL_OK);
  assert(HAL_FDCAN_Start(&hfdcan2) == HAL_OK);

  alloc_t *const alloc = mempool_init(&pool, memory_pool, sizeof(memory_pool));
  assert(alloc != NULL);

  net = can_net_create(alloc, BUS_A_ID);
  assert(net != NULL);
  can_net_set_send_func(net, &can_send_handler, NULL);

  {
    struct timespec zero = {0, 0};
    int rc = can_net_set_time(net, &zero);
    assert(rc == 0); (void)rc;
  }

  dev = od_slave_init();
  assert(dev != NULL);

  co_dev_set_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX, g_statusword);
  co_dev_set_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX, g_actual_pos);
  co_dev_set_val_i32(dev, OD_IDX_TARGET_POS, OD_SUBIDX, g_target_pos);

  nmt = co_nmt_create(net, dev);
  assert(nmt != NULL);

  co_nmt_set_cs_ind(nmt,       &cs_ind,       NULL);
  co_nmt_set_hb_ind(nmt,       &hb_ind,       NULL);
  co_nmt_set_ecss_rdn_ind(nmt, &rdn_ind,      NULL);
  co_nmt_set_st_ind(nmt,       &st_ind,       NULL);
  co_nmt_set_sync_ind(nmt,     &nmt_sync_ind, NULL);

  co_tpdo_t *tpdo = co_nmt_get_tpdo(nmt, PDO_NUM);
  assert(tpdo != NULL);
  co_tpdo_set_ind(tpdo, &tpdo_ind, NULL);

  co_rpdo_t *rpdo = co_nmt_get_rpdo(nmt, PDO_NUM);
  assert(rpdo != NULL);

  /* Konfiguracja ECSS redundancji */
  {
    int arc = co_nmt_set_alternate_bus_id(nmt, BUS_B_ID);
    assert(arc == 0); (void)arc;
  }

  /*
   * KLUCZOWE: ustaw Ttoggle i Ntoggle zaraz po konfiguracji NMT.
   * Musi być PO co_nmt_create() ale PRZED co_nmt_cs_ind(RESET_NODE).
   * Ttoggle=30 → ECSS testuje drugi bus co ~16s zamiast co ~1.5s.
   * Bez tego slave oscyluje między busami co kilka sekund.
   */
  set_ecss_timings();

  {
    int rc = co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_NODE);
    assert(rc == 0); (void)rc;
  }

  HAL_LOGF("[SLAVE] Node booted (ID=0x%02X). Slave autonomiczny.",
           co_dev_get_id(dev));
  /* USER CODE END 2 */

  while (true) {
    /* USER CODE BEGIN 3 */
    struct timespec now;
    HAL_Clock_GetTime(&now);
    (void)can_net_set_time(net, &now);

    struct can_msg msg;
    while (can_recv_bus(g_fd_a, &msg))
      (void)can_net_recv(net, &msg, BUS_A_ID);
    while (can_recv_bus(g_fd_b, &msg))
      (void)can_net_recv(net, &msg, BUS_B_ID);

    switch (g_state) {

      case ST_WAIT_START:
        /* Pre-OP — czekamy na NMT START od mastera */
        break;

      case ST_OPERATIONAL:
        /* Normalna praca — FSA w nmt_sync_ind() */
        break;

      case ST_RUNNING_BUS_B:
        /* Praca na bus B */
        break;

      case ST_NO_MASTER:
        /*
         * RESET_COMM wykonywany tutaj, poza callbackiem NMT.
         * Po RESET_COMM:
         *   cs_ind() przywróci FSA i ustawi Bdefault+Ttoggle
         *   slave wraca do Pre-OP i szuka mastera
         *   st_ind(MASTER, START) → ST_OPERATIONAL
         */
        if (g_reset_comm_pending) {
          g_reset_comm_pending = false;
          HAL_LOG("[SLAVE] RESET_COMM — restartuję komunikację...");
          co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_COMM);
        }
        break;
    }

    HAL_Delay(1);
    /* USER CODE END 3 */
  }

  HAL_LOG("[SLAVE] Shutdown.");
  co_nmt_destroy(nmt);
  can_net_destroy(net);
  return 0;
}

/* ── System Clock ────────────────────────────────────────────────────────── */

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN            = 10;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

void Error_Handler(void) { __disable_irq(); while (1) {} }

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { (void)file; (void)line; }
#endif
