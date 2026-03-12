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
#include <lely/util/error.h>
#include <lely/util/mempool.h>

#include "od_master.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
enum master_state {
  ST_RUNNING_BUS_A,
  ST_RUNNING_BUS_B,
  ST_SDO_SENT,
  ST_RUNNING_RECONFIG
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MASTER_NODE_ID 0x01u
#define SLAVE_NODE_ID  0x02u

#define BUS_A_ID 0x00u
#define BUS_B_ID 0x01u

#define BUS_A_TOKEN 0
#define BUS_B_TOKEN 1

/* To muszą odpowiadać obiektom mapowanym z TPDO slave’a */
#define OD_IDX_U32 0x2100u
#define OD_IDX_I8  0x2101u
#define OD_SUBIDX  0x00u

#define PDO_NUM 1u

#define SDO_RECONFIG_DELAY_SEC 3
#define NEW_SYNC_PERIOD_US 300000u

#ifndef MEMORY_POOL_SIZE
#define MEMORY_POOL_SIZE (90U * 1024U)
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
static struct mempool pool;

static co_dev_t  *dev  = NULL;
static co_nmt_t  *nmt  = NULL;
static can_net_t *net  = NULL;
static co_csdo_t *csdo = NULL;

static uint32_t g_sync_count = 0;
static uint32_t g_rpdo_count = 0;

static enum master_state g_state = ST_RUNNING_BUS_A;
static struct timespec g_switch_time;
static bool g_slave_hb_timeout = false;
static bool g_sdo_done = false;

/*
 * Nie trzymaj tu -1 / -1 jak w slave, bo wtedy oba busy mogą mapować się
 * na ten sam kontroler. Daj jawne tokeny.
 */
static int g_fd_a = BUS_A_TOKEN;
static int g_fd_b = BUS_B_TOKEN;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void HAL_LOG(const char *s);
static void HAL_LOGF(const char *fmt, ...);
void HAL_Clock_GetTime(struct timespec *tp);

static uint32_t len_to_fdcan_dlc(uint8_t len);
static uint8_t  fdcan_dlc_to_len(uint32_t dlc);
static FDCAN_HandleTypeDef* fd_to_fdcan(int fd);

static bool can_send_bus(int fd, const struct can_msg *msg);
static bool can_recv_bus(int fd, struct can_msg *msg);
static int  can_send_handler(const struct can_msg *msg, uint_least8_t bus_id, void *data);

static void sdo_dn_con(co_csdo_t *sdo, co_unsigned16_t idx,
                       co_unsigned8_t subidx, co_unsigned32_t ac, void *data);
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,
                   void *data);
static void rpdo_ind(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr,
                     size_t n, void *data);
static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec, co_unsigned8_t er,
                     void *data);
static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data);
static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st,
                   void *data);
static void reconfigure_slave_sync(co_unsigned32_t new_period_us);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void HAL_LOG(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart2, (uint8_t*)s, (uint16_t)strlen(s), 100);
  {
    const char crlf[] = "\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)crlf, 2, 100);
  }
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

static FDCAN_HandleTypeDef* fd_to_fdcan(int fd)
{
  if (fd == BUS_B_TOKEN) return &hfdcan2;
  return &hfdcan1;
}
//////////////////////////////////////////////////////////////////////

