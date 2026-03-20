/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c  [MASTER]
  * @brief          : CANopen ECSS redundancy A↔B + CiA 402 CSP
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "fdcan.h"
#include "usart.h"
#include "gpio.h"

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
#include <lely/co/rpdo.h>
#include <lely/co/tpdo.h>
#include <lely/util/error.h>
#include <lely/util/mempool.h>
#include "od_master_cia402.h"

/* ── State machine ───────────────────────────────────────────────────────── */
enum master_state {
  ST_WAIT_SLAVE_BOOT,   /* waiting for slave to come up before NMT START  */
  ST_ERROR,             /* both buses failed                               */
  ST_CIA402_INIT,       /* SDO: set CSP mode, wait ACK                    */
  ST_CIA402_SHUTDOWN,   /* CW=0x0006, wait SW=0x0021                      */
  ST_CIA402_SWITCH_ON,  /* CW=0x0007, wait SW=0x0023                      */
  ST_CIA402_ENABLE_OP,  /* CW=0x000F, wait SW=0x0027                      */
  ST_CIA402_RUNNING,    /* APPLICATION POINT                               */
  ST_CIA402_FAULT,      /* fault reset sequence                            */
};

/* ── Defines ─────────────────────────────────────────────────────────────── */
#define MASTER_NODE_ID       0x01u
#define SLAVE_NODE_ID        0x02u
#define BUS_A_ID             0x00u
#define BUS_B_ID             0x01u
#define BUS_A_TOKEN          0
#define BUS_B_TOKEN          1

#define OD_IDX_CONTROLWORD   0x6040u
#define OD_IDX_STATUSWORD    0x6041u
#define OD_IDX_MODE_OP       0x6060u
#define OD_IDX_ACTUAL_POS    0x6064u
#define OD_IDX_TARGET_POS    0x607Au
#define OD_SUBIDX            0x00u

#define CW_SHUTDOWN          0x0006u
#define CW_SWITCH_ON         0x0007u
#define CW_ENABLE_OP         0x000Fu
#define CW_FAULT_RESET       0x0080u
#define CW_QUICK_STOP        0x0002u

#define SW_MASK_STATE        0x006Fu
#define SW_STATE_READY       0x0021u
#define SW_STATE_SWITCHED_ON 0x0023u
#define SW_STATE_OP_ENABLED  0x0027u
#define SW_STATE_FAULT       0x000Fu

#define PDO_NUM              1u
#define SLAVE_BOOT_WAIT_SEC  2
#define RPDO_WATCHDOG_SEC    5
#define FAULT_RESET_WAIT_SEC 1

/*
 * MAX_BUS_SWITCHES: max ile razy można przełączyć bus zanim ogłosimy błąd.
 * Resetowane gdy CiA402 wejdzie w RUNNING (komunikacja stabilna).
 * Duża wartość — nie chcemy przedwcześnie wchodzić w error.
 */
#define MAX_BUS_SWITCHES     4

#ifndef MEMORY_POOL_SIZE
#define MEMORY_POOL_SIZE (90U * 1024U)
#endif

/* ── Variables ───────────────────────────────────────────────────────────── */
static uint8_t      memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static struct mempool pool;

static co_dev_t  *dev  = NULL;
static co_nmt_t  *nmt  = NULL;
static can_net_t *net  = NULL;
static co_csdo_t *csdo = NULL;
static co_tpdo_t *tpdo = NULL;

static uint16_t g_statusword  = 0u;
static int32_t  g_actual_pos  = 0;
static int32_t  g_target_pos  = 0;
static uint32_t g_sync_count  = 0u;
static uint32_t g_rpdo_count  = 0u;

static enum master_state g_state            = ST_WAIT_SLAVE_BOOT;
static uint8_t           g_active_bus       = BUS_A_ID;
static uint8_t           g_bus_switch_count = 0u;

static struct timespec g_slave_boot_time;
static struct timespec g_last_rpdo_time;
static struct timespec g_fault_time;

static bool g_rpdo_ever_received = false;
static bool g_slave_hb_timeout   = false;
static bool g_sdo_done           = false;
static bool g_slave_boot_pending = false;
static bool g_cia402_sdo_sent    = false;
static bool g_fault_reset_sent   = false;

static int g_fd_a = BUS_A_TOKEN;
static int g_fd_b = BUS_B_TOKEN;

