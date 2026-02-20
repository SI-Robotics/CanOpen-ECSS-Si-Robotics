#ifndef INC_CANOPEN_SPACE_H_
#define INC_CANOPEN_SPACE_H_
/**
 * @file canopen_space.h
 * @brief Lightweight CANopen/Lely-core “space” wrapper (init/start/tick/rx/tx helpers).
 *
 * This module wraps a minimal subset of Lely-core CANopen stack to be used on bare-metal
 * targets (STM32) with an external transport callback for CAN TX.
 *
 * Responsibilities:
 * - Create and configure Lely objects: can_net, co_dev (OD), co_nmt.
 * - Provide 1ms timebase integration (canopen_space_tick_1ms()).
 * - Feed received CAN frames into the stack (canopen_space_on_rx()).
 * - Optionally create RPDO1/TPDO1 objects on ACTUATOR role and configure PDO mapping runtime.
 * - Provide raw CAN send helper and a simple SDO write helper (expedited u32).
 *
 * The OD itself is provided by generated headers:
 * - od_slave_pdo.h / od_master_pdo.h
 */

#include <lely/config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <lely/can/msg.h>
#include <lely/can/net.h>
#include <lely/co/dev.h>
#include <lely/co/obj.h>
#include <lely/co/nmt.h>
#include <lely/util/error.h>
#include <lely/util/memory.h>
#include <lely/util/mempool.h>
#include <string.h>
#include <lely/co/rpdo.h>
#include <lely/co/tpdo.h>
#include "od_slave_pdo.h"
#include "od_master_pdo.h"

/**
 * @brief Runtime role of the stack instance.
 */
typedef enum {
  CO_ROLE_MASTER = 0,   /**< Node acts as CANopen master/controller. */
  CO_ROLE_ACTUATOR = 1, /**< Node acts as actuator/slave device. */
} canopen_space_role_t;

/**
 * @brief User-provided CAN send callback.
 *
 * The stack calls this to transmit CAN frames via the platform driver (FDCAN, bxCAN, etc.).
 *
 * @param bus_id Logical bus identifier.
 * @param msg    CAN frame to send (Lely can_msg).
 * @param user   User context pointer (cfg->send_user).
 * @return 0 on success, <0 on error.
 */
typedef int (*canopen_space_send_func_t)(uint_least8_t bus_id, const struct can_msg* msg, void* user);

/**
 * @brief Configuration passed to canopen_space_init().
 */
typedef struct {
  canopen_space_role_t role; 		/**< Role (master/actuator). */
  uint8_t node_id; 					/**< Local node ID used by OD/NMT. */
  canopen_space_send_func_t send;	/**< Mandatory transport TX callback. */
  void* send_user;					/**< User context for send callback. */
  void* mem;						/**< Memory region for Lely mempool. */
  uint32_t mem_size; 				/**< Size of the memory region. */
} canopen_space_cfg_t;

/**
 * @brief Global context of the canopen_space instance.
 *
 * This module currently uses a single static instance internally.
 */
typedef struct
{
  /* mempool */
  struct mempool pool;  /**< Lely mempool descriptor. */
  uint8_t* pool_mem;/**< Raw memory block for mempool. */
  uint32_t pool_size;/**< Size of pool_mem. */

  /* lely objects */
  can_net_t* net;						/**< CAN network object. */
  co_dev_t*  dev; 						/**< Object dictionary (OD). */
  co_nmt_t*  nmt; 						/**< NMT service object. */
  co_rpdo_t* rpdo1;						/**< RPDO1 object (actuator role). */
  co_tpdo_t* tpdo1; 					/**< TPDO1 object (actuator role). */
  /* time */
  struct timespec ts;					/**< Timebase used by can_net_set_time(). */
  /* config */
  canopen_space_role_t role;			/**< Role (cached from cfg). */
  uint8_t node_id;						/**< Local node ID (cached from cfg). */
  /* transport */
  canopen_space_send_func_t send; 		/**< TX callback (cached). */
  void* send_user; 						/**< TX callback user context. */
  /* debug */
  volatile uint32_t last_rx_id;			/**< Last received CAN ID fed to stack. */
  volatile uint32_t last_tx_id; 		/**< Last transmitted CAN ID. */
  volatile uint32_t fail_code; 			/**< Optional failure code for debugging. */

} canopen_space_ctx_t;
/**
 * @brief Initialize the CANopen/Lely stack instance using the provided configuration.
 *
 * Creates mempool, can_net, OD (master or slave), configures heartbeat objects,
 * creates NMT and (optionally) PDO objects for ACTUATOR role.
 *
 * @param cfg Pointer to init configuration.
 * @retval 0  Success.
 * @retval <0 Parameter/initialization error.
 */
