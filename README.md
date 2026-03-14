# FreeRTOS Manual Porting Guide — STM32F401RE (Nucleo)

> **Board:** STM32F401RETx (Cortex-M4F, 84 MHz)
> **IDE:** STM32CubeIDE 2.1.0 on Linux
> **FreeRTOS:** v202012.00

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [Create New Project in CubeIDE](#2-create-new-project-in-cubeide)
3. [Configure in STM32CubeMX](#3-configure-in-stm32cubemx)
4. [Download FreeRTOS](#4-download-freertos)
5. [Copy FreeRTOS Files into Project](#5-copy-freertos-files-into-project)
6. [Fix CMSIS Include Folder](#6-fix-cmsis-include-folder)
7. [Create FreeRTOSConfig.h](#7-create-freertosconfigh)
8. [Configure Build Settings in CubeIDE](#8-configure-build-settings-in-cubeide)
9. [Write main.c](#9-write-mainc)
10. [Fix syscalls.c for printf](#10-fix-syscallsc-for-printf)
11. [Build and Flash](#11-build-and-flash)
12. [View Output via SWV ITM Console](#12-view-output-via-swv-itm-console)
13. [Common Errors and Fixes](#13-common-errors-and-fixes)
14. [Project Folder Structure](#14-project-folder-structure)

---

## 1. Prerequisites

Make sure you have the following installed:

- STM32CubeIDE 2.1.0 (Linux)
- STM32CubeMX (standalone)
- STM32F401RE Nucleo board + USB cable
- FreeRTOS v202012.00 source (downloaded in Step 4)

---

## 2. Create New Project in CubeIDE

1. Open **STM32CubeIDE**
2. Go to **File → New → STM32 Project**
3. In the board selector, search for `STM32F401RETx`
4. Select it → click **Next**
5. Enter a **Project Name** (e.g. `freertos_demo`)
6. Click **Finish**

---

## 3. Configure in STM32CubeMX

On Linux, open CubeMX from terminal with your `.ioc` file:

```bash
/home/<user>/STM32CubeMX/STM32CubeMX \
  /home/<user>/STM32CubeIDE/projects/freertos_demo/freertos_demo.ioc &
```

### Pinout & Configuration tab

**System Core → SYS**
- Set **Timebase Source** → `TIM5`
- ⚠️ STM32F401RE has no TIM6 — use TIM5
- ⚠️ FreeRTOS uses SysTick internally so HAL must use a different timer

**System Core → NVIC**
- Set **Priority Group** → `4 bits for pre-emption priority (0 bits for subpriority)`
- ⚠️ FreeRTOS on Cortex-M4 requires all priority bits for pre-emption

**GPIO — PA5 (Onboard LED LD2)**
- Click pin **PA5** on the chip diagram
- Select **GPIO_Output**

### Clock Configuration tab

- Find the **HCLK** box
- Type `84` and press **Enter**
- CubeMX auto-calculates: PLLM=16, PLLN=336, PLLP=/4

### Generate Code

- Click **GENERATE CODE** button (top right)
- Click **Yes** when prompted to open in CubeIDE

---

## 4. Download FreeRTOS

```bash
cd ~
wget https://github.com/FreeRTOS/FreeRTOS/releases/download/202012.00/FreeRTOSv202012.00.zip
unzip FreeRTOSv202012.00.zip
```

---

## 5. Copy FreeRTOS Files into Project

Replace `<user>` and `<project>` with your actual username and project name.

```bash
PROJECT=/home/<user>/STM32CubeIDE/projects/<project>
FREERTOS=/home/<user>/FreeRTOSv202012.00/FreeRTOS/Source

# Create folder structure
mkdir -p $PROJECT/Thirdparty/FreeRTOS/portable/GCC/ARM_CM4F
mkdir -p $PROJECT/Thirdparty/FreeRTOS/portable/MemMang

# Copy core FreeRTOS source files
cp $FREERTOS/*.c $PROJECT/Thirdparty/FreeRTOS/

# Copy FreeRTOS header files
cp -r $FREERTOS/include $PROJECT/Thirdparty/FreeRTOS/

# Copy Cortex-M4F port files
cp -r $FREERTOS/portable/GCC/ARM_CM4F \
      $PROJECT/Thirdparty/FreeRTOS/portable/GCC/

# Copy heap_4 memory management
cp $FREERTOS/portable/MemMang/heap_4.c \
   $PROJECT/Thirdparty/FreeRTOS/portable/MemMang/
```

After running, refresh in CubeIDE:
**Right-click project → Refresh**

---

## 6. Fix CMSIS Include Folder

CubeMX sometimes generates an **empty** `Drivers/CMSIS/Include/` folder which causes `core_cm4.h: No such file or directory` errors.

Check if it is empty:
```bash
ls $PROJECT/Drivers/CMSIS/Include/
```

If empty, copy the CMSIS headers from your STM32Cube repository:
```bash
cp /home/<user>/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include/* \
   $PROJECT/Drivers/CMSIS/Include/
```

If you are unsure of the path, find it with:
```bash
find /home/<user> -name "core_cm4.h" 2>/dev/null | grep "STM32Cube"
```

---

## 7. Create FreeRTOSConfig.h

```bash
cat > $PROJECT/Thirdparty/FreeRTOS/FreeRTOSConfig.h << 'EOF'
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if defined(__ICCARM__) || defined(__GNUC__) || defined(__CC_ARM)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;
#endif

/* Scheduler */
#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0

/* Clock and Tick */
#define configCPU_CLOCK_HZ                      ( SystemCoreClock )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )

/* Tasks */
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( uint16_t ) 128 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 15 * 1024 ) )
#define configMAX_TASK_NAME_LEN                 ( 10 )
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1

/* Synchronisation */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8

/* Software Timers */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( 2 )
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )

/* Debug */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configCHECK_FOR_STACK_OVERFLOW          0

/* API Includes */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskResumeFromISR              1

/* Cortex-M4 Interrupt Priority */
#ifdef __NVIC_PRIO_BITS
  #define configPRIO_BITS                       __NVIC_PRIO_BITS
#else
  #define configPRIO_BITS                       4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

#define configKERNEL_INTERRUPT_PRIORITY \
    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Map FreeRTOS port handlers to CMSIS handler names */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#define configASSERT( x ) \
    if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for(;;); }

#endif /* FREERTOS_CONFIG_H */
EOF
```

---

## 8. Configure Build Settings in CubeIDE

### Step A — Exclude portable folder, then un-exclude only what is needed

| Folder | Action |
|--------|--------|
| `Thirdparty/FreeRTOS/portable` | ✅ Check **Exclude from build** |
| `Thirdparty/FreeRTOS/portable/GCC/ARM_CM4F` | ❌ Uncheck **Exclude from build** |
| `Thirdparty/FreeRTOS/portable/MemMang` | ❌ Uncheck **Exclude from build** |

For each folder:
> Right-click folder → **Properties → C/C++ Build → Settings** → check/uncheck **Exclude resource from build** → Apply and Close

### Step B — Add FreeRTOS Include Paths to PROJECT

Right-click **project root** → **Properties**
→ **C/C++ Build → Settings → MCU GCC Compiler → Include paths**

Click the **green + button** and add these 3 paths one by one:

```
"${workspace_loc:/${ProjName}/Thirdparty/FreeRTOS}"
"${workspace_loc:/${ProjName}/Thirdparty/FreeRTOS/include}"
"${workspace_loc:/${ProjName}/Thirdparty/FreeRTOS/portable/GCC/ARM_CM4F}"
```

Click **Apply and Close**

> ⚠️ Always add include paths to the **project root**, not to the Thirdparty folder.

---

## 9. Write main.c

Replace the full contents of `Core/Src/main.c`:

```c
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

static void task1_handler(void* parameters);
static void task2_handler(void* parameters);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  TaskHandle_t task1_handle;
  TaskHandle_t task2_handle;
  BaseType_t   status;

  status = xTaskCreate(task1_handler, "Task-1", 200,
                       "Hello world from Task-1", 2, &task1_handle);
  configASSERT(status == pdPASS);

  status = xTaskCreate(task2_handler, "Task-2", 200,
                       "Hello world from Task-2", 2, &task2_handle);
  configASSERT(status == pdPASS);

  vTaskStartScheduler();

  while (1) {}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = 16;
  RCC_OscInitStruct.PLL.PLLN            = 336;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ            = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                     RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin   = GPIO_PIN_5;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void task1_handler(void* parameters)
{
  while(1) {
    printf("%s\n", (char*)parameters);
    taskYIELD();
  }
}

static void task2_handler(void* parameters)
{
  while(1) {
    printf("%s\n", (char*)parameters);
    taskYIELD();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while(1) {}
}
```

---

## 10. Fix syscalls.c for printf

Open `Core/Src/syscalls.c` and replace the `_write` function body so that `printf` output goes to the SWV ITM Data Console:

```c
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
```

---

## 11. Build and Flash

In CubeIDE press:

```
Ctrl + B    →  Build project
```

Expected result:
```
Build Finished. 0 errors, 0 warnings.
```

To flash the firmware onto the board:
- Click the **Debug** button (bug icon) in the toolbar
- Click **Resume (F8)** to start execution

---

## 12. View Output via SWV ITM Console

1. Click **Debug** button → confirm ST-LINK connection
2. Go to **Debugger** tab → enable **Serial Wire Viewer (SWV)**
3. Set **Core Clock = 84 MHz**
4. Click **Debug**
5. In CubeIDE: **Window → Show View → SWV → SWV ITM Data Console**
6. Click **Configure trace** → enable **Port 0**
7. Click **Start trace** (red record button)
8. Press **Resume (F8)**

Expected console output:
```
Hello world from Task-1
Hello world from Task-2
Hello world from Task-1
Hello world from Task-2
...
```

---

## 13. Common Errors and Fixes

| Error | Cause | Fix |
|-------|-------|-----|
| `FreeRTOS.h: No such file or directory` | Include paths not added to project | Add 3 FreeRTOS paths to project root Include paths (Step 8B) |
| `core_cm4.h: No such file or directory` | CMSIS/Include folder is empty | Copy CMSIS headers from STM32Cube repository (Step 6) |
| `SystemCoreClock undeclared` | `stm32f4xx.h` included in FreeRTOSConfig.h | Remove `#include "stm32f4xx.h"` — keep only `extern uint32_t SystemCoreClock` |
| `Multiple definition of SysTick_Handler` | HAL and FreeRTOS both using SysTick | Set Timebase Source to **TIM5** in CubeMX (Step 3) |
| `TaskHandle_t undeclared` | FreeRTOS includes placed inside `main.h` | Move all FreeRTOS includes to `main.c` only — keep `main.h` clean |
| Board hangs at startup / no output | Heap too small or stack overflow | Increase `configTOTAL_HEAP_SIZE` in FreeRTOSConfig.h |

---

## 14. Project Folder Structure

```
freertos_demo/
├── Core/
│   ├── Inc/
│   │   ├── main.h                        ← Clean — no FreeRTOS includes here
│   │   ├── stm32f4xx_hal_conf.h
│   │   └── stm32f4xx_it.h
│   └── Src/
│       ├── main.c                        ← FreeRTOS tasks here
│       ├── stm32f4xx_hal_msp.c
│       ├── stm32f4xx_hal_timebase_tim.c  ← TIM5 timebase (generated by CubeMX)
│       ├── stm32f4xx_it.c
│       ├── syscalls.c                    ← ITM_SendChar for printf
│       └── system_stm32f4xx.c
├── Drivers/
│   ├── CMSIS/
│   │   ├── Include/                      ← Must NOT be empty
│   │   └── Device/ST/STM32F4xx/
│   └── STM32F4xx_HAL_Driver/
├── Thirdparty/
│   └── FreeRTOS/
│       ├── FreeRTOSConfig.h              ← Your config
│       ├── include/                      ← FreeRTOS headers
│       ├── portable/
│       │   ├── GCC/
│       │   │   └── ARM_CM4F/             ← port.c (NOT excluded from build)
│       │   └── MemMang/
│       │       └── heap_4.c              ← NOT excluded from build
│       ├── tasks.c
│       ├── queue.c
│       ├── list.c
│       ├── timers.c
│       ├── event_groups.c
│       └── stream_buffer.c
└── freertos_demo.ioc
```