/* ── Forward declarations ────────────────────────────────────────────────── */
void SystemClock_Config(void);
static void HAL_LOG(const char *s);
static void HAL_LOGF(const char *fmt, ...);
void        HAL_Clock_GetTime(struct timespec *tp);
static uint32_t             len_to_fdcan_dlc(uint8_t len);
static uint8_t              fdcan_dlc_to_len(uint32_t dlc);
static FDCAN_HandleTypeDef *fd_to_fdcan(int fd);
static bool     can_send_bus(int fd, const struct can_msg *msg);
static bool     can_recv_bus(int fd, struct can_msg *msg);
static int      can_send_handler(const struct can_msg *msg, uint_least8_t bus_id, void *data);
static void     sdo_dn_con(co_csdo_t *sdo, co_unsigned16_t idx, co_unsigned8_t subidx,
                           co_unsigned32_t ac, void *data);
static void     hb_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_nmt_ec_state_t ec_state,
                       co_nmt_ec_reason_t reason, void *data);
static void     rpdo_ind(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void     rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec, co_unsigned8_t er, void *data);
static void     nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void     st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data);
static void     reinit_slave(void);
static void     enter_error_state(const char *reason);
static void     do_bus_switch(void);
static void     restart_cia402(void);
static void     send_controlword(uint16_t cw);
static void     send_target_pos(int32_t pos);
static uint16_t get_cia402_state(void);
static bool     application_control(void);

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
  char buf[256]; va_list ap; va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap); HAL_LOG(buf);
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
    case 0: return FDCAN_DLC_BYTES_0; case 1: return FDCAN_DLC_BYTES_1;
    case 2: return FDCAN_DLC_BYTES_2; case 3: return FDCAN_DLC_BYTES_3;
    case 4: return FDCAN_DLC_BYTES_4; case 5: return FDCAN_DLC_BYTES_5;
    case 6: return FDCAN_DLC_BYTES_6; case 7: return FDCAN_DLC_BYTES_7;
    default: return FDCAN_DLC_BYTES_8;
  }
}
static uint8_t fdcan_dlc_to_len(uint32_t dlc)
{
  switch (dlc) {
    case FDCAN_DLC_BYTES_0: return 0; case FDCAN_DLC_BYTES_1: return 1;
    case FDCAN_DLC_BYTES_2: return 2; case FDCAN_DLC_BYTES_3: return 3;
    case FDCAN_DLC_BYTES_4: return 4; case FDCAN_DLC_BYTES_5: return 5;
    case FDCAN_DLC_BYTES_6: return 6; case FDCAN_DLC_BYTES_7: return 7;
    case FDCAN_DLC_BYTES_8: return 8; default: return 8;
  }
}
static FDCAN_HandleTypeDef *fd_to_fdcan(int fd)
{
  if (fd == BUS_B_TOKEN) return &hfdcan2; return &hfdcan1;
}

/* ── CAN ─────────────────────────────────────────────────────────────────── */
static bool can_send_bus(int fd, const struct can_msg *msg)
{
  if (!msg) return false;
  FDCAN_HandleTypeDef  *h = fd_to_fdcan(fd);
  FDCAN_TxHeaderTypeDef th; uint8_t data[8] = {0}; memset(&th, 0, sizeof(th));
  if ((msg->id & 0x1FFFFFFFu) > 0x7FFu) {
    th.IdType = FDCAN_EXTENDED_ID; th.Identifier = (uint32_t)(msg->id & 0x1FFFFFFFu);
  } else { th.IdType = FDCAN_STANDARD_ID; th.Identifier = (uint32_t)(msg->id & 0x7FFu); }
  th.TxFrameType = FDCAN_DATA_FRAME;
  th.DataLength  = len_to_fdcan_dlc((uint8_t)((msg->len > 8) ? 8 : msg->len));
  th.ErrorStateIndicator = FDCAN_ESI_ACTIVE; th.BitRateSwitch = FDCAN_BRS_OFF;
  th.FDFormat = FDCAN_CLASSIC_CAN; th.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  th.MessageMarker = 0;
  memcpy(data, msg->data, (msg->len > 8) ? 8 : msg->len);
  return (HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) == HAL_OK);
}
static bool can_recv_bus(int fd, struct can_msg *msg)
{
  if (!msg) return false;
  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);
  if (HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0) == 0u) return false;
  FDCAN_RxHeaderTypeDef rh; uint8_t data[8];
  if (HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &rh, data) != HAL_OK) return false;
  memset(msg, 0, sizeof(*msg));
  msg->id  = (rh.IdType == FDCAN_EXTENDED_ID)
             ? (uint_least32_t)(rh.Identifier & 0x1FFFFFFFu)
             : (uint_least32_t)(rh.Identifier & 0x7FFu);
  msg->len = fdcan_dlc_to_len(rh.DataLength);
  if (msg->len > 8) msg->len = 8;
  memcpy(msg->data, data, msg->len); return true;
}
static int can_send_handler(const struct can_msg *msg, uint_least8_t bus_id, void *data)
{
  (void)data;
  if (bus_id == BUS_B_ID) return can_send_bus(g_fd_b, msg) ? 0 : -1;
  return can_send_bus(g_fd_a, msg) ? 0 : -1;
}