static bool can_send_bus(int fd, const struct can_msg *msg)
{
  if (!msg) return false;

  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);

  FDCAN_TxHeaderTypeDef th;
  uint8_t data[8] = {0};

  memset(&th, 0, sizeof(th));

  if ((msg->id & 0x1FFFFFFFu) > 0x7FFu) {
    th.IdType = FDCAN_EXTENDED_ID;
    th.Identifier = (uint32_t)(msg->id & 0x1FFFFFFFu);
  } else {
    th.IdType = FDCAN_STANDARD_ID;
    th.Identifier = (uint32_t)(msg->id & 0x7FFu);
  }

  th.TxFrameType = FDCAN_DATA_FRAME;
  th.DataLength = len_to_fdcan_dlc((uint8_t)((msg->len > 8) ? 8 : msg->len));
  th.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  th.BitRateSwitch = FDCAN_BRS_OFF;
  th.FDFormat = FDCAN_CLASSIC_CAN;
  th.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  th.MessageMarker = 0;

  {
    uint8_t len = (uint8_t)((msg->len > 8) ? 8 : msg->len);
    memcpy(data, msg->data, len);
  }

  if (HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) != HAL_OK) {
    return false;
  }

  return true;
}
static bool can_recv_bus(int fd, struct can_msg *msg)
{
  if (!msg) return false;

  FDCAN_HandleTypeDef *h = fd_to_fdcan(fd);

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
static int can_send_handler(const struct can_msg *msg,
                            const uint_least8_t bus_id, void *data)
{
  (void)data;

  if (bus_id == BUS_B_ID) {
    return can_send_bus(g_fd_b, msg) ? 0 : -1;
  }
  return can_send_bus(g_fd_a, msg) ? 0 : -1;
}
static void sdo_dn_con(co_csdo_t *sdo, co_unsigned16_t idx,
                       co_unsigned8_t subidx, co_unsigned32_t ac, void *data)
{
  (void)sdo;
  (void)data;

  if (ac != 0) {
    HAL_LOGF("[MASTER] SDO download to 0x%04X:%02X failed, abort=0x%08X",
             idx, subidx, (unsigned)ac);
    return;
  }

  HAL_LOGF("[MASTER] SDO download to 0x%04X:%02X OK", idx, subidx);

  if (idx == 0x1006u && subidx == 0x00u) {
    g_sdo_done = true;
  }
}
static void hb_ind(co_nmt_t *nmt_, co_unsigned8_t id,
                   co_nmt_ec_state_t ec_state, co_nmt_ec_reason_t reason,
                   void *data)
{
  (void)data;

  if (reason == CO_NMT_EC_TIMEOUT) {
    if (ec_state == CO_NMT_EC_OCCURRED) {
      HAL_LOGF("[MASTER] Heartbeat TIMEOUT for node 0x%02X", id);
      if (id == SLAVE_NODE_ID) {
        g_slave_hb_timeout = true;
      }
    } else {
      HAL_LOGF("[MASTER] Heartbeat timeout RESOLVED for node 0x%02X", id);
    }
  } else if (reason == CO_NMT_EC_STATE) {
    HAL_LOGF("[MASTER] Heartbeat state change for node 0x%02X", id);
  }

  co_nmt_on_hb(nmt_, id, ec_state, reason);
}
static void rpdo_ind(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr,
                     size_t n, void *data)
{
  (void)pdo;
  (void)ptr;
  (void)n;
  (void)data;

  if (ac != 0) {
    HAL_LOGF("[MASTER] RPDO error, abort=0x%08X", (unsigned)ac);
    return;
  }

  ++g_rpdo_count;

  {
    const co_unsigned32_t val_u32 = co_dev_get_val_u32(dev, OD_IDX_U32, OD_SUBIDX);
    const co_integer8_t   val_i8  = co_dev_get_val_i8(dev, OD_IDX_I8, OD_SUBIDX);

    HAL_LOGF("[MASTER] RPDO #%lu received: U32=0x%08lX I8=%d (%u bytes)",
             (unsigned long)g_rpdo_count,
             (unsigned long)val_u32,
             (int)val_i8,
             (unsigned)n);
  }
}

static void rpdo_err(co_rpdo_t *pdo, co_unsigned16_t eec, co_unsigned8_t er,
                     void *data)
{
  (void)pdo;
  (void)data;

  HAL_LOGF("[MASTER] RPDO error: eec=0x%04X er=0x%02X",
           (unsigned)eec, (unsigned)er);
}

static void nmt_sync_ind(co_nmt_t *nmt_, co_unsigned8_t cnt, void *data)
{
  (void)nmt_;
  (void)data;

  ++g_sync_count;
  HAL_LOGF("[MASTER] SYNC #%lu received (cnt=%u)",
           (unsigned long)g_sync_count, cnt);
}

static void st_ind(co_nmt_t *nmt_, co_unsigned8_t id, co_unsigned8_t st,
                   void *data)
{
  (void)nmt_;
  (void)data;

  const char *state_name = "unknown";

  switch (st) {
    case CO_NMT_ST_BOOTUP: state_name = "BOOT-UP"; break;
    case CO_NMT_ST_STOP:   state_name = "STOPPED"; break;
    case CO_NMT_ST_START:  state_name = "OPERATIONAL"; break;
    case CO_NMT_ST_PREOP:  state_name = "PRE-OPERATIONAL"; break;
  }

  HAL_LOGF("[MASTER] Node 0x%02X state -> %s (0x%02X)", id, state_name, st);
}

static void reconfigure_slave_sync(co_unsigned32_t new_period_us)
{
  HAL_LOGF("[MASTER] Sending SDO: slave SYNC period = %lu us",
           (unsigned long)new_period_us);

  {
    const int rc = co_csdo_dn_val_req(csdo, 0x1006u, 0x00u,
                                      CO_DEFTYPE_UNSIGNED32,
                                      &new_period_us,
                                      NULL,
                                      &sdo_dn_con,
                                      NULL);
    if (rc != 0) {
      HAL_LOG("[MASTER] Failed to submit SDO download");
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

  alloc_t *alloc = mempool_init(&pool, memory_pool, sizeof(memory_pool));
     assert(alloc != NULL);

     net = can_net_create(alloc, BUS_A_ID);
     assert(net != NULL);
     can_net_set_send_func(net, &can_send_handler, NULL);

     struct timespec zero = {0, 0};
     int rc = can_net_set_time(net, &zero);
     assert(rc == 0);
     (void)rc;

     dev = od_master_init();
         assert(dev != NULL);

         nmt = co_nmt_create(net, dev);
         assert(nmt != NULL);

         co_nmt_set_hb_ind(nmt, &hb_ind, NULL);
         co_nmt_set_st_ind(nmt, &st_ind, NULL);
         co_nmt_set_sync_ind(nmt, &nmt_sync_ind, NULL);
         co_rpdo_t *rpdo = co_nmt_get_rpdo(nmt, PDO_NUM);
         assert(rpdo != NULL);
         co_rpdo_set_ind(rpdo, &rpdo_ind, NULL);
         co_rpdo_set_err(rpdo, &rpdo_err, NULL);
         csdo = co_csdo_create(net, dev, 1u);
         assert(csdo != NULL);
         co_csdo_start(csdo);
         rc = co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_NODE);
         assert(rc == 0);
//         int rc = co_nmt_cs_ind(nmt, CO_NMT_CS_START);
//         assert(rc == 0);

         HAL_LOGF("[MASTER] Node started (ID=0x%02X) on bus A", co_dev_get_id(dev));
         HAL_LOGF("[MASTER] Monitoring slave Node 0x%02X heartbeat", SLAVE_NODE_ID);
         HAL_LOG("[MASTER] Receiving slave TPDOs via RPDO");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  struct timespec now;
	     struct can_msg msg;

	     HAL_Clock_GetTime(&now);
	     (void)can_net_set_time(net, &now);

	     while (can_recv_bus(g_fd_a, &msg)) {
	       (void)can_net_recv(net, &msg, BUS_A_ID);
	     }

	     while (can_recv_bus(g_fd_b, &msg)) {
	       (void)can_net_recv(net, &msg, BUS_B_ID);
	     }

	     switch (g_state) {
	       case ST_RUNNING_BUS_A:
	         if (g_slave_hb_timeout) {
	           g_slave_hb_timeout = false;

	           HAL_LOG("[MASTER] Slave heartbeat lost on bus A -> switching to bus B");

	           if (co_nmt_set_active_bus(nmt, BUS_B_ID) == 0) {
	             HAL_Clock_GetTime(&g_switch_time);
	             g_state = ST_RUNNING_BUS_B;
	             HAL_LOG("[MASTER] Active bus is now B");
	           } else {
	             HAL_LOG("[MASTER] ERROR: bus switch failed");
	           }
	         }
	         break;

	       case ST_RUNNING_BUS_B:
	       {
	         long diff_sec = now.tv_sec - g_switch_time.tv_sec;
	         if (diff_sec >= SDO_RECONFIG_DELAY_SEC) {
	           HAL_LOG("[MASTER] Reconfiguring slave SYNC period via SDO...");
	           reconfigure_slave_sync(NEW_SYNC_PERIOD_US);
	           g_state = ST_SDO_SENT;
	         }
	         break;
	       }

	       case ST_SDO_SENT:
	         if (g_sdo_done) {
	           g_sdo_done = false;
	           HAL_LOGF("[MASTER] Slave SYNC interval set to %lu ms",
	                    (unsigned long)(NEW_SYNC_PERIOD_US / 1000u));
	           g_state = ST_RUNNING_RECONFIG;
	         }
	         break;

	       case ST_RUNNING_RECONFIG:
	         break;
	     }

	     HAL_Delay(1);
  }
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
