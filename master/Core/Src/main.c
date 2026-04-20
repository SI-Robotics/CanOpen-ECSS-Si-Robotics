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
#include "canopen.h"
#include "cia402_master.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define NMT_RESET_COMM_TEST_ENABLE   0
#define NMT_RESET_COMM_TEST_MS       20000u

#define RPDO_FLOOD_TEST_ENABLE   0
#define RPDO_FLOOD_START_MS      20000u   /* kiedy startuje test */
#define RPDO_FLOOD_STEP_MS       5000u    /* co ile zwiększa częstotliwość */

#define SDO_FLOOD_TEST_ENABLE   0
#define SDO_FLOOD_START_MS      20000u
#define SDO_FLOOD_REPEAT        100u

#define SDO_READ_FLOOD_TEST_ENABLE   0
#define SDO_READ_FLOOD_START_MS      20000u
#define SDO_READ_FLOOD_REPEAT        100u

#define SDO_PDO_COMBINED_TEST_ENABLE   0
#define SDO_PDO_COMBINED_START_MS      20000u
#define SDO_PDO_COMBINED_REPEAT        50u
#define SDO_PDO_COMBINED_FREQ_HZ       500u

#define DISABLE_VOLTAGE_TEST_ENABLE   0
#define DISABLE_VOLTAGE_TEST_MS       20000u

#define PDO_BADLEN_TEST_ENABLE   0
#define PDO_BADLEN_TEST_MS       20000u
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


#if SDO_FLOOD_TEST_ENABLE

typedef struct {
    uint16_t idx;
    uint8_t  sub;
    uint32_t val;
    uint16_t deftype;
    bool     allowed;   /* false = celowo niedozwolony */
} sdo_flood_entry_t;

static const sdo_flood_entry_t sdo_flood_table[] = {
    {0x6040u, 0x00u, 0x0006u, CO_DEFTYPE_UNSIGNED16,  true},
    {0x607Au, 0x00u, 0x0000u, CO_DEFTYPE_INTEGER32,   true},
    {0x6060u, 0x00u, 0x0008u, CO_DEFTYPE_INTEGER8,    true},
    {0x1000u, 0x00u, 0xDEADu, CO_DEFTYPE_UNSIGNED32,  false},
    {0x1018u, 0x01u, 0xDEADu, CO_DEFTYPE_UNSIGNED32,  false},
    {0x9999u, 0x00u, 0xDEADu, CO_DEFTYPE_UNSIGNED32,  false},
};
#define SDO_FLOOD_COUNT (sizeof(sdo_flood_table)/sizeof(sdo_flood_table[0]))

static uint8_t  sdo_flood_idx      = 0u;
static uint32_t sdo_flood_ok       = 0u;
static uint32_t sdo_flood_abort    = 0u;
static uint32_t sdo_flood_timeout  = 0u;
static bool     sdo_flood_busy     = false;
static bool     sdo_flood_done     = false;
static uint32_t sdo_flood_round    = 0u;   /* ← nowe */
static uint32_t sdo_flood_t_start  = 0u;   /* ← nowe */

static void sdo_flood_cb(co_csdo_t *sdo, uint16_t idx, uint8_t sub,
                          uint32_t ac, void *data)
{
    (void)sdo; (void)data;
    const sdo_flood_entry_t *e = &sdo_flood_table[
        (sdo_flood_idx == 0u) ? 0u : sdo_flood_idx - 1u];

    if (ac == 0u)
    {
        sdo_flood_ok++;
    }
    else if (ac == CO_SDO_AC_TIMEOUT)
    {
        sdo_flood_timeout++;
        MAIN_LOGF("[SDO_FLOOD] TIMEOUT round=%lu idx=0x%04X",
                  sdo_flood_round, idx);
    }
    else
    {
        sdo_flood_abort++;
        if (e->allowed)
        {
            MAIN_LOGF("[SDO_FLOOD] UNEXPECTED ABORT round=%lu "
                      "idx=0x%04X ac=0x%08lX",
                      sdo_flood_round, idx, ac);
        }
    }
    sdo_flood_busy = false;
}
#endif

#if SDO_READ_FLOOD_TEST_ENABLE