/* ── SDO ─────────────────────────────────────────────────────────────────── */
static void sdo_dn_con(co_csdo_t *sdo, co_unsigned16_t idx,
                       co_unsigned8_t subidx, co_unsigned32_t ac, void *data)
{
  (void)sdo; (void)data;
  if (ac != 0) {
    HAL_LOGF("[MASTER] SDO 0x%04X:%02X FAILED ac=0x%08X", idx, subidx, (unsigned)ac);
    enter_error_state("SDO failed");
    return;
  }
  HAL_LOGF("[MASTER] SDO 0x%04X:%02X OK", idx, subidx);
  g_sdo_done = true;
}

/* ── HB ──────────────────────────────────────────────────────────────────── */
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason, void *data)
{
  (void)data;
  if (reason == CO_NMT_EC_TIMEOUT) {
    if (ec_state == CO_NMT_EC_OCCURRED) {
      HAL_LOGF("[MASTER] HB TIMEOUT node 0x%02X (bus %s)",
               id, (g_active_bus == BUS_A_ID) ? "A" : "B");
      if (id == SLAVE_NODE_ID &&
          g_state != ST_ERROR && g_state != ST_WAIT_SLAVE_BOOT)
        g_slave_hb_timeout = true;
    } else {
      HAL_LOGF("[MASTER] HB RESOLVED node 0x%02X", id);
      /* HB wróciło — resetuj licznik żeby redundancja nie blokowała */
      if (id == SLAVE_NODE_ID) g_bus_switch_count = 0u;
    }
  } else if (reason == CO_NMT_EC_STATE) {
    HAL_LOGF("[MASTER] HB state change node 0x%02X", id);
  }
  co_nmt_on_hb(nmt_, id, ec_state, reason);
}

/* ── RPDO ────────────────────────────────────────────────────────────────── */
static void rpdo_ind(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data)
{
  (void)pdo; (void)ptr; (void)data;
  if (ac != 0) { HAL_LOGF("[MASTER] RPDO error 0x%08X", (unsigned)ac); return; }
  ++g_rpdo_count; HAL_Clock_GetTime(&g_last_rpdo_time); g_rpdo_ever_received = true;
  if (n != 6u) HAL_LOGF("[MASTER] RPDO DLC=%u", (unsigned)n);
  g_statusword = co_dev_get_val_u16(dev, OD_IDX_STATUSWORD, OD_SUBIDX);
  g_actual_pos = co_dev_get_val_i32(dev, OD_IDX_ACTUAL_POS, OD_SUBIDX);
  HAL_LOGF("[MASTER] RPDO #%lu: SW=0x%04X pos=%ld",
           (unsigned long)g_rpdo_count, (unsigned)g_statusword, (long)g_actual_pos);
}
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec, co_unsigned8_t er, void *data)
{
  (void)pdo; (void)data;
  HAL_LOGF("[MASTER] RPDO err eec=0x%04X er=0x%02X", (unsigned)eec, (unsigned)er);
}

/* ── SYNC ────────────────────────────────────────────────────────────────── */
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
  (void)nmt_; (void)data; ++g_sync_count;
  HAL_LOGF("[MASTER] SYNC #%lu (cnt=%u)", (unsigned long)g_sync_count, cnt);
}

