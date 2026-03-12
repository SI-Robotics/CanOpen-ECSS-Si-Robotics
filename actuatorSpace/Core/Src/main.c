/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <lely/config.h>
#include <assert.h>
//#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <stdarg.h>
/* lely-core headers */
//#include <lelc>

#include <lely/can/msg.h>
#include <lely/can/net.h>
#include <lely/co/csdo.h>
#include <lely/co/dev.h>
#include <lely/co/nmt.h>
#include <lely/co/ssdo.h>
#include <lely/co/tpdo.h>
#include <lely/util/error.h>
#include <lely/util/mempool.h>
//#include <lely/util/time.h>

#include "od_slave.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#if defined(LELY_NO_CO_OBJ_DEFAULT)
#warning "LELY_NO_CO_OBJ_DEFAULT=1"
#else
#warning "LELY_NO_CO_OBJ_DEFAULT=0"
#endif

#if defined(LELY_NO_CO_OBJ_LIMITS)
#warning "LELY_NO_CO_OBJ_LIMITS=1"
#else
#warning "LELY_NO_CO_OBJ_LIMITS=0"
#endif

#if defined(LELY_NO_CO_OBJ_NAME)
#warning "LELY_NO_CO_OBJ_NAME=1"
#else
#warning "LELY_NO_CO_OBJ_NAME=0"
#endif

#if defined(LELY_NO_CO_OBJ_UPLOAD)
#warning "LELY_NO_CO_OBJ_UPLOAD=1"
#else
#warning "LELY_NO_CO_OBJ_UPLOAD=0"
#endif

#define MASTER_NODE_ID 0x01u

#define BUS_A_ID 0x00u
#define BUS_B_ID 0x01u

/* PDO-mapped object dictionary indices */
#define OD_IDX_U32 0x2100u
#define OD_IDX_I8 0x2101u
#define OD_SUBIDX 0x00u

/* Initial values for PDO-mapped objects */
#define INITIAL_VAL_U32 0xCAFE0000u
#define INITIAL_VAL_I8 ((co_integer8_t)0x7F)

/* PDO number (first TPDO) */
#define PDO_NUM 1u

/* Producer heartbeat time (ms) */
#define HB_PRODUCER_MS 500u

/* How long to produce heartbeats before stopping (seconds) */
#define HB_STOP_DELAY_SEC 5

/* ── Memory pool ───────────────────────────────────────────────────────── */

#ifndef MEMORY_POOL_SIZE
#define MEMORY_POOL_SIZE (90U * 1024U)
#endif


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t mp_guard1 = 0x11223344;
static uint8_t memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static uint32_t mp_guard2 = 0x55667788;
static struct mempool pool;

//static volatile sig_atomic_t g_running = 1;
static co_dev_t *dev;
static co_nmt_t *nmt;
static can_net_t *net;

static co_unsigned32_t g_val_u32 = INITIAL_VAL_U32;
static co_integer8_t g_val_i8 = INITIAL_VAL_I8;
static uint32_t g_sync_count = 0;
static uint32_t g_tpdo_count = 0;

/* State machine */
enum slave_state {
  ST_WAIT_START,    /* Waiting for master NMT START */
  ST_OPERATIONAL,   /* Running with heartbeat on bus A */
  ST_HB_STOPPED,    /* Heartbeat stopped, waiting for bus switch */
  ST_RUNNING_BUS_B, /* On bus B, heartbeat re-enabled */
};
static enum slave_state g_state = ST_WAIT_START;
static struct timespec g_op_start_time;

/* ── Dual CAN sockets (vcan0 = bus A, vcan1 = bus B) ──────────────────── */

static int g_fd_a = -1; /* vcan0 */
static int g_fd_b = -1; /* vcan1 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


static void HAL_LOG(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart2, (uint8_t*)s, (uint16_t)strlen(s), 100);
  const char crlf[] = "\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)crlf, 2, 100);
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
/* --- STM32 replacement for x86 HAL_Clock_GetTime() --- */
void HAL_Clock_GetTime(struct timespec *tp)
{
  if (!tp) return;

  uint32_t ms = HAL_GetTick();            /* ms since boot */
  tp->tv_sec  = (time_t)(ms / 1000u);
  tp->tv_nsec = (long)((ms % 1000u) * 1000000L);
}

