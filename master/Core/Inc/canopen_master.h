/**
 * @file canopen_master.h
 * @brief CANopen master layer – initialisation, processing and configuration.
 *
 * Wraps the Lely CANopen stack (can_net, co_nmt, co_csdo, PDO objects) and
 * exposes a simple application interface through @ref canopen_config and the
 * @ref canopen_init / @ref canopen_process functions.
 *
 * Supports dual-bus redundancy (Bus A / Bus B) with automatic switchover on
 * Heartbeat timeout.
 *
 * @author
 * @date
 */

#ifndef INC_CANOPEN_MASTER_H_
#define INC_CANOPEN_MASTER_H_

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

/** @defgroup canopen_node Node and Bus IDs
 *  @brief Node and bus identifier constants.
 *  @{
 */
#define CANOPEN_MASTER_NODE       0x01u /**< CANopen Node-ID of this master. */
#define CANOPEN_REMOTE_NODE       0x02u /**< CANopen Node-ID of the monitored slave. */
#define CANOPEN_BUS_A_ID          0x00u /**< Bus index for the primary CAN bus (A). */
#define CANOPEN_BUS_B_ID          0x01u /**< Bus index for the redundant CAN bus (B). */
#define CANOPEN_BUS_A_TOKEN       0     /**< Token passed to the HAL layer for bus A. */
#define CANOPEN_BUS_B_TOKEN       1     /**< Token passed to the HAL layer for bus B. */
/** @} */

/** @defgroup canopen_mem Memory Pool
 *  @brief Static memory allocation parameters for the CANopen stack.
 *  @{
 */
#define CANOPEN_MEMORY_POOL_SIZE  (90U * 1024U)/**< Total memory pool size [bytes]. */
/** @} */

/** @defgroup canopen_pdo PDO and Heartbeat Parameters
 *  @brief PDO channel number and Heartbeat producer period.
 *  @{
 */
#define PDO_NUM                   1u    /**< PDO channel used for RPDO/TPDO (channel 1). */
#define HB_PRODUCER_MS            500u/**< Master Heartbeat producer period [ms]. */
/** @} */

/** @defgroup canopen_resilience Redundancy and Boot Parameters
 *  @brief Constants governing bus switchover and slave boot behaviour.
 *  @{
 */
#define CANOPEN_MAX_BUS_SWITCHES  4u /**< Maximum A<->B switchovers before a fatal error is raised. */
#define CANOPEN_SLAVE_BOOT_WAIT_S 2  /**< Time to wait for slave BOOTUP after NMT START [s]. */
/** @} */


/**
 * @brief Return code for CANopen API functions.
 */
typedef enum
{
	CANOPEN_OK,/**< Operation completed successfully. */
	CANOPEN_ERROR,/**< Operation failed; see UART log for details. */

}canopen_status;
/**
 * @brief Hardware abstraction: transmit a CAN frame.
 * @param[in] bus_id  Bus index (0 = A, 1 = B).
 * @param[in] msg     Pointer to the CAN frame to transmit.
 * @return @c true on success, @c false if the frame could not be sent.
 */
typedef bool (*canopen_send_fn)(uint8_t bus_id, const struct can_msg *msg);

/**
 * @brief Hardware abstraction: receive a pending CAN frame.
 * @param[in]  bus_id  Bus index (0 = A, 1 = B).
 * @param[out] msg     Buffer to write the received frame into.
 * @return @c true if a frame was available and written to @p msg.
 */
typedef bool (*canopen_recv_fn)(uint8_t bus_id, struct can_msg *msg);
/**
 * @brief Hardware abstraction: read the current monotonic time.
 * @param[out] tp  Pointer to a @c timespec structure to fill.
 */
typedef void (*canopen_time_fn)(struct timespec *tp);
/**
 * @brief Application callback invoked on every SYNC event.
 * @param[in] cnt  SYNC counter value (0-240) forwarded by the CANopen stack.
 */
typedef void (*canopen_app_sync_fn)   (uint8_t cnt);
/**
 * @brief Application callback invoked when the slave enters OPERATIONAL.
 */
typedef void (*canopen_app_running_fn)(void);
/**
 * @brief Application callback invoked when the slave leaves OPERATIONAL.
 *
 * Triggered by: Heartbeat timeout, slave BOOTUP, or NMT STOPPED.
 */
typedef void (*canopen_app_stopped_fn)(void);
/**
 * @brief Application callback invoked after every received RPDO.
 * @param[in] statusword  Drive Statusword extracted from the RPDO (OD 0x6041).
 * @param[in] actual_pos  Actual position extracted from the RPDO (OD 0x6064) [pulses].
 */
