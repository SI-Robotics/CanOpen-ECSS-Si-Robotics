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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <lely/config.h>
//#include <signal.h>
#include <stdio.h>
//#include <time.h>
#include <lely/can/msg.h>
#include <lely/can/net.h>
#include <lely/co/dev.h>
#include <lely/co/obj.h>
#include <lely/co/nmt.h>
#include <lely/util/error.h>
#include <lely/util/memory.h>
#include <lely/util/mempool.h>
#include <string.h>
#include <lely/util/error.h>
//#include <lely/co/nmt_rdn.h>


#if defined(LELY_NO_CO_NMT_HB)
#warning "LELY_NO_CO_NMT_HB is defined"
#endif
#if defined(LELY_NO_CO_HB)
#warning "LELY_NO_CO_HB is defined"
#endif
#if defined(LELY_NO_CO_NMT)
#warning "LELY_NO_CO_NMT is defined"
#endif

#define PDO

//#define ACTUATOR
#define MASTER

#ifdef ACTUATOR
#ifdef PDO
#include <lely/co/rpdo.h>
#include <lely/co/tpdo.h>
#include "od_slave_pdo.h"
#else
#include "od_slave.h"   // wygenerowane przez dcf2dev

#endif
#else
#include "od_master_pdo.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef ACTUATOR
static inline co_dev_t * app_od_init(void){ return od_slave_init();}
#define NODE_ID 2U
#else
static inline co_dev_t * app_od_init(void){ return od_master_init();}
#define NODE_ID 1U
#endif
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//#define NODE_ID          2u              // <-- w projekcie node3 ustaw 3
#define BUS_A   0u
#define BUS_B   1u
#define MEMPOOL_SIZE     (80u * 1024u)   // jak zabraknie, daj 48k/64k

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

static struct mempool g_pool;
static uint8_t g_pool_mem[MEMPOOL_SIZE] __attribute__((aligned(16)));

static can_net_t *g_net = NULL;
static co_dev_t  *g_dev = NULL;
static co_nmt_t  *g_nmt = NULL;
#if defined(ACTUATOR)
static co_rpdo_t *g_rpdo1 = NULL;
static co_tpdo_t *g_tpdo1 = NULL;
#endif
static struct timespec g_ts = {0};

//volatile int g_fail_bus_a = 0;
//volatile int g_force_bus  = -1;  // -1=normal, 0=BUS_A, 1=BUS_B (test)
//volatile uint32_t g_last_bus = 0;
//volatile uint32_t g_last_id  = 0;

// debug
static volatile uint32_t g_last_rx_id  = 0;
static volatile uint32_t g_last_tx_id  = 0;
static volatile uint32_t g_last_bus    = 0;
static volatile uint32_t g_fail_code   = 0;
//static volatile uint32_t g_fail_code = 0;
static volatile uint_least8_t g_active_bus = BUS_A;
//static volatile uint_least8_t g_active_bus = BUS_A;
//static volatile co_nmt_ecss_rdn_reason_t g_rdn_reason = 0;

static can_timer_t* ttest = NULL;
static volatile struct timespec g_deadline;
static volatile int g_deadline_valid = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static int my_can_next_func(const struct timespec* tp, void* data)
{
    (void)data;
    if (!tp) { g_deadline_valid = 0; return 0; }   // brak deadline
    g_deadline = *tp;
    g_deadline_valid = 1;
//    __BKPT(0);
    return 0;
}