/* ===== Helpers: DLC map ===== */
static uint32_t len_to_fdcan_dlc(uint8_t len)
{
  switch (len) {
    case 0: return FDCAN_DLC_BYTES_0;
    case 1: return FDCAN_DLC_BYTES_1;
    case 2: return FDCAN_DLC_BYTES_2;
    case 3: return FDCAN_DLC_BYTES_3;
    case 4: return FDCAN_DLC_BYTES_4;
    case 5: return FDCAN_DLC_BYTES_5;
    case 6: return FDCAN_DLC_BYTES_6;
    case 7: return FDCAN_DLC_BYTES_7;
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
    default: return 8;
  }
}

/* ===== fd -> FDCAN handle ===== */
static inline FDCAN_HandleTypeDef* fd_to_fdcan(int fd)
{
  if (fd == g_fd_b) return &hfdcan2;   /* bus B */
  return &hfdcan1;                    /* bus A (default) */
}

static bool can_send_bus(int fd, const struct can_msg *msg)
{
  if (!msg) return false;

  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);

  FDCAN_TxHeaderTypeDef th;
  uint8_t data[8] = {0};

  memset(&th, 0, sizeof(th));

  /* Heurystyka: ID > 0x7FF traktujemy jako extended */
  if ((msg->id & 0x1FFFFFFFu) > 0x7FFu) {
    th.IdType = FDCAN_EXTENDED_ID;
    th.Identifier = (uint32_t)(msg->id & 0x1FFFFFFFu);
  } else {
    th.IdType = FDCAN_STANDARD_ID;
    th.Identifier = (uint32_t)(msg->id & 0x7FFu);
  }

  th.TxFrameType = FDCAN_DATA_FRAME;
  th.DataLength  = len_to_fdcan_dlc((uint8_t)((msg->len > 8) ? 8 : msg->len));
  th.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  th.BitRateSwitch = FDCAN_BRS_OFF;          /* Classic CAN */
  th.FDFormat = FDCAN_CLASSIC_CAN;           /* Classic CAN */
  th.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  th.MessageMarker = 0;

  uint8_t len = (uint8_t)((msg->len > 8) ? 8 : msg->len);
  memcpy(data, msg->data, len);

  /* UWAGA: musisz mieć HAL_FDCAN_Start() zrobione wcześniej */
  if (HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) != HAL_OK) {
    return false;
  }
  return true;
}

static bool can_recv_bus(int fd, struct can_msg *msg)
{
  if (!msg) return false;

  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);

  /* Non-blocking: jak FIFO puste -> false */
  if (HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0) == 0u) {
    return false;
  }

  FDCAN_RxHeaderTypeDef rh;
  uint8_t data[8];

  if (HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &rh, data) != HAL_OK) {
    return false;
  }

  memset(msg, 0, sizeof(*msg));

  if (rh.IdType == FDCAN_EXTENDED_ID) {
    msg->id = (uint_least32_t)(rh.Identifier & 0x1FFFFFFFu);
  } else {
    msg->id = (uint_least32_t)(rh.Identifier & 0x7FFu);
  }

  msg->len = fdcan_dlc_to_len(rh.DataLength);
  if (msg->len > 8) msg->len = 8;
  memcpy(msg->data, data, msg->len);

  return true;
}

/* ── CAN send handler (routes by bus_id) ───────────────────────────────── */

static int can_send_handler(const struct can_msg *const msg,
                            const uint_least8_t bus_id, void *const data) {
  (void)data;

  if (bus_id == BUS_B_ID) {
    return can_send_bus(g_fd_b, msg) ? 0 : -1;
  }
  /* Default: bus A */
  return can_send_bus(g_fd_a, msg) ? 0 : -1;
}

