/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c  [MASTER]
  * @brief          : CANopen ECSS redundancy A↔B + CiA 402 CSP
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "canopen_master.h"
#include "cia402_master.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* ── CAN send / recv ─────────────────────────────────────────────────────── */
static bool my_can_send(uint8_t bus_id, const struct can_msg *msg)
{
    FDCAN_HandleTypeDef *h = (bus_id == CANOPEN_BUS_B_ID) ? &hfdcan2 : &hfdcan1;
    FDCAN_TxHeaderTypeDef th = {0};
    uint8_t data[8] = {0};
    uint8_t len = (msg->len > 8u) ? 8u : (uint8_t)msg->len;
    th.IdType     = ((msg->id & 0x1FFFFFFFu) > 0x7FFu) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    th.Identifier = (uint32_t)(msg->id & ((th.IdType == FDCAN_EXTENDED_ID) ? 0x1FFFFFFFu : 0x7FFu));
    th.TxFrameType        = FDCAN_DATA_FRAME;
    th.DataLength         = len_to_fdcan_dlc(len);
    th.ErrorStateIndicator= FDCAN_ESI_ACTIVE;
    th.BitRateSwitch      = FDCAN_BRS_OFF;
    th.FDFormat           = FDCAN_CLASSIC_CAN;
    th.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    if ((msg->id & 0x780u) == 0x200u)
    {
        MAIN_LOGF("[MASTER] sending RPDO id=0x%03lX len=%u",
                  (unsigned long)msg->id, msg->len);
    }
    memcpy(data, msg->data, len);
    bool result = HAL_FDCAN_AddMessageToTxFifoQ(h, &th, data) == HAL_OK;
    if ((msg->id & 0x780u) == 0x200u)
    {
        MAIN_LOGF("[MASTER] RPDO send id=0x%03lX len=%u result=%s",
                  (unsigned long)msg->id, msg->len,
                  result ? "OK" : "FAIL");
    }
    return result;
}

static bool my_can_recv(uint8_t bus_id, struct can_msg *msg)
{
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

static void my_get_time(struct timespec *tp)
{
    if (!tp) return;
    uint32_t ms = HAL_GetTick();
    tp->tv_sec  = (time_t)(ms / 1000u);
    tp->tv_nsec = (long)((ms % 1000u) * 1000000L);
}
static void app_on_sync   (uint8_t cnt)
{
	cia402_master_on_sync(cnt);
}

static void app_on_running(void)
{
	cia402_master_on_running();
}

static void app_on_stopped(void)
{
	cia402_master_on_stopped();
}

static void app_on_rpdo   (uint16_t sw, int32_t actual_pos)
{
	cia402_master_on_rpdo(sw, actual_pos);
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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  assert(HAL_FDCAN_Start(&hfdcan1) == HAL_OK);
  assert(HAL_FDCAN_Start(&hfdcan2) == HAL_OK);
  HAL_TIM_Base_Start_IT(&htim6);

  canopen_config cfg = {
      .send = my_can_send,
      .recv = my_can_recv,
      .time = my_get_time,
      .on_sync    = app_on_sync,
      .on_running = app_on_running,
      .on_stopped = app_on_stopped,
      .on_rpdo    = app_on_rpdo,
  };
  assert(canopen_init(cfg) == CANOPEN_OK);
  cia402_master_init(canopen.dev, canopen.csdo);



  static const int32_t positions[] = { 0, 1000, -1000, 500, -500 };
  static uint8_t       pos_idx     = 0u;
  static uint32_t      last_tick   = 0u;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	     uint32_t now1 = HAL_GetTick();
	        if (now1 - last_tick >= 3000u &&
	            cia402_master.state == CIA402_M_RUNNING)
	        {
	            last_tick = now1;
	            pos_idx   = (pos_idx + 1u) % 5u;
	            cia402_master_set_target(positions[pos_idx]);
	        }

      canopen_process();
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if RPDO_FLOOD_TEST_ENABLE
    if (htim->Instance == TIM6)
    {
        struct can_msg sync = {0};
        sync.id  = 0x080u;
        sync.len = 0u;
        canopen.cfg.send(canopen.bus_id, &sync);
    }
#endif
#if SDO_PDO_COMBINED_TEST_ENABLE
    if (htim->Instance == TIM6)
    {
        struct can_msg sync = {0};
        sync.id  = 0x080u;
        sync.len = 0u;
        canopen.cfg.send(canopen.bus_id, &sync);
    }
#endif
}
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
