/**
 * @file canopen.h
 * @brief CANopen slave layer – initialisation, processing and configuration.
 *
 * Wraps the Lely CANopen stack (can_net, co_nmt, PDO objects) for a slave
 * node and exposes a simple application interface through @ref canopen_config
 * and the @ref canopen_init / @ref canopen_process functions.
 *
 * Supports dual-bus redundancy (Bus A / Bus B) via the ECSS redundancy
 * object at OD index 0x2000.
 *
 * @author
 * @date
 */
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

/** @defgroup canopen_node Node and Bus IDs
 *  @brief Node and bus identifier constants.
 *  @{
 */
#define CANOPEN_MASTER_NODE   0x01u  /**< CANopen Node-ID of the master this slave communicates with. */
#define CANOPEN_BUS_A_ID      0x00u  /**< Bus index for the primary CAN bus (A). */
#define CANOPEN_BUS_B_ID      0x01u  /**< Bus index for the redundant CAN bus (B). */
#define CANOPEN_BUS_A_TOKEN   0      /**< Token passed to the HAL layer for bus A. */
#define CANOPEN_BUS_B_TOKEN   1      /**< Token passed to the HAL layer for bus B. */
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
#define PDO_NUM          1u    /**< PDO channel used for RPDO/TPDO (channel 1). */
#define HB_PRODUCER_MS   500u  /**< Slave Heartbeat producer period [ms]. */
/** @} */

/** @defgroup canopen_ecss ECSS Redundancy Parameters
 *  @brief OD indices and timing constants for the ECSS redundancy object (0x2000).
 *  @{
 */
#define OD_IDX_RDN      0x2000u  /**< OD index of the ECSS redundancy object. */
#define OD_RDN_BDEFAULT 0x01u    /**< Sub-index: default active bus (0 = A, 1 = B). */
#define OD_RDN_TTOGGLE  0x02u    /**< Sub-index: toggle timeout [s]. */
#define OD_RDN_NTOGGLE  0x03u    /**< Sub-index: maximum number of toggle attempts. */
#define ECSS_TTOGGLE    30u      /**< Default toggle timeout value written to OD [s]. */
#define ECSS_NTOGGLE    2u       /**< Default maximum toggle attempts written to OD. */
/** @} */

/**
 * @brief Return code for CANopen API functions.
 */
typedef enum
{
    CANOPEN_OK,     /**< Operation completed successfully. */
    CANOPEN_ERROR,  /**< Operation failed; see UART log for details. */
} canopen_status;

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
 * @param[in] cnt  SYNC counter value (0–240) forwarded by the CANopen stack.
 */
typedef void (*canopen_app_sync_fn)   (uint8_t cnt);
/**
 * @brief Application callback invoked when the slave enters OPERATIONAL.
 */
typedef void (*canopen_app_running_fn)(void);
/**
 * @brief Application callback invoked when the slave leaves OPERATIONAL.
 *
 * Triggered by: NMT STOP, communication reset, or bus error.
 */
typedef void (*canopen_app_stopped_fn)(void);

/**
 * @brief Application callback invoked after a TPDO has been transmitted.
 *
 * Can be used to prepare the next TPDO payload in the OD before the
 * following SYNC cycle.
 */
typedef void (*canopen_app_tpdo_fn)   (void);

/**
 * @brief CAN bus / NMT operational state tracked by the slave.
 */
typedef enum
{
    CANOPEN_BUS_RESET_NODE,  /**< Node reset in progress. */
    CANOPEN_BUS_RESET_COMM,  /**< Communication reset in progress. */
    CANOPEN_BUS_PREOP,       /**< Slave is in the Pre-Operational state. */
    CANOPEN_BUS_OP,          /**< Slave is in the Operational state. */
    CANOPEN_BUS_STOPPED,     /**< Slave is in the Stopped state. */
} canopen_bus_state;


/**
 * @brief Configuration structure passed to @ref canopen_init().
 *
 * All function pointers must be non-NULL before calling @ref canopen_init().
 */
typedef struct {
    canopen_send_fn    send;        /**< HAL function to transmit a CAN frame. */
    canopen_recv_fn    recv;        /**< HAL function to receive a pending CAN frame. */
    canopen_time_fn    time;        /**< HAL function to read the monotonic clock. */
    canopen_app_sync_fn    on_sync;     /**< App callback: SYNC frame received. */
    canopen_app_running_fn on_running;  /**< App callback: slave entered OPERATIONAL. */
    canopen_app_stopped_fn on_stopped;  /**< App callback: slave left OPERATIONAL. */
    canopen_app_tpdo_fn    on_tpdo;     /**< App callback: TPDO transmitted. */
} canopen_config;

/**
 * @brief Runtime state of the CANopen slave layer.
 *
 * The single global instance is @ref canopen.
 * Populated by @ref canopen_init() and updated continuously by
 * @ref canopen_process() and the internal stack callbacks.
 */
typedef struct
{
    co_dev_t  *dev;   /**< CANopen device object (slave Object Dictionary). */
    co_nmt_t  *nmt;   /**< NMT slave object. */
    can_net_t *net;   /**< Lely CAN network layer instance. */

    canopen_bus_state bus_state;           /**< Current bus / NMT operational state. */
    uint8_t           bus_id;              /**< Active bus index (0 = A, 1 = B). */
    bool              reset_comm_pending;  /**< @c true when a communication reset has been
                                                requested and is waiting to be executed. */
    bool              reset_comm_is_recovery; /**< @c true when the pending comm reset is
                                                   part of a redundancy recovery sequence. */
    uint8_t           saved_bus_id;        /**< Bus index saved before a comm reset so it
                                                can be restored afterwards. */
    canopen_config    cfg;                 /**< Application-supplied configuration (HAL + callbacks). */
} canopen_t;


/**
 * @brief Global CANopen slave state instance.
 * @note  Initialised by @ref canopen_init(). Treat internal fields as read-only
 *        from outside this module.
 */
extern canopen_t canopen;
/**
 * @brief Initialises the CANopen slave stack.
 *
 * Allocates and configures: memory pool, can_net, Object Dictionary, NMT,
 * RPDO and TPDO objects. Registers all internal stack callbacks and writes
 * the ECSS redundancy parameters (OD 0x2000) to the OD.
 * Ends by issuing NMT RESET_NODE, transitioning the slave to Pre-Operational
 * and then to Operational once the master issues NMT START.
 *
 * @param[in] conf  Layer configuration (HAL functions + application callbacks).
 * @return @ref CANOPEN_OK on success, @ref CANOPEN_ERROR on any failure.
 */
canopen_status canopen_init(canopen_config conf);
/**
 * @brief Processes all pending CANopen events – call from the main loop.
 *
 * Responsibilities (in order):
 * -# Advances the Lely stack clock to the current monotonic time.
 * -# Drains all pending CAN frames from bus A then bus B.
 * -# Executes a deferred communication reset if @c reset_comm_pending is set.
 *
 * @note Not thread-safe; call only from a single execution context.
 */
void canopen_process(void);


#endif /* INC_CANOPEN_H_ */
