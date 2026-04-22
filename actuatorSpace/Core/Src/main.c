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
#include "canopen.h"
#include "cia402.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */

//#define TPDO_FLOOD_TEST_ENABLE   1
//#define TPDO_FLOOD_START_MS      20000u
//#define TPDO_FLOOD_EXTRA         9u   /* ile dodatkowych TPDO na SYNC (łącznie 10x) */

/* USER CODE END PD */

/* USER CODE BEGIN PV */


/* USER CODE END PV */

void SystemClock_Config(void);

/* USER CODE BEGIN 0 */

static void MAIN_LOGF(const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf,
                      (uint16_t)strlen(buf), 100);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
}

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

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan,uint32_t ErrorStatusITs)
{
    if (ErrorStatusITs & FDCAN_IT_BUS_OFF)
    {
        HAL_UART_Transmit(&huart2,
            (uint8_t *)"[SLAVE] BUS-OFF detected on FDCAN",
            33, 100);

        /* Próba odzyskania — zatrzymaj i restart kontrolera */
        uint8_t bus_id = (hfdcan == &hfdcan2)
                         ? CANOPEN_BUS_B_ID
                         : CANOPEN_BUS_A_ID;

        HAL_FDCAN_Stop(hfdcan);
        HAL_FDCAN_Start(hfdcan);

        HAL_UART_Transmit(&huart2,
            (uint8_t *)"[SLAVE] FDCAN restarted after bus-off\r\n",
            39, 100);
        (void)bus_id;
    }
}

/* ── CAN send ────────────────────────────────────────────────────────────── */

static bool my_can_send(uint8_t bus_id, const struct can_msg *msg)
{
    if (!msg) return false;

    FDCAN_HandleTypeDef  *h  = (bus_id == CANOPEN_BUS_B_ID) ? &hfdcan2 : &hfdcan1;
    FDCAN_TxHeaderTypeDef th = {0};
    uint8_t data[8]          = {0};
    uint8_t len              = (msg->len > 8u) ? 8u : (uint8_t)msg->len;

    if ((msg->id & 0x1FFFFFFFu) > 0x7FFu) {
        th.IdType     = FDCAN_EXTENDED_ID;
        th.Identifier = (uint32_t)(msg->id & 0x1FFFFFFFu);
    } else {
        th.IdType     = FDCAN_STANDARD_ID;
        th.Identifier = (uint32_t)(msg->id & 0x7FFu);
    }

    th.TxFrameType         = FDCAN_DATA_FRAME;
    th.DataLength          = len_to_fdcan_dlc(len);
    th.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    th.BitRateSwitch       = FDCAN_BRS_OFF;
    th.FDFormat            = FDCAN_CLASSIC_CAN;
    th.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;

    memcpy(data, msg->data, len);
    return HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) == HAL_OK;
}


/* ── CAN recv ────────────────────────────────────────────────────────────── */

static bool my_can_recv(uint8_t bus_id, struct can_msg *msg)
{
    if (!msg) return false;

    FDCAN_HandleTypeDef *h = (bus_id == CANOPEN_BUS_B_ID) ? &hfdcan2 : &hfdcan1;

    if (HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0) == 0u) return false;

    FDCAN_RxHeaderTypeDef rh;
    uint8_t data[8];

    if (HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &rh, data) != HAL_OK) return false;

    memset(msg, 0, sizeof(*msg));
    msg->id  = (rh.IdType == FDCAN_EXTENDED_ID)
               ? (uint_least32_t)(rh.Identifier & 0x1FFFFFFFu)
               : (uint_least32_t)(rh.Identifier & 0x7FFu);
    msg->len = fdcan_dlc_to_len(rh.DataLength);
    if (msg->len > 8u) msg->len = 8u;
    memcpy(msg->data, data, msg->len);

    return true;
}
/* ── czas ────────────────────────────────────────────────────────────────── */

static void my_get_time(struct timespec *tp)
{
    if (!tp) return;
    uint32_t ms  = HAL_GetTick();
    tp->tv_sec   = (time_t)(ms / 1000u);
    tp->tv_nsec  = (long)((ms % 1000u) * 1000000L);
}
















static void app_on_sync   (uint8_t cnt) { cia402_on_sync(cnt);    }
static void app_on_running(void)        { cia402_on_running();     }
static void app_on_stopped(void)        { cia402_on_stopped();     }
static void app_on_tpdo   (void)        { cia402_on_tpdo();        }



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

  HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_BUS_OFF, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_BUS_OFF, 0);
  canopen_config cfg =
  {
      .send = my_can_send,
      .recv = my_can_recv,
      .time = my_get_time,
      .on_sync    = app_on_sync,
      .on_running = app_on_running,
      .on_stopped = app_on_stopped,
      .on_tpdo    = app_on_tpdo,
  };

  assert(canopen_init(cfg) == CANOPEN_OK);
  cia402_init(canopen.dev);




  /* USER CODE END 2 */

  while (true) {
    /* USER CODE BEGIN 3 */
      canopen_process();

    HAL_Delay(1);

      /* USER CODE END 3 */
  }

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