/* ── NMT state change ────────────────────────────────────────────────────── */
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st, void *data)
{
  (void)nmt_; (void)data;
  const char *sn = "?";
  switch (st) {
    case CO_NMT_ST_BOOTUP: sn = "BOOT-UP";     break;
    case CO_NMT_ST_STOP:   sn = "STOPPED";     break;
    case CO_NMT_ST_START:  sn = "OPERATIONAL"; break;
    case CO_NMT_ST_PREOP:  sn = "PRE-OP";      break;
  }
  HAL_LOGF("[MASTER] Node 0x%02X -> %s", id, sn);

  if (id != SLAVE_NODE_ID) return;

  if (st == CO_NMT_ST_BOOTUP) {
    /* Slave zresetował się — czekaj 2s i wyślij START */
    HAL_LOG("[MASTER] Slave BOOTUP -> wait then START");
    g_slave_boot_pending = true;
    g_slave_hb_timeout   = false;
    g_sdo_done           = false;
    g_cia402_sdo_sent    = false;
    g_fault_reset_sent   = false;
    send_controlword(CW_QUICK_STOP);
    HAL_Clock_GetTime(&g_slave_boot_time);
    g_state = ST_WAIT_SLAVE_BOOT;

  } else if (st == CO_NMT_ST_PREOP) {
    /*
     * Slave w Pre-OP.
     *
     * Przypadek A — initial boot: slave jest w Pre-OP po własnym RESET_NODE.
     *   Master jest w ST_WAIT_SLAVE_BOOT, slave sam wejdzie w OP
     *   gdy zobaczy HB mastera. Ignorujemy.
     *
     * Przypadek B — recovery: slave zrobił RESET_COMM (utrata mastera)
     *   i wróci do Pre-OP. Master jest w CiA402 i musi wysłać START.
     *   Wysyłamy START natychmiast (bez 2s czekania).
     */
    if (g_state == ST_CIA402_INIT    || g_state == ST_CIA402_SHUTDOWN  ||
        g_state == ST_CIA402_SWITCH_ON || g_state == ST_CIA402_ENABLE_OP ||
        g_state == ST_CIA402_RUNNING  || g_state == ST_CIA402_FAULT) {
      HAL_LOG("[MASTER] Slave Pre-OP (recovery) -> NMT START now");
      /* Wyślij START natychmiast — slave czeka na komendę */
      const int rc = co_nmt_cs_req(nmt, CO_NMT_CS_START, SLAVE_NODE_ID);
      if (rc != 0) HAL_LOGF("[MASTER] NMT START failed rc=%d", rc);
      /* restart_cia402() zostanie wywołane przez st_ind(START) */
    }
    /* W ST_WAIT_SLAVE_BOOT — slave sam wejdzie w OP, nic nie robimy */

  } else if (st == CO_NMT_ST_START) {
    /* Slave wszedł w Operational — startuj CiA402 */
    HAL_LOG("[MASTER] Slave OPERATIONAL -> CiA402 start");
    g_slave_hb_timeout = false; /* kasuj ewentualny pending timeout */
    restart_cia402();
  }
}

/* ── Helpers ─────────────────────────────────────────────────────────────── */
static void reinit_slave(void)
{
  HAL_LOGF("[MASTER] NMT START -> slave 0x%02X", SLAVE_NODE_ID);
  if (co_nmt_cs_req(nmt, CO_NMT_CS_START, SLAVE_NODE_ID) != 0)
    HAL_LOG("[MASTER] NMT START failed");
  g_slave_boot_pending = false;
}

static void enter_error_state(const char *reason)
{
  HAL_LOGF("[MASTER] *** ERROR: %s ***", reason ? reason : "?");
  send_controlword(CW_QUICK_STOP);
  g_state = ST_ERROR;
}

static void do_bus_switch(void)
{
  if (g_bus_switch_count >= MAX_BUS_SWITCHES) {
    enter_error_state("Max bus switch attempts");
    return;
  }
  uint8_t new_bus   = (g_active_bus == BUS_A_ID) ? BUS_B_ID : BUS_A_ID;
  const char *bname = (new_bus == BUS_B_ID) ? "B" : "A";
  HAL_LOGF("[MASTER] Bus switch -> %s (attempt %u/%u)",
           bname, g_bus_switch_count + 1u, MAX_BUS_SWITCHES);
  /*
   * Nie wysyłamy CW_QUICK_STOP tutaj — slave go zobaczy po przełączeniu
   * i FSA wejdzie w SW_ON_DISABLED. Master wyśle Shutdown w ST_CIA402_INIT.
   */
  if (co_nmt_set_active_bus(nmt, new_bus) != 0) {
    enter_error_state("co_nmt_set_active_bus failed"); return;
  }
  g_active_bus = new_bus;
  g_bus_switch_count++;
  g_slave_hb_timeout = false;
  HAL_LOGF("[MASTER] Active bus: %s", bname);
  /*
   * NIE wywołujemy restart_cia402() tutaj.
   * Czekamy aż slave potwierdzi że jest w OP na nowym busie przez
   * st_ind(SLAVE, START) — wtedy wywołamy restart_cia402().
   * Gdybyśmy wysłali SDO teraz, slave jeszcze nie odpowiada → fail.
   *
   * W międzyczasie wchodzimy w ST_WAIT_SLAVE_BOOT żeby mieć jasny stan.
   */
  g_slave_boot_pending = false; /* nie czekamy 2s — slave sam się zgłosi */
  g_state = ST_WAIT_SLAVE_BOOT;
}