int  canopen_space_init(const canopen_space_cfg_t* cfg);
/**
 * @brief Start the stack services depending on role.
 *
 * For ACTUATOR role this typically starts RPDO/TPDO objects and transitions NMT states
 * (Reset comm -> Pre-op -> Operational).
 * For MASTER role it performs minimal local NMT init (implementation-defined).
 *
 * @return 0 on success, <0 on error.
 */
int  canopen_space_start(void);
/**
 * @brief Advance stack time by 1 ms (must be called every 1 ms).
 *
 * Updates internal timespec and calls can_net_set_time().
 * This is critical on bare-metal targets without POSIX clocks.
 */
void canopen_space_tick_1ms(void);
/**
 * @brief Feed an incoming CAN frame into the Lely stack.
 *
 * Call this from your CAN RX interrupt/callback (after copying payload).
 *
 * @param bus_id Bus identifier.
 * @param id     CAN identifier.
 * @param ext    Extended ID flag.
 * @param rtr    Remote transmission request flag.
 * @param data   Payload pointer (may be NULL if len=0).
 * @param len    Payload length (0..8).
 */
void canopen_space_on_rx(uint_least8_t bus_id,uint32_t id,bool ext,bool rtr,const uint8_t* data,uint8_t len);
/**
 * @brief Get pointer to the initialized Object Dictionary (OD).
 * @return OD pointer or NULL if not initialized.
 */
co_dev_t*  canopen_space_dev(void);
/**
 * @brief Get pointer to the NMT object.
 * @return NMT pointer or NULL if not initialized.
 */
co_nmt_t*  canopen_space_nmt(void);
/**
 * @brief Get pointer to TPDO1 (actuator role).
 * @return TPDO1 pointer or NULL if not created.
 */
co_tpdo_t* canopen_space_tpdo1(void);
/**
 * @brief Get pointer to RPDO1 (actuator role).
 * @return RPDO1 pointer or NULL if not created.
 */
co_rpdo_t* canopen_space_rpdo1(void);



/**
 * @brief Send an expedited SDO download (u32) to a node (COB-ID 0x600 + node_id).
 *
 * Builds an 8-byte "Download Initiate" request with command specifier 0x23:
 * - expedited transfer
 * - size indicated
 * - 4 data bytes (u32)
 *
 * Payload layout:
 * - byte 0 : 0x23 (SDO Download Initiate, expedited, size=4)
 * - byte 1 : index low
 * - byte 2 : index high
 * - byte 3 : subindex
 * - byte 4..7 : value (little-endian)
 *
 * @param bus_id  Bus identifier used by canopen_space transport.
 * @param node_id Destination node ID.
 * @param idx     Object dictionary index.
 * @param sub     Object dictionary subindex.
 * @param val     32-bit value to write.
 *
 * @retval 0  Success.
 * @retval <0 Transport error.
 */
int canopen_space_sdo_write_u32(uint8_t bus_id,uint8_t node_id,uint16_t idx,uint8_t sub,uint32_t val);

/**
 * @brief Send a raw CAN frame using the user TX callback.
 *
 * @param bus_id Bus identifier.
 * @param id     CAN identifier.
 * @param ext    Extended ID flag.
 * @param rtr    RTR flag.
 * @param data   Payload pointer (may be NULL if len=0).
 * @param len    Payload length (0..8).
 * @return 0 on success, <0 on error.
 */



int canopen_space_send_raw(uint_least8_t bus_id,uint32_t id,bool ext,bool rtr,const uint8_t* data,uint8_t len);
/**
 * @brief Send an expedited SDO download (u32) to a node (COB-ID 0x600 + node).
 *
 * Builds an 8-byte SDO Download Initiate (0x23) frame and sends it via canopen_space_send_raw().
 *
 * @param bus_id  Bus identifier.
 * @param node_id Destination node ID.
 * @param idx     Object index.
 * @param sub     Object sub-index.
 * @param val     32-bit value to write.
 * @return 0 on success, <0 on error.
 */
int canopen_space_sdo_write_u32(uint8_t bus_id,uint8_t node_id,uint16_t idx,uint8_t sub, uint32_t val);

#endif /* INC_CANOPEN_SPACE_H_ */