typedef void (*canopen_app_rpdo_fn)   (uint16_t statusword, int32_t actual_pos);

/**
 * @brief CAN bus / NMT operational state tracked by the master.
 */
typedef enum
{
	CANOPEN_BUS_RESET_NODE,	/**< Slave is performing a node reset. */
	CANOPEN_BUS_RESET_COMM, /**< Slave is performing a communication reset. */
	CANOPEN_BUS_PREOP,		/**< Slave is in the Pre-Operational state. */
	CANOPEN_BUS_OP, 		/**< Slave is in the Operational state. */
	CANOPEN_BUS_STOPPED, 	/**< Bus halted due to a fatal error. */

}canopen_bus_state;

typedef struct
{

}canopen_callbacks;


/**
 * @brief Configuration structure passed to @ref canopen_init().
 *
 * All function pointers must be non-NULL before calling @ref canopen_init().
 */
typedef struct {
    canopen_send_fn send;					/**< HAL function to transmit a CAN frame. */
    canopen_recv_fn recv;    				/**< HAL function to receive a pending CAN frame. */
    canopen_time_fn time;				 	/**< HAL function to read the monotonic clock. */
    canopen_app_sync_fn    on_sync; 		/**< App callback: SYNC frame processed. */
    canopen_app_running_fn on_running;		/**< App callback: slave entered OPERATIONAL. */
    canopen_app_stopped_fn on_stopped;		/**< App callback: slave left OPERATIONAL. */
    canopen_app_rpdo_fn    on_rpdo;			/**< App callback: RPDO received from slave. */
} canopen_config;


/**
 * @brief Runtime state of the CANopen master layer.
 *
 * The single global instance is @ref canopen.
 * Populated by @ref canopen_init() and updated continuously by
 * @ref canopen_process() and the internal stack callbacks.
 */
typedef struct
{
	co_dev_t  *dev; 					 /**< CANopen device object (master Object Dictionary). */
	co_nmt_t  *nmt;						 /**< NMT master object. */
	can_net_t *net;						 /**< Lely CAN network layer instance. */
	co_csdo_t         *csdo;			 /**< Client SDO object used for SDO download requests. */
	canopen_bus_state bus_state;		 /**< Current bus / NMT operational state. */
	uint8_t bus_id;   /**< Active bus index (0 = A, 1 = B). */
    uint8_t            bus_switch_count;/**< Number of A<->B switchovers since last HB resolved. */
    bool               slave_boot_pending;/**< @c true while waiting for the slave's first BOOTUP. */
    bool               slave_hb_timeout;/**< @c true when the HB consumer detects a timeout;
                                               processed and cleared inside @ref canopen_process(). */
    struct timespec    slave_boot_time;/**< Monotonic timestamp of the last slave BOOTUP frame. */
    canopen_config     cfg;  /**< Application-supplied configuration (HAL + callbacks). */

}canopen_t;
/**
 * @brief Global CANopen master state instance.
 * @note  Initialised by @ref canopen_init(). Treat internal fields as read-only
 *        from outside this module.
 */
extern canopen_t canopen;
/**
 * @brief Initialises the CANopen master stack.
 *
 * Allocates and configures: memory pool, can_net, Object Dictionary, NMT,
 * CSDO, RPDO and TPDO objects. Registers all internal stack callbacks.
 * Ends by issuing NMT RESET_NODE followed by NMT START, transitioning
 * the master to @c CANOPEN_BUS_OP.
 *
 * @param[in] conf  Layer configuration (HAL functions + application callbacks).
 * @return @ref CANOPEN_OK on success, @ref CANOPEN_ERROR on any allocation
 *         or stack initialisation failure.
 */
canopen_status canopen_init(canopen_config conf);
/**
 * @brief Processes all pending CANopen events – call from the main loop.
 *
 * Responsibilities (in order):
 * -# Updates the stack's internal clock via @c can_net_set_time().
 * -# Drains all pending CAN frames from bus A and bus B.
 * -# Handles a deferred bus switchover when @c slave_hb_timeout is set.
 * -# Runs per-state logic for the current @c bus_state.
 *
 * @note Not thread-safe; must be called from a single execution context.
 */
void canopen_process(void);
/**
 * @brief Forces an immediate transmission of the master TPDO.
 * @note  Reserved for application use – body to be implemented as needed.
 */
void canopen_send_tpdo(void);
#endif /* INC_CANOPEN_MASTER_H_ */