static int ttest_cb(const struct timespec* tp, void* data){
  (void)tp; (void)data;
//  __BKPT(0);                 // będzie walić co 100ms jak timery działają
  return 0;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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

static void die(uint32_t code) {
  g_fail_code = code;
  volatile int errc = get_errc();  // <<<<< KLUCZ: zobacz w debuggerze
  (void)errc;
  __BKPT(0);
  __disable_irq();
  while (1) {}
}

static void assert_obj(co_dev_t* dev, uint16_t idx) {
  co_obj_t* o = co_dev_find_obj(dev, idx);
  if (!o) {
	  __BKPT(0);
    Error_Handler();
  }
}

#define DIE(code) do { die(code); } while(0)
//////////////////////////////////////////////////////////////////////////

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

static FDCAN_HandleTypeDef* bus_to_fdcan(uint_least8_t bus_id)
{
  if (bus_id == BUS_A) return &hfdcan1;
  if (bus_id == BUS_B) return &hfdcan2;
  return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////



static int lely_can_send(const struct can_msg *msg, uint_least8_t bus_id, void *data)
{
  (void)data;

  g_last_bus  = bus_id;
  g_last_tx_id = msg->id;

  // jeśli chcesz: wymuś zawsze aktywny bus
  // bus_id = g_active_bus;

  FDCAN_HandleTypeDef *h = bus_to_fdcan(bus_id);
  if (!h) {
    g_fail_code = 0xE002; // bad bus_id
    __BKPT(0);
    return -1;
  }
  FDCAN_TxHeaderTypeDef txh;
  memset(&txh, 0, sizeof(txh));
#if defined(ACTUATOR)
  if(msg->id == 0x182)
  {
//	  __BKPT(0);
  }
  if(msg->id == 0x702)
  {
	  __BKPT(0);
  }
#endif
  const bool is_ext = (msg->flags & CAN_FLAG_IDE) != 0;
  const bool is_rtr = (msg->flags & CAN_FLAG_RTR) != 0;

  txh.Identifier = msg->id;
  txh.IdType     = is_ext ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  txh.TxFrameType= is_rtr ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;
  txh.DataLength = len_to_dlc((uint8_t)((msg->len > 8) ? 8 : msg->len));
  txh.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txh.BitRateSwitch       = FDCAN_BRS_OFF;
  txh.FDFormat            = FDCAN_CLASSIC_CAN;
  txh.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;

  uint8_t payload[8] = {0};
  const uint8_t n = (msg->len > 8) ? 8 : (uint8_t)msg->len;
  if (!is_rtr && n) memcpy(payload, msg->data, n);
  if (HAL_FDCAN_AddMessageToTxFifoQ(h, &txh, payload) != HAL_OK)
  {
	    g_fail_code = 0xE001;                // TX fail during NMT create
	    g_last_bus  = bus_id;
	    g_last_tx_id = msg->id;
	    __BKPT(0);
	    return -1;
  }


  return 0;
}

static inline uint32_t u32_le(const uint8_t *d)
{
  return (uint32_t)d[0]
      | ((uint32_t)d[1] << 8)
      | ((uint32_t)d[2] << 16)
      | ((uint32_t)d[3] << 24);
}

static void lely_time_tick_1ms(void)
{
  g_ts.tv_nsec += 1000000L;
  if (g_ts.tv_nsec >= 1000000000L) {
    g_ts.tv_nsec -= 1000000000L;
    g_ts.tv_sec += 1;
  }
  if (g_net) (void)can_net_set_time(g_net, &g_ts);
}

static void on_ecss_rdn(co_nmt_t *nmt, uint_least8_t bus_id,co_nmt_ecss_rdn_reason_t reason, void *data)
{
  (void)nmt; (void)reason; (void)data;
  g_active_bus = bus_id;
  __BKPT(0);
}



static void lely_can_rx_push(uint_least8_t bus_id, uint32_t id, bool ext, bool rtr, uint8_t *data, uint8_t len)
{
  if (!g_net) return;

//#if defined(ACTUATOR)
//  // RPDO1 dla node 2: COB-ID 0x200 + 2 = 0x202
//  if (!ext && !rtr && id == (0x200u + NODE_ID) && len >= 4) {
//    uint32_t cmd = u32_le(data);
//    // zapis do OD: 0x2001:00 (RPDO1_command_u32)
//    (void)co_dev_set_val(g_dev, 0x2001, 0x00, &cmd, sizeof(cmd));
//    // debug: breakpoint / LED
//    // __BKPT(0);
//  }
//#elif defined(MASTER)
//  if (!ext && !rtr && id == (0x180u + 2u) && len >= 4) {
//    uint32_t tp = u32_le(data);
//    // tu np. breakpoint:
//    // __BKPT(0);
//    (void)tp;
//  }
//#endif
//  if (!ext && id == 0x702)
//  {
////    __BKPT(0); // heartbeat slave node 2
//  }
//  if (!ext && id == 0x582)
//  { // odpowiedź SDO od node 2
////    __BKPT(0);
//  }


  g_last_rx_id = id;
  struct can_msg m = CAN_MSG_INIT;
  m.id = id;
  if (ext) m.flags |= CAN_FLAG_IDE;
  if (rtr) m.flags |= CAN_FLAG_RTR;

  if (len > 8) len = 8;
  m.len = len;
  if (len && data) memcpy(m.data, data, len);

  g_last_rx_id = id;
  if (!ext && (id == 0x000u || id == (0x700u + NODE_ID))) {
    __BKPT(0);
  }
#if defined(MASTER)
  // TPDO1 od slave NODE_ID=2 ma COB-ID 0x180 + 2 = 0x182
  if (!ext && !rtr && id == (0x180u + 2u) && len >= 4) {
    volatile uint32_t tpdo_u32 = u32_le(data);   // podgląd w debuggerze
    g_last_rx_id = id;                           // już masz, ale nie szkodzi
//    __BKPT(0);                                   // <<< tu Ci zatrzyma jak przyszło TPDO
    (void)tpdo_u32;
  }
#endif
  (void)can_net_recv(g_net, &m, bus_id);
	#if defined(MASTER)
	  // podgląd TPDO1 od node2: 0x182
	  if (!ext && !rtr && id == (0x180u + 2u) && len >= 4) {
		volatile uint32_t tp = u32_le(data);
		(void)tp;
		// __BKPT(0);
	  }
	#endif
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t it)
{
  if ((it & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U) return;

  uint_least8_t bus_id;
  if (hfdcan == &hfdcan1) bus_id = BUS_A;
  else if (hfdcan == &hfdcan2) bus_id = BUS_B;
  else return;

  FDCAN_RxHeaderTypeDef rxh;
  uint8_t payload[8];

  while (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxh, payload) == HAL_OK) {
    const bool ext = (rxh.IdType == FDCAN_EXTENDED_ID);
    const bool rtr = (rxh.RxFrameType == FDCAN_REMOTE_FRAME);
    const uint8_t len = dlc_to_len(rxh.DataLength);
    if (rxh.Identifier == 0x702 && rxh.IdType == FDCAN_STANDARD_ID) {
//      __BKPT(0); // heartbeat od slave node 2
    }
    lely_can_rx_push(bus_id, rxh.Identifier, ext, rtr, payload, len);
  }
}

static void send_nmt(uint8_t cs, uint8_t node_id, uint_least8_t bus_id)
{
  // NMT: COB-ID 0x000, 2 bajty: [cs, node]
  struct can_msg m = CAN_MSG_INIT;
  m.id  = 0x000;
  m.len = 2;
  m.data[0] = cs;        // 0x01=start, 0x02=stop, 0x80=pre-op, 0x81=reset node, 0x82=reset comm
  m.data[1] = node_id;   // 0=all nodes, albo konkretny node

  (void)lely_can_send(&m, bus_id, NULL);
}

static void send_rpdo1_u32(uint8_t node_id, uint32_t val, uint_least8_t bus_id){
  struct can_msg m = CAN_MSG_INIT;
  m.id  = 0x200u + node_id;
  m.len = 4;
  m.data[0] = (uint8_t)(val & 0xFF);
  m.data[1] = (uint8_t)((val >> 8) & 0xFF);
  m.data[2] = (uint8_t)((val >> 16) & 0xFF);
  m.data[3] = (uint8_t)((val >> 24) & 0xFF);
  (void)lely_can_send(&m, bus_id, NULL);
}


#if defined(ACTUATOR)
static void pdo_config_slave_runtime(void)
{
  // TPDO1 -> 0x2000:00 (U32), COB-ID 0x180+NODE
  uint32_t tpdo_cobid     = 0x180u + NODE_ID;        // 0x182
  uint32_t tpdo_cobid_dis = tpdo_cobid | 0x80000000u;
  uint8_t  tt_event = 255; // event-driven
  uint16_t inhibit = 0;
  uint16_t event_ms = 0;

  (void)co_dev_set_val(g_dev, 0x1800, 0x01, &tpdo_cobid_dis, sizeof(tpdo_cobid_dis));
  uint8_t n0 = 0;
  (void)co_dev_set_val(g_dev, 0x1A00, 0x00, &n0, sizeof(n0));

  uint32_t map_t = 0x21000020u; // idx=0x2000 sub=0 len=32
  (void)co_dev_set_val(g_dev, 0x1A00, 0x01, &map_t, sizeof(map_t));
  uint8_t n1 = 1;
  (void)co_dev_set_val(g_dev, 0x1A00, 0x00, &n1, sizeof(n1));

  (void)co_dev_set_val(g_dev, 0x1800, 0x02, &tt_event, sizeof(tt_event));
  (void)co_dev_set_val(g_dev, 0x1800, 0x03, &inhibit, sizeof(inhibit));
  (void)co_dev_set_val(g_dev, 0x1800, 0x05, &event_ms, sizeof(event_ms));

  (void)co_dev_set_val(g_dev, 0x1800, 0x01, &tpdo_cobid, sizeof(tpdo_cobid));

  // RPDO1 <- 0x2001:00 (U32), COB-ID 0x200+NODE
  uint32_t rpdo_cobid     = 0x200u + NODE_ID;        // 0x202
  uint32_t rpdo_cobid_dis = rpdo_cobid | 0x80000000u;

  (void)co_dev_set_val(g_dev, 0x1400, 0x01, &rpdo_cobid_dis, sizeof(rpdo_cobid_dis));
  (void)co_dev_set_val(g_dev, 0x1600, 0x00, &n0, sizeof(n0));

  uint32_t map_r = 0x21010020u; // idx=0x2001 sub=0 len=32
  (void)co_dev_set_val(g_dev, 0x1600, 0x01, &map_r, sizeof(map_r));
  (void)co_dev_set_val(g_dev, 0x1600, 0x00, &n1, sizeof(n1));

  (void)co_dev_set_val(g_dev, 0x1400, 0x01, &rpdo_cobid, sizeof(rpdo_cobid));
}
#endif



#if defined(MASTER)
static void send_sdo_write_u32(uint8_t node, uint16_t idx, uint8_t sub, uint32_t val)
{
  struct can_msg m = CAN_MSG_INIT;
  m.id  = 0x600u + node;
  m.len = 8;
  m.data[0] = 0x23;
  m.data[1] = (uint8_t)(idx & 0xFF);
  m.data[2] = (uint8_t)(idx >> 8);
  m.data[3] = sub;
  m.data[4] = (uint8_t)(val & 0xFF);
  m.data[5] = (uint8_t)((val >> 8) & 0xFF);
  m.data[6] = (uint8_t)((val >> 16) & 0xFF);
  m.data[7] = (uint8_t)((val >> 24) & 0xFF);

  (void)lely_can_send(&m, g_active_bus, NULL);
}
#endif






static void CANopen_Start(void)
{
	  alloc_t *alloc = mempool_init(&g_pool, g_pool_mem, sizeof(g_pool_mem));
	  if (!alloc) DIE(1);

	  g_net = can_net_create(alloc, BUS_A);
	  if (!g_net) DIE(2);


	  can_net_set_send_func(g_net, &lely_can_send, NULL);
	  can_net_set_next_func(g_net, &my_can_next_func, NULL);
	  g_ts.tv_sec = 0;
	  g_ts.tv_nsec = 1000000L;   // 1ms start
	  (void)can_net_set_time(g_net, &g_ts);

	  ttest = can_timer_create(alloc);
	  if (!ttest) DIE(90);
	  can_timer_set_func(ttest, ttest_cb, NULL);

	  struct timespec interval = { .tv_sec = 0, .tv_nsec = 100000000L }; // 100 ms
	  can_timer_start(ttest, g_net, NULL, &interval);

	  g_dev = app_od_init();
	  if (!g_dev) DIE(3);

	  int ret2 = co_dev_set_id(g_dev, (co_unsigned8_t)NODE_ID);
	  (void)ret2;
	  // --- ECSS RDN: napraw minimalne suby zanim zrobisz co_nmt_create() ---

//	  {
//	    co_unsigned8_t sub0 = 4;
//	    co_unsigned8_t bdefault = BUS_A;
//
//	    (void)co_dev_set_val(g_dev, CO_NMT_RDN_REDUNDANCY_OBJ_IDX, 0x00, &sub0, sizeof(sub0));
//	    (void)co_dev_set_val(g_dev, CO_NMT_RDN_REDUNDANCY_OBJ_IDX, 0x01, &bdefault, sizeof(bdefault));
//	  }
#if defined(ACTUATOR)
  const co_unsigned16_t hb_ms = 100;
  if (co_dev_set_val(g_dev, 0x1017, 0x00, &hb_ms, sizeof(hb_ms)) != sizeof(hb_ms))
    DIE(34);

#endif
//#if defined(MASTER)
//  co_unsigned32_t vid = 0;
//  if (co_dev_get_val_u32(g_dev, 0x1018, 0x01, &vid) != 0) { // jeśli nie masz co_dev_get_val_u32 -> patrz niżej
//    DIE(35);
//  }
//#endif


//	  assert_obj(g_dev, 0x1000);
//	  assert_obj(g_dev, 0x1001);
//	  assert_obj(g_dev, 0x1017);
//	  assert_obj(g_dev, 0x1F80);
//	  assert_obj(g_dev, 0x1200);
	  int err = get_errc();
	  (void)err;
	  set_errc(0);
	  g_nmt = co_nmt_create(g_net, g_dev);
	  if (!g_nmt) DIE(6);
	  const void* p = co_dev_get_val(g_dev, 0x1017, 0x00);
	  volatile uint16_t hb = p ? *(const uint16_t*)p : 0xFFFF;
	  (void)hb; // podejrzyj w debuggerze

//	  co_nmt_set_ecss_rdn_ind(g_nmt, &on_ecss_rdn, NULL);
//	  (void)co_nmt_set_alternate_bus_id(g_nmt, (co_unsigned8_t)BUS_B);
#if defined(ACTUATOR)
	  pdo_config_slave_runtime(); // <-- KLUCZ
  g_rpdo1 = co_rpdo_create(g_net, g_dev, 1);
  if (!g_rpdo1) DIE(61);

  g_tpdo1 = co_tpdo_create(g_net, g_dev, 1);
  if (!g_tpdo1) DIE(62);

  co_rpdo_start(g_rpdo1);
  co_tpdo_start(g_tpdo1);

  // BOOT-UP: 0x700+ID, data=0

  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_RESET_COMM);

  // NIE startuj tutaj - master wysyła NMT START ALL
#endif
#if defined(MASTER)
  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_RESET_COMM);
//  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_START);
#endif

//	  co_nmt_set_ecss_rdn_ind(g_nmt, &on_ecss_rdn, NULL);
//
//	  (void)co_nmt_set_alternate_bus_id(g_nmt, (co_unsigned8_t)BUS_B);
//
//		#if defined(ACTUATOR)
//		  // SLAVE: wymuś BOOT-UP (0x700+NODE_ID, 0x00) i PRE-OP po starcie stosu
//		  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_RESET_NODE);
//		  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_START);
//		#endif
//#if defined(MASTER)
//	  (void)co_nmt_cs_ind(g_nmt, CO_NMT_CS_START);
//#endif
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
  /* USER CODE BEGIN 2 */

  FDCAN_AcceptAll(&hfdcan1);
  FDCAN_AcceptAll(&hfdcan2);


  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) DIE(200);
  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) DIE(201);

  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) DIE(202);
  if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) DIE(203);


  CANopen_Start();
  uint32_t ms = 0;
  bool started = false;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    HAL_Delay(1);
	    ms++;
	    lely_time_tick_1ms();

		#if defined(MASTER)
		  if (!started && ms >= 1000) {
			started = true;
			send_nmt(0x01, 0x00, BUS_A); // START ALL
		  }

		  if (started && (ms % 10) == 0) {
			static uint32_t v = 0;
			v++;
			send_rpdo1_u32(2, v, BUS_A); // RPDO1 do node2 => 0x202
		  }
		#endif
		#if defined(ACTUATOR)
//		    if (g_deadline_valid) {
//		      // jeśli aktualny czas >= deadline, to odpal timery
//		      if ((g_ts.tv_sec > g_deadline.tv_sec) ||
//		          (g_ts.tv_sec == g_deadline.tv_sec && g_ts.tv_nsec >= g_deadline.tv_nsec)) {
//		        g_deadline_valid = 0;
////		        can_net_process(g_net);   // albo can_net_step()/can_net_poll() zależnie od wersji
//		      }
//		    }



		  if ((ms % 100) == 0) {
			static uint32_t cnt = 0;
			cnt++;
			(void)co_dev_set_val(g_dev, 0x2100, 0x00, &cnt, sizeof(cnt));
			if (g_tpdo1) co_tpdo_event(g_tpdo1); // TPDO1 = 0x182
		  }
//		  can_net_create(alloc, bus_id)
		  // sprawdź czy RPDO1 doszło -> Lely zapisze 0x2001:00
		  static uint32_t last = 0;
		  const void *p = co_dev_get_val(g_dev, 0x2101, 0x00);
		  if (p) {
			uint32_t v = *(const uint32_t*)p;
			if (v != last) {
			  last = v;
//			   __BKPT(0);
			}
		  }
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
