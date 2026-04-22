# CanOpen ECSS SI-Robotics

CANopen library with **ECSS** redundancy (dual bus A/B), **CiA 402 CSP/FSA** state machine, and automatic Master ↔ Slave lifecycle management on STM32 (Nucleo G474RE).

---

## Table of Contents

- [Library Architecture](#library-architecture)
- [Hardware Connection](#hardware-connection)
- [Building lely-core](#building-lely-core)
- [Generating OD from DCF Files](#generating-od-from-dcf-files)
- [Slave Project — actuatorSpace](#slave-project--actuatorspace)
- [Master Project](#master-project)

---

## Library Architecture

The refactor replaced 3 monolithic libraries with 4 dedicated ones — 2 for the **master** and 2 for the **slave/actuator**. Each project now contains only the code it actually needs.

```
          BEFORE                              AFTER
┌─────────────────────────┐     ┌──────────────────────────────┐
│ · Actuator Library      │     │ MASTER                       │
│ · Cia402 wrapper        │ ──▶ │   · Cia402 master wrapper    │
│ · CANopen space wrapper │     │   · CANopen space master wrap │
└─────────────────────────┘     ├──────────────────────────────┤
                                │ SLAVE / ACTUATOR             │
                                │   · Cia402 wrapper           │
                                │   · CANopen space wrapper    │
                                └──────────────────────────────┘
```

---

## Hardware Connection

Two Nucleo G474RE boards are connected via **two CAN buses** implementing ECSS redundancy (bus A and bus B):

```
┌──────────────────────┐                       ┌──────────────────────┐
│                      │ PB13 ── CAN 2 TX ── PB13│                      │
│                      │ PB12 ── CAN 2 RX ── PB12│                      │
│   Nucleo Actuator    │ PA12 ── CAN 1 TX ── PA12│    Nucleo Master     │
│      (Slave)         │ PA11 ── CAN 1 RX ── PA11│                      │
│                      │                         │                      │
└──────────────────────┘                       └──────────────────────┘
       FDCAN1 = bus A (CAN 1)
       FDCAN2 = bus B (CAN 2, ECSS redundancy)
```

| Slave Pin | Signal   | Master Pin |
|-----------|----------|------------|
| PB13      | CAN 2 TX | PB13       |
| PB12      | CAN 2 RX | PB12       |
| PA12      | CAN 1 TX | PA12       |
| PA11      | CAN 1 RX | PA11       |

---

## Building lely-core

> **Requirements:** `arm-none-eabi-gcc`, `autoconf`, `automake`, `libtool`

```bash
git clone https://gitlab.com/n7space/canopen/lely-core.git --depth=1 --branch=v3.5.0
cd lely-core
autoreconf -i

./configure --host=arm-none-eabi \
  "LDFLAGS= -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16 \
  -ffunction-sections \
  -fdata-sections \
  -Wl,--gc-sections \
  --specs=nosys.specs" \
  "CFLAGS= -O2 -ggdb3 \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16 \
  -ffunction-sections \
  -fdata-sections \
  -DLELY_HAVE_ITIMERSPEC=1" \
  --enable-ecss-compliance \
  --disable-shared \
  --disable-python \
  --disable-tests \
  --disable-unit-tests \
  --disable-threads

make -j
make check           # run unit tests
make html            # build HTML documentation
sudo make install    # system-wide install

# local install for use in the STM32 project
make install DESTDIR="$PWD/_stm32_install"
```

Compiled `.a` files and headers will be placed in `_stm32_install/`.

---

## Generating OD from DCF Files

DCF files describe the **Object Dictionary** of a node. The `dcf2dev` tool generates `.h/.c` files ready to include in the STM32 project.

### Installing dcf-tools

```bash
cd python/dcf-tools

python3 -m venv .venv
source .venv/bin/activate

pip install -U pip wheel
pip install empy pyyaml
pip install "setuptools==65.5.1"
python3 setup.py install
```

### Generating OD Code

```bash
# from the repository root

# Master
dcf2dev --header dcfs/master.dcf od_master > dcfs/od_master.h
dcf2dev          dcfs/master.dcf od_master > dcfs/od_master.c

# Slave
dcf2dev --header dcfs/slave.dcf od_slave > dcfs/od_slave.h
dcf2dev          dcfs/slave.dcf od_slave > dcfs/od_slave.c
```

Copy the generated files into the respective CubeIDE projects.

---

## Slave Project — actuatorSpace

CubeIDE project for the **Slave / Actuator** node. The slave operates autonomously and always maintains its heartbeat regardless of master presence.

### Node Behaviour

| Event | Response |
|-------|----------|
| Master lost | Quick Stop → `RESET_COMM` (restarts HB consumer) |
| Master reconnected | Automatic resume without NMT RESET |
| SYNC received | Position update + TPDO transmission |
| BUS-OFF detected | FDCAN controller stop and restart |

ECSS oscillation is eliminated via `Ttoggle = 30` and dynamic `Bdefault`.

### Initialization (`main.c`)

```c
#include "canopen.h"
#include "cia402.h"

/* Callbacks — invoked automatically by the library */
static void app_on_sync   (uint8_t cnt) { cia402_on_sync(cnt);  }
static void app_on_running (void)       { cia402_on_running();   }
static void app_on_stopped (void)       { cia402_on_stopped();   }
static void app_on_tpdo    (void)       { cia402_on_tpdo();      }

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_FDCAN1_Init();
    MX_FDCAN2_Init();
    MX_USART2_UART_Init();

    HAL_FDCAN_Start(&hfdcan1);
    HAL_FDCAN_Start(&hfdcan2);

    /* Enable BUS-OFF notifications on both buses */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_BUS_OFF, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_BUS_OFF, 0);

    canopen_config cfg = {
        .send       = my_can_send,
        .recv       = my_can_recv,
        .time       = my_get_time,
        .on_sync    = app_on_sync,
        .on_running = app_on_running,
        .on_stopped = app_on_stopped,
        .on_tpdo    = app_on_tpdo,
    };

    canopen_init(cfg);
    cia402_init(canopen.dev);   /* initialize CiA 402 state machine */

    while (true) {
        canopen_process();      /* must be called every ~1 ms */
        HAL_Delay(1);
    }
}
```

### BUS-OFF Recovery

```c
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    if (ErrorStatusITs & FDCAN_IT_BUS_OFF) {
        HAL_FDCAN_Stop(hfdcan);
        HAL_FDCAN_Start(hfdcan);
    }
}
```

---

## Master Project

CubeIDE project for the **Master** node. Controls the slave via RPDO/TPDO in **CSP (Cyclic Synchronous Position)** mode. Implemented primarily to demonstrate slave functionality.

### Node Behaviour

- Sends SYNC cyclically via TIM6
- Changes the target position every **3 seconds** cycling through `{0, 1000, −1000, 500, −500}`
- Handles ECSS redundancy — automatic switchover between bus A and bus B

### Initialization (`main.c`)

```c
#include "canopen_master.h"
#include "cia402_master.h"

/* Callbacks */
static void app_on_sync   (uint8_t cnt)              { cia402_master_on_sync(cnt);      }
static void app_on_running (void)                    { cia402_master_on_running();       }
static void app_on_stopped (void)                    { cia402_master_on_stopped();       }
static void app_on_rpdo   (uint16_t sw, int32_t pos) { cia402_master_on_rpdo(sw, pos);  }

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_FDCAN1_Init();
    MX_FDCAN2_Init();
    MX_USART2_UART_Init();
    MX_TIM6_Init();

    HAL_FDCAN_Start(&hfdcan1);
    HAL_FDCAN_Start(&hfdcan2);
    HAL_TIM_Base_Start_IT(&htim6);

    canopen_config cfg = {
        .send       = my_can_send,
        .recv       = my_can_recv,
        .time       = my_get_time,
        .on_sync    = app_on_sync,
        .on_running = app_on_running,
        .on_stopped = app_on_stopped,
        .on_rpdo    = app_on_rpdo,
    };

    canopen_init(cfg);
    cia402_master_init(canopen.dev, canopen.csdo);

    static const int32_t positions[] = { 0, 1000, -1000, 500, -500 };
    static uint8_t  pos_idx   = 0u;
    static uint32_t last_tick = 0u;

    while (1) {
        uint32_t now = HAL_GetTick();
        if (now - last_tick >= 3000u &&
            cia402_master.state == CIA402_M_RUNNING)
        {
            last_tick = now;
            pos_idx   = (pos_idx + 1u) % 5u;
            cia402_master_set_target(positions[pos_idx]);
        }

        canopen_process();      /* must be called every ~1 ms */
        HAL_Delay(1);
    }
}
```

### SYNC Generation via TIM6

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        struct can_msg sync = { .id = 0x080u, .len = 0u };
        canopen.cfg.send(canopen.bus_id, &sync);
    }
}
```

---

## License

This project is based on [lely-core](https://gitlab.com/n7space/canopen/lely-core), available under the **Apache 2.0** license.