static const uint16_t sdo_read_table[][2] = {
    {0x6041u, 0x00u},   /* statusword */
    {0x6064u, 0x00u},   /* actual position */
    {0x6060u, 0x00u},   /* modes of operation */
    {0x1000u, 0x00u},   /* device type */
    {0x1018u, 0x01u},   /* vendor ID */
    {0x9999u, 0x00u},   /* nieistniejący — abort oczekiwany */
};
#define SDO_READ_COUNT (sizeof(sdo_read_table)/sizeof(sdo_read_table[0]))

static uint8_t  sdo_read_idx     = 0u;
static uint32_t sdo_read_ok      = 0u;
static uint32_t sdo_read_abort   = 0u;
static uint32_t sdo_read_timeout = 0u;
static bool     sdo_read_busy    = false;
static bool     sdo_read_done    = false;
static uint32_t sdo_read_round   = 0u;
static uint32_t sdo_read_t_start = 0u;

static void sdo_read_cb(co_csdo_t *sdo, uint16_t idx, uint8_t sub,
                         uint32_t ac, const void *ptr, size_t n, void *data)
{
    (void)sdo; (void)ptr; (void)n; (void)data;

    if (ac == 0u)
    {
        sdo_read_ok++;
    }
    else if (ac == CO_SDO_AC_TIMEOUT)
    {
        sdo_read_timeout++;
        MAIN_LOGF("[SDO_READ] TIMEOUT round=%lu idx=0x%04X",
                  sdo_read_round, idx);
    }
    else
    {
        sdo_read_abort++;
        /* tylko nieoczekiwane — obiekt 0x9999 jest oczekiwany */
        if (idx != 0x9999u)
        {
            MAIN_LOGF("[SDO_READ] UNEXPECTED ABORT round=%lu "
                      "idx=0x%04X:0x%02X ac=0x%08lX",
                      sdo_read_round, idx, sub, ac);
        }
    }

    sdo_read_busy = false;
}
#endif
#if SDO_PDO_COMBINED_TEST_ENABLE

static uint8_t  comb_idx          = 0u;
static uint32_t comb_ok           = 0u;
static uint32_t comb_abort        = 0u;
static uint32_t comb_timeout      = 0u;
static bool     comb_busy         = false;
static bool     comb_done         = false;
static uint32_t comb_round        = 0u;
static uint32_t comb_t_start      = 0u;
static uint32_t comb_pdo_lost     = 0u;   /* zlicz utracone RPDO */
//static uint32_t comb_last_rpdo_n  = 0u;   /* ostatnie n z RPDO */
static uint32_t comb_last_pdo_check = 0u;   /* ← dodaj */
static uint32_t comb_rpdo_at_check  = 0u;   /* ← dodaj */
static const uint16_t comb_read_table[][2] = {
    {0x6041u, 0x00u},   /* statusword */
    {0x6064u, 0x00u},   /* actual position */
    {0x1000u, 0x00u},   /* device type */
};
#define COMB_COUNT (sizeof(comb_read_table)/sizeof(comb_read_table[0]))