/* ── Signal handler ────────────────────────────────────────────────────── */

//static void signal_handler(int sig)
//{
//  (void)sig;
//  g_running = 0;
//}

/* ── Stop heartbeat production ─────────────────────────────────────────── */

static void stop_heartbeat(void) {
  const co_unsigned16_t zero = 0;
  const int rc = co_dev_dn_val_req(dev, 0x1017u, 0x00u, CO_DEFTYPE_UNSIGNED16,
                                   &zero, NULL, NULL, NULL);
  if (rc != 0) {
    HAL_LOG("[SLAVE] Failed to set producer heartbeat time to 0");
  } else {
    HAL_LOG("[SLAVE] Heartbeat producer STOPPED (set to 0 ms)");
  }
}

/* ── Restart heartbeat production ──────────────────────────────────────── */

static void restart_heartbeat(void) {
  const co_unsigned16_t hb = HB_PRODUCER_MS;
  const int rc = co_dev_dn_val_req(dev, 0x1017u, 0x00u, CO_DEFTYPE_UNSIGNED16,
                                   &hb, NULL, NULL, NULL);
  if (rc != 0) {
    HAL_LOG("[SLAVE] Failed to restart heartbeat producer");
  } else {
    HAL_LOGF("[SLAVE] Heartbeat producer RESTARTED (%u ms)", HB_PRODUCER_MS);
  }
}

/* ── TPDO indication callback ──────────────────────────────────────────── */

static void tpdo_ind(co_tpdo_t *pdo, co_unsigned32_t ac, const void *ptr,
                     size_t n, void *data) {
  (void)pdo;
  (void)ptr;
  (void)n;
  (void)data;

  if (ac != 0) {
    HAL_LOGF("[SLAVE] TPDO error, abort code 0x%08X", (unsigned)ac);
    return;
  }

  ++g_tpdo_count;
  HAL_LOGF("[SLAVE] TPDO #%u sent: U32=0x%08X, I8=%d (%u bytes)", g_tpdo_count,
           (unsigned)g_val_u32, (int)g_val_i8, (unsigned)n);
}

/* ── NMT SYNC indication callback ─────────────────────────────────────── */

static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data) {
  (void)nmt_;
  (void)data;

  ++g_sync_count;

  /* Update OD values for the next SYNC cycle */
  ++g_val_u32;
  --g_val_i8;

  co_dev_set_val_u32(dev, OD_IDX_U32, OD_SUBIDX, g_val_u32);
  co_dev_set_val_i8(dev, OD_IDX_I8, OD_SUBIDX, g_val_i8);

  HAL_LOGF("[SLAVE] SYNC #%u produced (cnt=%u)", g_sync_count, cnt);
}

/* ── NMT command indication ────────────────────────────────────────────── */

static void cs_ind(co_nmt_t *nmt_, co_unsigned8_t cs, void *data) {
  (void)nmt_;
  (void)data;

  const char *cmd_name = "unknown";
  switch (cs) {
  case CO_NMT_CS_START:
    cmd_name = "START";
    break;
  case CO_NMT_CS_STOP:
    cmd_name = "STOP";
    break;
  case CO_NMT_CS_ENTER_PREOP:
    cmd_name = "ENTER PRE-OP";
    break;
  case CO_NMT_CS_RESET_NODE:
    cmd_name = "RESET NODE";
    break;
  case CO_NMT_CS_RESET_COMM:
    cmd_name = "RESET COMM";
    break;
  }

  HAL_LOGF("[SLAVE] Received NMT command: %s (0x%02X)", cmd_name, cs);
}

/* ── NMT heartbeat indication ─────────────────────────────────────────── */