static void restart_cia402(void)
{
  g_cia402_sdo_sent  = false;
  g_sdo_done         = false;
  g_fault_reset_sent = false;
  g_state = ST_CIA402_INIT;
}

static void send_controlword(uint16_t cw)
{
  co_dev_set_val_u16(dev, OD_IDX_CONTROLWORD, OD_SUBIDX, cw);
  HAL_LOGF("[MASTER] CW -> 0x%04X", cw);
}
static void send_target_pos(int32_t pos)
{
  g_target_pos = pos;
  co_dev_set_val_i32(dev, OD_IDX_TARGET_POS, OD_SUBIDX, pos);
}
static uint16_t get_cia402_state(void) { return g_statusword & SW_MASK_STATE; }

/* ── APPLICATION POINT (MASTER) ─────────────────────────────────────────────
 * true  → target = 100 pulse
 * false → target = 0   pulse
 * Wpisz tu logikę swojej aplikacji.
 * ─────────────────────────────────────────────────────────────────────────── */
static bool application_control(void)
{
  static struct timespec last = {0,0}; static bool s = false;
  struct timespec now; HAL_Clock_GetTime(&now);
  if (now.tv_sec - last.tv_sec >= 5) { s = !s; last = now;
    HAL_LOGF("[MASTER] APP: target -> %s", s ? "100" : "0"); }
  return s;
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(void)
{
  HAL_Init(); SystemClock_Config();
  MX_GPIO_Init(); MX_FDCAN1_Init(); MX_FDCAN2_Init(); MX_USART2_UART_Init();

  assert(HAL_FDCAN_Start(&hfdcan1) == HAL_OK);
  assert(HAL_FDCAN_Start(&hfdcan2) == HAL_OK);

  alloc_t *alloc = mempool_init(&pool, memory_pool, sizeof(memory_pool));
  assert(alloc != NULL);

  net = can_net_create(alloc, BUS_A_ID); assert(net != NULL);
  can_net_set_send_func(net, &can_send_handler, NULL);
  { struct timespec z={0,0}; int rc=can_net_set_time(net,&z); assert(rc==0); (void)rc; }

  dev = od_master_init(); assert(dev != NULL);
  nmt = co_nmt_create(net, dev); assert(nmt != NULL);

  co_nmt_set_hb_ind(nmt,   &hb_ind,       NULL);
  co_nmt_set_st_ind(nmt,   &st_ind,       NULL);
  co_nmt_set_sync_ind(nmt, &nmt_sync_ind, NULL);

  co_rpdo_t *rpdo = co_nmt_get_rpdo(nmt, PDO_NUM); assert(rpdo != NULL);
  co_rpdo_set_ind(rpdo, &rpdo_ind, NULL);
  co_rpdo_set_err(rpdo, &rpdo_err, NULL);

  tpdo = co_nmt_get_tpdo(nmt, PDO_NUM); assert(tpdo != NULL);

  csdo = co_csdo_create(net, dev, 1u); assert(csdo != NULL);
  co_csdo_start(csdo);

  { int rc = co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_NODE); assert(rc==0); (void)rc; }

  HAL_Clock_GetTime(&g_last_rpdo_time);
  HAL_Clock_GetTime(&g_slave_boot_time);
  HAL_LOGF("[MASTER] Started (ID=0x%02X) bus A", co_dev_get_id(dev));

  while (1) {
    struct timespec now; struct can_msg msg;
    HAL_Clock_GetTime(&now); (void)can_net_set_time(net, &now);
    while (can_recv_bus(g_fd_a, &msg)) (void)can_net_recv(net, &msg, BUS_A_ID);
    while (can_recv_bus(g_fd_b, &msg)) (void)can_net_recv(net, &msg, BUS_B_ID);

    /* Globalna obsługa utraty HB slave — działa w każdym stanie */
    if (g_slave_hb_timeout) {
      g_slave_hb_timeout = false;
      do_bus_switch();
    }

    switch (g_state) {

      case ST_WAIT_SLAVE_BOOT: {
        long e = now.tv_sec - g_slave_boot_time.tv_sec;
        if (e >= SLAVE_BOOT_WAIT_SEC && g_slave_boot_pending)
          reinit_slave(); /* st_ind(START) wywoła restart_cia402() */
        break;
      }

      case ST_ERROR:
        if (g_slave_boot_pending) {
          HAL_LOG("[MASTER] Recovery from error");
          g_bus_switch_count = 0u; g_slave_boot_pending = false;
          HAL_Clock_GetTime(&g_slave_boot_time);
          g_state = ST_WAIT_SLAVE_BOOT;
        }
        break;

      case ST_CIA402_INIT:
        if (!g_cia402_sdo_sent) {
          co_unsigned8_t mode = 0x08;
          if (co_csdo_dn_val_req(csdo, OD_IDX_MODE_OP, OD_SUBIDX,
                                 CO_DEFTYPE_INTEGER8, &mode,
                                 NULL, &sdo_dn_con, NULL) == 0) {
            g_cia402_sdo_sent = true;
            HAL_LOG("[MASTER] CiA402: setting CSP mode...");
          } else enter_error_state("SDO submit failed");
        } else if (g_sdo_done) {
          g_sdo_done = false; g_cia402_sdo_sent = false;
          HAL_LOG("[MASTER] CiA402: CSP OK -> Shutdown");
          send_controlword(CW_SHUTDOWN);
          g_state = ST_CIA402_SHUTDOWN;
        }
        break;

      case ST_CIA402_SHUTDOWN:
        if (get_cia402_state() == SW_STATE_READY) {
          HAL_LOG("[MASTER] CiA402: Ready -> Switch on");
          send_controlword(CW_SWITCH_ON); g_state = ST_CIA402_SWITCH_ON;
        }
        break;

      case ST_CIA402_SWITCH_ON:
        if (get_cia402_state() == SW_STATE_SWITCHED_ON) {
          HAL_LOG("[MASTER] CiA402: Switched on -> Enable op");
          send_target_pos(g_actual_pos); /* anti-jump */
          send_controlword(CW_ENABLE_OP); g_state = ST_CIA402_ENABLE_OP;
        }
        break;

      case ST_CIA402_ENABLE_OP:
        if (get_cia402_state() == SW_STATE_OP_ENABLED) {
          HAL_LOG("[MASTER] CiA402: ENABLED");
          g_bus_switch_count = 0u; /* sukces — resetuj licznik */
          g_state = ST_CIA402_RUNNING;
        }
        break;

      /* ── APPLICATION POINT (MASTER) ──────────────────────────────────
       * Zmień application_control() żeby sterować napędem.
       * ────────────────────────────────────────────────────────────── */
      case ST_CIA402_RUNNING: {
        send_target_pos(application_control() ? 100 : 0);
        if (get_cia402_state() == SW_STATE_FAULT) {
          HAL_LOG("[MASTER] FAULT"); g_fault_reset_sent = false;
          g_state = ST_CIA402_FAULT;
        }
        if (g_rpdo_ever_received) {
          long d = now.tv_sec - g_last_rpdo_time.tv_sec;
          if (d >= RPDO_WATCHDOG_SEC)
            HAL_LOGF("[MASTER] WARNING: no RPDO %ld s", d);
        }
        break;
      }

      case ST_CIA402_FAULT:
        if (!g_fault_reset_sent) {
          send_controlword(CW_FAULT_RESET);
          HAL_Clock_GetTime(&g_fault_time); g_fault_reset_sent = true;
        } else if (now.tv_sec - g_fault_time.tv_sec >= FAULT_RESET_WAIT_SEC) {
          g_fault_reset_sent = false;
          send_controlword(CW_SHUTDOWN); g_state = ST_CIA402_SHUTDOWN;
        }
        break;
    }
    HAL_Delay(1);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState  = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1; RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}
void Error_Handler(void) { __disable_irq(); while (1) {} }
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { (void)file; (void)line; }
#endif