static void comb_sdo_cb(co_csdo_t *sdo, uint16_t idx, uint8_t sub,
                         uint32_t ac, const void *ptr, size_t n, void *data)
{
    (void)sdo; (void)ptr; (void)n; (void)data; (void)sub;

    if (ac == 0u)
    {
        comb_ok++;
    }
    else if (ac == CO_SDO_AC_TIMEOUT)
    {
        comb_timeout++;
        MAIN_LOGF("[COMB] SDO TIMEOUT round=%lu idx=0x%04X",
                  comb_round, idx);
    }
    else
    {
        comb_abort++;
    }
    comb_busy = false;
}
#endif
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
#if RPDO_FLOOD_TEST_ENABLE
static const uint32_t flood_periods[] = {999u, 499u, 199u, 99u, 49u};
#define FLOOD_STEPS (sizeof(flood_periods)/sizeof(flood_periods[0]))
static uint8_t  flood_step      = 0u;
static uint32_t flood_last_tick = 0u;
#endif
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
#if PDO_BADLEN_TEST_ENABLE
{
    static uint8_t  plen_step = 0u;
    static bool     plen_done = false;
    static uint32_t plen_last = 0u;

    if (!plen_done && HAL_GetTick() >= PDO_BADLEN_TEST_MS &&
        cia402_master.state == CIA402_M_RUNNING)
    {
        if (HAL_GetTick() - plen_last >= 1000u)
        {
            plen_last = HAL_GetTick();

            struct can_msg msg = {0};
            msg.id = 0x200u + CANOPEN_REMOTE_NODE;  /* RPDO1 COB-ID */

            switch (plen_step)
            {
                case 0:
                    /* poprawna długość — baseline */
                    msg.len = 6u;
                    memset(msg.data, 0xAAu, 6u);
                    MAIN_LOGF("[PDO_LEN] t=%lums sending len=6 (correct)",
                              HAL_GetTick());
                    break;
                case 1:
                    /* za krótkie — 2 bajty zamiast 6 */
                    msg.len = 2u;
                    memset(msg.data, 0xBBu, 2u);
                    MAIN_LOGF("[PDO_LEN] t=%lums sending len=2 (too short)",
                              HAL_GetTick());
                    break;
                case 2:
                    /* za długie — 8 bajtów zamiast 6 */
                    msg.len = 8u;
                    memset(msg.data, 0xCCu, 8u);
                    MAIN_LOGF("[PDO_LEN] t=%lums sending len=8 (too long)",
                              HAL_GetTick());
                    break;
                case 3:
                    /* zerowa długość */
                    msg.len = 0u;
                    MAIN_LOGF("[PDO_LEN] t=%lums sending len=0 (empty)",
                              HAL_GetTick());
                    break;
                default:
                    plen_done = true;
                    MAIN_LOGF("[PDO_LEN] DONE cia402_state=%d SW=0x%04X",
                              (int)cia402_master.state,
                              cia402_master.statusword);
                    break;
            }

            if (!plen_done)
            {
                canopen.cfg.send(canopen.bus_id, &msg);
                plen_step++;
            }
        }
    }
}
#endif
#if DISABLE_VOLTAGE_TEST_ENABLE
{
    static bool     dv_sent      = false;
    static bool     dv_logged    = false;
    static uint32_t dv_sent_tick = 0u;

    if (!dv_sent && HAL_GetTick() >= DISABLE_VOLTAGE_TEST_MS &&
        cia402_master.state == CIA402_M_RUNNING)
    {
        dv_sent      = true;
        dv_sent_tick = HAL_GetTick();

        /* zmień CW w OD mastera — RPDO wyśle 0x0000 przy następnym SYNC */
        cia402_master_set_controlword(0x0000u);

        MAIN_LOGF("[DV_TEST] t=%lums Disable Voltage CW=0x0000 "
                  "actual_pos=%ld",
                  HAL_GetTick(),
                  (long)cia402_master.actual_pos);
    }

    if (dv_sent && !dv_logged &&
        HAL_GetTick() >= dv_sent_tick + 7000u)
    {
        dv_logged = true;
        MAIN_LOGF("[DV_TEST] t=%lums after 3s: SW=0x%04X "
                  "cia402_state=%d actual_pos=%ld",
                  HAL_GetTick(),
                  cia402_master.statusword,
                  (int)cia402_master.state,
                  (long)cia402_master.actual_pos);
    }
}
#endif
#if SDO_PDO_COMBINED_TEST_ENABLE
    if (!comb_done &&
        HAL_GetTick() >= SDO_PDO_COMBINED_START_MS)
    {
        /* pierwsze wejście — ustaw timer na żądaną częstotliwość */
        if (comb_t_start == 0u)
        {
            comb_t_start = HAL_GetTick();
            uint32_t period = (1000000u / SDO_PDO_COMBINED_FREQ_HZ) - 1u;
            __HAL_TIM_SET_AUTORELOAD(&htim6, period);
            MAIN_LOGF("[COMB] START freq=%uHz rounds=%u",
                      SDO_PDO_COMBINED_FREQ_HZ,
                      SDO_PDO_COMBINED_REPEAT);
        }

        /* SDO read równolegle z PDO */
        if (!comb_busy && comb_idx < COMB_COUNT &&
            cia402_master.state == CIA402_M_RUNNING)
        {
            comb_busy = true;
            co_csdo_up_req(canopen.csdo,
                           comb_read_table[comb_idx][0],
                           comb_read_table[comb_idx][1],
                           0,
                           comb_sdo_cb, NULL);
            comb_idx++;
        }

        if (comb_idx >= COMB_COUNT && !comb_busy)
        {
            comb_round++;

            if (comb_round % 10u == 0u)
            {
                uint32_t elapsed = HAL_GetTick() - comb_t_start;
                MAIN_LOGF("[COMB] round=%lu ok=%lu abort=%lu "
                          "timeout=%lu pdo_lost=%lu elapsed=%lums "
                          "cia402=%d",
                          comb_round, comb_ok, comb_abort,
                          comb_timeout, comb_pdo_lost,
                          elapsed,
                          (int)cia402_master.state);
            }

            if (comb_round >= SDO_PDO_COMBINED_REPEAT)
            {
                comb_done = true;
                /* przywróć normalny timer 1ms */
                __HAL_TIM_SET_AUTORELOAD(&htim6, 999u);
                uint32_t elapsed = HAL_GetTick() - comb_t_start;
                MAIN_LOGF("[COMB] DONE ok=%lu abort=%lu timeout=%lu "
                          "pdo_lost=%lu elapsed=%lums final_state=%d",
                          comb_ok, comb_abort, comb_timeout,
                          comb_pdo_lost, elapsed,
                          (int)cia402_master.state);
            }
            else
            {
                comb_idx = 0u;
            }
        }

        /* wykryj utratę RPDO — n powinno rosnąć co rundę */
//        static uint32_t comb_last_pdo_check = 0u;
//        static uint32_t comb_rpdo_at_check  = 0u;

        uint32_t now_c = HAL_GetTick();
        if (now_c - comb_last_pdo_check >= 50u)
        {
            uint32_t delta = cia402_master.rpdo_count - comb_rpdo_at_check;
            if (delta == 0u && cia402_master.rpdo_count > 0u)
            {
                comb_pdo_lost++;
//                MAIN_LOGF("[COMB] PDO LOST at t=%lums round=%lu",
//                          now_c, comb_round);
            }
            comb_rpdo_at_check   = cia402_master.rpdo_count;
            comb_last_pdo_check  = now_c;
        }
    }