static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,
                   void *data) {
  (void)data;

  if (reason == CO_NMT_EC_TIMEOUT) {
    if (ec_state == CO_NMT_EC_OCCURRED) {
      HAL_LOGF("[SLAVE] Heartbeat TIMEOUT for Node 0x%02X "
               "(master lost on current bus)",
               id);
    } else {
      HAL_LOGF("[SLAVE] Heartbeat timeout RESOLVED for Node 0x%02X "
               "(master found again)",
               id);
    }
  } else if (reason == CO_NMT_EC_STATE) {
    HAL_LOGF("[SLAVE] Heartbeat state change for Node 0x%02X", id);
  }

  /* Let lely-core handle the internal NMT / redundancy actions */
  co_nmt_on_hb(nmt_, id, ec_state, reason);
}

/* ── NMT redundancy indication ─────────────────────────────────────────── */

static void rdn_ind(co_nmt_t *nmt_, co_unsigned8_t bus_id,
                    co_nmt_ecss_rdn_reason_t reason, void *data) {
  (void)nmt_;
  (void)data;

  const char *bus_name = (bus_id == BUS_B_ID) ? "B (vcan1)" : "A (vcan0)";

  switch (reason) {
  case CO_NMT_ECSS_RDN_BUS_SWITCH:
    HAL_LOGF("[SLAVE] REDUNDANCY: Bus switch — now active on bus %s", bus_name);
    /* Re-enable heartbeat after switching to redundant bus */
    if (g_state == ST_HB_STOPPED && bus_id == BUS_B_ID) {
      restart_heartbeat();
      g_state = ST_RUNNING_BUS_B;
      HAL_LOG(
          "[SLAVE] Now running on redundant bus B with all services active.");
    }
    break;
  case CO_NMT_ECSS_RDN_NO_MASTER:
    HAL_LOGF("[SLAVE] REDUNDANCY: No master detected after toggling! "
             "Entering no-master operation on bus %s",
             bus_name);
    break;
  }
}

/* ── NMT state-change indication ───────────────────────────────────────── */

