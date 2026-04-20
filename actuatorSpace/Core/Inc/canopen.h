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

#include "od_slave_cia402.h"


/* ── node / bus ──────────────────────────────────────────────────────────── */
#define CANOPEN_MASTER_NODE       0x01u
#define CANOPEN_BUS_A_ID          0x00u
#define CANOPEN_BUS_B_ID          0x01u
#define CANOPEN_BUS_A_TOKEN       0
#define CANOPEN_BUS_B_TOKEN       1

/* ── memory ──────────────────────────────────────────────────────────────── */
#define CANOPEN_MEMORY_POOL_SIZE  (90U * 1024U)

/* ── PDO / HB ────────────────────────────────────────────────────────────── */
#define PDO_NUM                   1u
#define HB_PRODUCER_MS            500u

/* ── ECSS (OD 0x2000) ────────────────────────────────────────────────────── */
#define OD_IDX_RDN                0x2000u
#define OD_RDN_BDEFAULT           0x01u
#define OD_RDN_TTOGGLE            0x02u
#define OD_RDN_NTOGGLE            0x03u
#define ECSS_TTOGGLE              30u
#define ECSS_NTOGGLE              2u


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
typedef void (*canopen_app_tpdo_fn)   (void);

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
    /* application layer */
    canopen_app_sync_fn    on_sync;
    canopen_app_running_fn on_running;
    canopen_app_stopped_fn on_stopped;
    canopen_app_tpdo_fn    on_tpdo;
} canopen_config;

typedef struct
{
	co_dev_t  *dev;
	co_nmt_t  *nmt;
	can_net_t *net;
	canopen_bus_state bus_state;
	uint8_t bus_id;
    bool              reset_comm_pending;
    bool              reset_comm_is_recovery;
    uint8_t           saved_bus_id;
    canopen_config    cfg;

}canopen_t;

extern canopen_t canopen;

canopen_status canopen_init(canopen_config conf);
void canopen_process(void);


#endif /* INC_CANOPEN_H_ */