#endif
#if SDO_READ_FLOOD_TEST_ENABLE
    if (!sdo_read_done &&
        HAL_GetTick() >= SDO_READ_FLOOD_START_MS &&
        cia402_master.state == CIA402_M_RUNNING)
    {
        if (sdo_read_round == 0u && sdo_read_idx == 0u &&
            sdo_read_t_start == 0u)
        {
            sdo_read_t_start = HAL_GetTick();
            MAIN_LOGF("[SDO_READ] START %lu rounds x %u entries",
                      (uint32_t)SDO_READ_FLOOD_REPEAT,
                      (uint8_t)SDO_READ_COUNT);
        }

        if (!sdo_read_busy && sdo_read_idx < SDO_READ_COUNT)
        {
            sdo_read_busy = true;
            co_csdo_up_req(canopen.csdo,
                           sdo_read_table[sdo_read_idx][0],
                           sdo_read_table[sdo_read_idx][1],
                           0,
                           sdo_read_cb, NULL);
            sdo_read_idx++;
        }

        if (sdo_read_idx >= SDO_READ_COUNT && !sdo_read_busy)
        {
            sdo_read_round++;

            if (sdo_read_round % 10u == 0u)
            {
                uint32_t elapsed = HAL_GetTick() - sdo_read_t_start;
                uint32_t rate = (elapsed > 0u)
                    ? (sdo_read_round * SDO_READ_COUNT * 1000u / elapsed)
                    : 0u;
                MAIN_LOGF("[SDO_READ] round=%lu ok=%lu abort=%lu "
                          "timeout=%lu rate=%lu SDO/s elapsed=%lums",
                          sdo_read_round, sdo_read_ok,
                          sdo_read_abort, sdo_read_timeout,
                          rate, elapsed);
            }

            if (sdo_read_round >= SDO_READ_FLOOD_REPEAT)
            {
                sdo_read_done = true;
                uint32_t elapsed = HAL_GetTick() - sdo_read_t_start;
                MAIN_LOGF("[SDO_READ] DONE rounds=%lu ok=%lu "
                          "abort=%lu timeout=%lu elapsed=%lums",
                          sdo_read_round, sdo_read_ok,
                          sdo_read_abort, sdo_read_timeout,
                          elapsed);
            }
            else
            {
                sdo_read_idx = 0u;
            }
        }
    }