static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st,
                   void *data) {
  (void)nmt_;
  (void)data;

  const char *state_name = "unknown";
  switch (st) {
  case CO_NMT_ST_BOOTUP:
    state_name = "BOOT-UP";
    break;
  case CO_NMT_ST_STOP:
    state_name = "STOPPED";
    break;
  case CO_NMT_ST_START:
    state_name = "OPERATIONAL";
    break;
  case CO_NMT_ST_PREOP:
    state_name = "PRE-OPERATIONAL";
    break;
  }

  HAL_LOGF("[SLAVE] Node 0x%02X state -> %s (0x%02X)", id, state_name, st);

  /* When we detect the master's heartbeat during bus selection
     (in Pre-operational), transition to Operational */
  if (id == MASTER_NODE_ID && st == CO_NMT_ST_START) {
    const co_unsigned8_t my_st = co_nmt_get_st(nmt);
    if (my_st == CO_NMT_ST_PREOP) {
      HAL_LOG("[SLAVE] Master detected — entering Operational.");
      co_nmt_cs_ind(nmt, CO_NMT_CS_START);
      /* Only enter the heartbeat-stop countdown on first boot;
         after redundancy switch, stay in ST_RUNNING_BUS_B */
      if (g_state == ST_WAIT_START) {
        g_state = ST_OPERATIONAL;
        HAL_Clock_GetTime(&g_op_start_time);
        HAL_LOG("[SLAVE] Now producing: heartbeat (500ms), SYNC (1s), TPDO "
                "(every 3rd SYNC).");
      } else {
        HAL_LOG("[SLAVE] Re-entered Operational on redundant bus.");
      }
    }
  }
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  assert(HAL_FDCAN_Start(&hfdcan1) == HAL_OK);
  assert(HAL_FDCAN_Start(&hfdcan2) == HAL_OK);
  /* 3. Memory-pool allocator */
    alloc_t *const alloc = mempool_init(&pool, memory_pool, sizeof(memory_pool));
    assert(alloc != NULL);

    /* 4. Create CAN network (start on bus A = 0) */
    net = can_net_create(alloc, BUS_A_ID);
    assert(net != NULL);
    can_net_set_send_func(net, &can_send_handler, NULL);

    {
      struct timespec zero = {0, 0};
      int rc = can_net_set_time(net, &zero);
      assert(rc == 0);
      (void)rc;
    }

    /* 5. Initialise device from static descriptor */
    dev = od_slave_init();
    assert(dev != NULL);

    /* 6. Set initial values in object dictionary */
    co_dev_set_val_u32(dev, OD_IDX_U32, OD_SUBIDX, g_val_u32);
    co_dev_set_val_i8(dev, OD_IDX_I8, OD_SUBIDX, g_val_i8);

    /* 7. Create NMT service — internally creates SYNC, TPDO, SDO services */
    nmt = co_nmt_create(net, dev);
    assert(nmt != NULL);

    /* 8. Set NMT callbacks */
    co_nmt_set_cs_ind(nmt, &cs_ind, NULL);
    co_nmt_set_hb_ind(nmt, &hb_ind, NULL);
    co_nmt_set_ecss_rdn_ind(nmt, &rdn_ind, NULL);
    co_nmt_set_st_ind(nmt, &st_ind, NULL);

    /* 9. Set SYNC indication on NMT */
    co_nmt_set_sync_ind(nmt, &nmt_sync_ind, NULL);

    /* 10. Set TPDO indication on the NMT-managed TPDO service */
    co_tpdo_t *tpdo = co_nmt_get_tpdo(nmt, PDO_NUM);
    assert(tpdo != NULL);
    co_tpdo_set_ind(tpdo, &tpdo_ind, NULL);

    /* 11. Configure the alternate bus for the redundancy manager */
    int arc = co_nmt_set_alternate_bus_id(nmt, BUS_B_ID);
    assert(arc == 0);
    (void)arc;

    /* 12. Boot the node — enters Pre-operational for bus selection phase */
    int rc = co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_NODE);
    assert(rc == 0);
    (void)rc;

    HAL_LOGF("[SLAVE] Node booted (ID=0x%02X). Waiting for master to start us.",
             co_dev_get_id(dev));
    HAL_LOGF("[SLAVE] Will stop heartbeat %d seconds after entering Operational.",
             HB_STOP_DELAY_SEC);
    HAL_LOG("[SLAVE] Press Ctrl-C to quit.");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (true)
  {

	  struct timespec now;
	      HAL_Clock_GetTime(&now);
	      (void)can_net_set_time(net, &now);

	      /* Receive CAN frames from BOTH buses */
	      struct can_msg msg;
	      while (can_recv_bus(g_fd_a, &msg)) {
	        (void)can_net_recv(net, &msg, BUS_A_ID);
	      }
	      while (can_recv_bus(g_fd_b, &msg)) {
	        (void)can_net_recv(net, &msg, BUS_B_ID);
	      }

	      /* State machine */
	      switch (g_state) {
	      case ST_WAIT_START:
	        /* Waiting for master NMT START — nothing to do */
	        break;

	      case ST_OPERATIONAL: {
	        long elapsed_sec = now.tv_sec - g_op_start_time.tv_sec;
	        if (elapsed_sec >= HB_STOP_DELAY_SEC) {
	          HAL_LOGF("[SLAVE] %d seconds elapsed — stopping heartbeat "
	                   "to trigger redundancy switch.",
	                   HB_STOP_DELAY_SEC);
	          stop_heartbeat();
	          g_state = ST_HB_STOPPED;
	        }
	        break;
	      }

	      case ST_HB_STOPPED:
	        /* Waiting for redundancy manager to switch buses */
	        break;

	      case ST_RUNNING_BUS_B:
	        /* Normal operation on bus B */
	        break;
	      }

	      HAL_Delay(1);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  HAL_LOG("[SLAVE] Shutting down...");

  co_nmt_destroy(nmt);
  can_net_destroy(net);
//  close_can_socket(g_fd_a);
//  close_can_socket(g_fd_b);

  HAL_LOGF("[SLAVE] Total: %u SYNC produced, %u TPDOs transmitted.",
           g_sync_count, g_tpdo_count);
  HAL_LOG("[SLAVE] Bye.");
  return 0;
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
