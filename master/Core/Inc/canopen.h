#ifndef INC_CANOPEN_H_
#define INC_CANOPEN_H_

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

#include "od_master_cia402.h"


/* ── node / bus ──────────────────────────────────────────────────────────── */
#define CANOPEN_MASTER_NODE       0x01u
#define CANOPEN_REMOTE_NODE       0x02u
#define CANOPEN_BUS_A_ID          0x00u
#define CANOPEN_BUS_B_ID          0x01u
#define CANOPEN_BUS_A_TOKEN       0
#define CANOPEN_BUS_B_TOKEN       1

/* ── memory ──────────────────────────────────────────────────────────────── */
#define CANOPEN_MEMORY_POOL_SIZE  (90U * 1024U)

/* ── PDO / HB ────────────────────────────────────────────────────────────── */
#define PDO_NUM                   1u
#define HB_PRODUCER_MS            500u


#define CANOPEN_MAX_BUS_SWITCHES  4u
#define CANOPEN_SLAVE_BOOT_WAIT_S 2



typedef enum
{
	CANOPEN_OK,
	CANOPEN_ERROR,

}canopen_status;

typedef bool (*canopen_send_fn)(uint8_t bus_id, const struct can_msg *msg);
typedef bool (*canopen_recv_fn)(uint8_t bus_id, struct can_msg *msg);
typedef void (*canopen_time_fn)(struct timespec *tp);
typedef void (*canopen_app_sync_fn)   (uint8_t cnt);
typedef void (*canopen_app_running_fn)(void);
typedef void (*canopen_app_stopped_fn)(void);
typedef void (*canopen_app_rpdo_fn)   (uint16_t statusword, int32_t actual_pos);


typedef enum
{
	CANOPEN_BUS_RESET_NODE,
	CANOPEN_BUS_RESET_COMM,
	CANOPEN_BUS_PREOP,
	CANOPEN_BUS_OP,
	CANOPEN_BUS_STOPPED,

}canopen_bus_state;

typedef struct
{

}canopen_callbacks;



typedef struct {
    canopen_send_fn send;
    canopen_recv_fn recv;
    canopen_time_fn time;
    canopen_app_sync_fn    on_sync;
    canopen_app_running_fn on_running;
    canopen_app_stopped_fn on_stopped;
    canopen_app_rpdo_fn    on_rpdo;
} canopen_config;

typedef struct
{
	co_dev_t  *dev;
	co_nmt_t  *nmt;
	can_net_t *net;
	co_csdo_t         *csdo;
	canopen_bus_state bus_state;
	uint8_t bus_id;
    uint8_t            bus_switch_count;
    bool               slave_boot_pending;
    bool               slave_hb_timeout;
    struct timespec    slave_boot_time;
    canopen_config     cfg;

}canopen_t;

extern canopen_t canopen;

canopen_status canopen_init(canopen_config conf);
void canopen_process(void);
//void canopen_on_running(void);
//void canopen_on_stopped(void);
//void canopen_on_sync(uint8_t cnt);
void canopen_send_tpdo(void);
#endif /* INC_CANOPEN_H_ */