#endif
#if SDO_FLOOD_TEST_ENABLE
    if (!sdo_flood_done &&
        HAL_GetTick() >= SDO_FLOOD_START_MS &&
        cia402_master.state == CIA402_M_RUNNING)
    {
        if (sdo_flood_round == 0u && sdo_flood_idx == 0u &&
            sdo_flood_t_start == 0u)
        {
            sdo_flood_t_start = HAL_GetTick();
            MAIN_LOGF("[SDO_FLOOD] START %lu rounds x %u entries",
                      (uint32_t)SDO_FLOOD_REPEAT,
                      (uint8_t)SDO_FLOOD_COUNT);
        }

        if (!sdo_flood_busy && sdo_flood_idx < SDO_FLOOD_COUNT)
        {
            const sdo_flood_entry_t *e = &sdo_flood_table[sdo_flood_idx];
            static uint32_t val;
            val = e->val;
            sdo_flood_busy = true;
            sdo_flood_idx++;

            co_csdo_dn_val_req(canopen.csdo,
                               e->idx, e->sub,
                               e->deftype,
                               &val,
                               0,
                               sdo_flood_cb, NULL);
        }

        if (sdo_flood_idx >= SDO_FLOOD_COUNT && !sdo_flood_busy)
        {
            sdo_flood_round++;

            if (sdo_flood_round % 10u == 0u)
            {
                uint32_t elapsed = HAL_GetTick() - sdo_flood_t_start;
                uint32_t rate = (elapsed > 0u)
                    ? (sdo_flood_round * SDO_FLOOD_COUNT * 1000u / elapsed)
                    : 0u;
                MAIN_LOGF("[SDO_FLOOD] round=%lu ok=%lu abort=%lu "
                          "timeout=%lu rate=%lu SDO/s elapsed=%lums",
                          sdo_flood_round, sdo_flood_ok,
                          sdo_flood_abort, sdo_flood_timeout,
                          rate, elapsed);
            }

            if (sdo_flood_round >= SDO_FLOOD_REPEAT)
            {
                sdo_flood_done = true;
                uint32_t elapsed = HAL_GetTick() - sdo_flood_t_start;
                MAIN_LOGF("[SDO_FLOOD] DONE rounds=%lu ok=%lu "
                          "abort=%lu timeout=%lu elapsed=%lums",
                          sdo_flood_round, sdo_flood_ok,
                          sdo_flood_abort, sdo_flood_timeout,
                          elapsed);
            }
            else
            {
                sdo_flood_idx = 0u;
            }
        }
    }
#endif
#if NMT_RESET_COMM_TEST_ENABLE
static bool reset_comm_sent = false;

if (!reset_comm_sent && HAL_GetTick() >= NMT_RESET_COMM_TEST_MS)
{
    reset_comm_sent = true;

    struct can_msg nmt_frame = {0};
    nmt_frame.id      = 0x000u;
    nmt_frame.len     = 2u;
    nmt_frame.data[0] = 0x82u;            /* RESET_COMM */
    nmt_frame.data[1] = 0x00u;            /* broadcast */

    bool ok = canopen.cfg.send(canopen.bus_id, &nmt_frame);
    MAIN_LOGF("[MASTER] t=%lums NMT RESET_COMM raw: %s",
             HAL_GetTick(), ok ? "OK" : "FAIL");
}
#endif
#if RPDO_FLOOD_TEST_ENABLE
    if (HAL_GetTick() >= RPDO_FLOOD_START_MS)
    {
        if (flood_step < FLOOD_STEPS &&
            HAL_GetTick() - flood_last_tick >= RPDO_FLOOD_STEP_MS)
        {
            flood_last_tick = HAL_GetTick();
            __HAL_TIM_SET_AUTORELOAD(&htim6, flood_periods[flood_step]);
            MAIN_LOGF("[FLOOD] t=%lums step=%u -> %luHz",
                      HAL_GetTick(), flood_step,
                      1000000u / (flood_periods[flood_step] + 1u));
            flood_step++;
        }
    }
#endif
#if !RPDO_FLOOD_TEST_ENABLE && !SDO_FLOOD_TEST_ENABLE && !SDO_READ_FLOOD_TEST_ENABLE && !SDO_PDO_COMBINED_TEST_ENABLE && !PDO_BADLEN_TEST_ENABLE
    HAL_Delay(1);
#endif


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
