/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "canopen_space.h"
#include "cia402_wrapper.h"
#include "actuator.h"

#define SERVO_US_MIN  1500u
#define SERVO_US_MAX  1900u

#define POS_0_DEG    ((int32_t)0)
#define POS_90_DEG   ((int32_t)9000)

#define ACTUATOR
//#define MASTER

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static inline bool btn_pressed(void)
{
  GPIO_PinState s = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
  return (s == GPIO_PIN_RESET);
}

static void servo_write_us(uint16_t us)
{
  if (us <= SERVO_US_MIN) us = SERVO_US_MIN;
  if (us >= SERVO_US_MAX) us = SERVO_US_MAX;

  // jeśli timer ma 1MHz -> CCR = us
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, us);
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#define MEMPOOL_SIZE     (80u * 1024u)   // jak zabraknie, daj 48k/64k

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

static uint8_t co_mem[MEMPOOL_SIZE] __attribute__((aligned(16)));



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint8_t dlc_to_len(uint32_t dlc)
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
    default:               return 8;
  }
}

static uint32_t len_to_dlc(uint8_t len)
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
    default:return FDCAN_DLC_BYTES_8;
  }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void FDCAN_AcceptAll(FDCAN_HandleTypeDef *h)
{
  FDCAN_FilterTypeDef f = {0};
  f.IdType       = FDCAN_STANDARD_ID;
  f.FilterIndex  = 0;
  f.FilterType   = FDCAN_FILTER_MASK;
  f.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  f.FilterID1    = 0x000;
  f.FilterID2    = 0x000; // mask=0 => accept all
  HAL_FDCAN_ConfigFilter(h, &f);

  HAL_FDCAN_ConfigGlobalFilter(h,
    FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0,
    FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0) return;

  FDCAN_RxHeaderTypeDef rxh;
  uint8_t data[8];

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxh, data) != HAL_OK)
    return;

  uint_least8_t bus_id = (hfdcan == &hfdcan1) ? 0 : 1;

  bool ext = (rxh.IdType == FDCAN_EXTENDED_ID);
  bool rtr = (rxh.RxFrameType == FDCAN_REMOTE_FRAME);

  uint8_t len = dlc_to_len(rxh.DataLength);

  canopen_space_on_rx(bus_id, rxh.Identifier, ext, rtr, data, len);
	#if defined(MASTER)
	  // Zaktualizuj rejestry CiA402 na podstawie TPDO1 (0x180 + node)
	  // To musi być w RX, bo master dostaje TPDO jako ramkę CAN.
//	  cia402_master_pdo_parse_tpdo1(rxh.Identifier, data, len);
	  actuator_master_rx_update(rxh.Identifier, data, len);
	#endif
}

static int co_send(uint_least8_t bus_id, const struct can_msg* msg, void* user)
{
  (void)user;

  FDCAN_HandleTypeDef* h = (bus_id == 0) ? &hfdcan1 : &hfdcan2;

  FDCAN_TxHeaderTypeDef txh = {0};
  txh.Identifier = msg->id;
  txh.IdType = (msg->flags & CAN_FLAG_IDE) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  txh.TxFrameType = (msg->flags & CAN_FLAG_RTR) ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;
  txh.DataLength = len_to_dlc((msg->len > 8) ? 8 : msg->len);

  // Classic CAN (bez FD)
  txh.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txh.BitRateSwitch       = FDCAN_BRS_OFF;
  txh.FDFormat            = FDCAN_CLASSIC_CAN;
  txh.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
  txh.MessageMarker       = 0;

  uint8_t payload[8] = {0};
  if (msg->len) memcpy(payload, msg->data, (msg->len > 8) ? 8 : msg->len);

  if (HAL_FDCAN_AddMessageToTxFifoQ(h, &txh, payload) != HAL_OK) {
    return -1;
  }
  return 0;
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  FDCAN_AcceptAll(&hfdcan1);
  FDCAN_AcceptAll(&hfdcan2);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  HAL_FDCAN_Start(&hfdcan1);
  HAL_FDCAN_Start(&hfdcan2);

  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
#if defined(MASTER)
  canopen_space_cfg_t cfg =
  {
    .role = CO_ROLE_MASTER,
    .node_id = 1,
    .send = co_send,
    .send_user = NULL,
    .mem = co_mem,
    .mem_size = sizeof(co_mem),
  };
  cia402_master_set_node_bus(2, 0);
  actuator_init();
#endif
#if defined(ACTUATOR)
  canopen_space_cfg_t cfg =
  {
    .role = CO_ROLE_ACTUATOR,
    .node_id = 2,
    .send = co_send,
    .send_user = NULL,
    .mem = co_mem,
    .mem_size = sizeof(co_mem),
  };
  cia402_actuator_set_node_bus(2, 0);
#endif

  (void)canopen_space_init(&cfg);
  (void)canopen_space_start();



  uint32_t ms = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    HAL_Delay(1);
	    ms++;
	    canopen_space_tick_1ms();
		#if defined(MASTER)
		static bool started = false;

		if (!started && ms >= 1000) {
		  started = true;
		  (void)cia402_send_nmt(0x01, 0x00, 0); // START ALL
		}

		if (started && (ms % 10) == 0) {
//		  static int32_t pos = 0;
//		  pos += 100;
		  int32_t target = btn_pressed() ? POS_0_DEG : POS_90_DEG;
		  uint16_t cw = 0x000F;
		  actuator_master_set_controlword(cw);
		  actuator_master_set_target_pos(target);
		  actuator_master_tx_update();
//		  cia402_master_set_controlword(cw);
//		  cia402_master_set_target_pos(target);
//		  cia402_master_pdo_flush_rpdo1();
//		  uint16_t sw = cia402_master_get_statusword();
//		  int32_t  pa = cia402_master_get_pos_actual();
		  uint16_t sw = actuator_master_get_statusword();
		  int32_t pa = actuator_master_get_pos_actual();
		  (void)sw;
		  (void)pa;
		}
//		static bool sdo_sent = false;
//		if (!sdo_sent && ms >= 1500u)
//		{
//		  sdo_sent = true;
//		  (void)canopen_space_sdo_write_u32(0 , 2, 0x607A, 0x00, 9000);
//		}
		#endif

		#if defined(ACTUATOR)
		  cia402_actuator_pdo_pull_rpdo1_from_od(ms);
//		  int32_t t = cia402_actuator_get_target_pos();
		  int32_t target = actuator_slave_get_target_pos();
		  uint16_t pwm_us = (target == 0) ? SERVO_US_MIN : SERVO_US_MAX;
		  servo_write_us(pwm_us);
		  actuator_slave_set_pos_actual(target);
		  actuator_slave_set_statusword(0x1237);

		  actuator_slave_tx_update();
//		  cia402_actuator_set_pos_actual(t);
//
//		  cia402_actuator_set_statusword(0x1237);
//		  cia402_actuator_pdo_push_tpdo1_to_od_and_send();
		#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
void HAL_SYSTICK_Callback(void)
{
//  lely_time_tick_1ms();
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
