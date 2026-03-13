# FreeRTOS Simple Semaphore Demo — STM32F401RE

A beginner-friendly FreeRTOS project running on STM32F401RE Nucleo board
that demonstrates binary semaphore, task scheduling, and priority-based
task execution using UART serial output.

---

## Hardware Required

| Component         | Details                        |
|-------------------|-------------------------------|
| MCU Board         | STM32F401RE Nucleo-64          |
| USB Cable         | Mini USB (for programming)     |
| Serial Monitor    | PuTTY / Minicom                |
| Optional          | USB OTG + Android Phone        |

---

## Software Required

| Software          | Version / Link                                      |
|-------------------|-----------------------------------------------------|
| STM32CubeIDE      | 2.x — https://www.st.com/stm32cubeide              |
| FreeRTOS          | Included in project (Thirdparty folder)             |
| PuTTY             | https://www.putty.org                               |
| ARM GCC Toolchain | arm-none-eabi-gcc (bundled with STM32CubeIDE)       |

---


## Project Structureproject/
├── Core/
│   ├── Inc/
│   │   └── main.h
│   └── Src/
│       └── main.c          ← All application code here
├── Thirdparty/
│   └── FreeRTOS/
│       ├── include/        ← FreeRTOS headers
│       ├── portable/       ← ARM Cortex-M4 port
│       └── *.c             ← FreeRTOS source files
├── Drivers/
│   ├── STM32F4xx_HAL_Driver/
│   └── CMSIS/
└── README.md
## Project Structure



# FreeRTOS Simple Semaphore Demo — STM32F401RE

A beginner-friendly FreeRTOS project running on STM32F401RE Nucleo board
that demonstrates binary semaphore, task scheduling, and priority-based
task execution using UART serial output.

---

## Hardware Required

| Component         | Details                        |
|-------------------|-------------------------------|
| MCU Board         | STM32F401RE Nucleo-64          |
| USB Cable         | Mini USB (for programming)     |
| Serial Monitor    | PuTTY / Minicom                |
| Optional          | USB OTG + Android Phone        |

---

## Software Required

| Software          | Version / Link                                      |
|-------------------|-----------------------------------------------------|
| STM32CubeIDE      | 2.x — https://www.st.com/stm32cubeide              |
| FreeRTOS          | Included in project (Thirdparty folder)             |
| PuTTY             | https://www.putty.org                               |
| ARM GCC Toolchain | arm-none-eabi-gcc (bundled with STM32CubeIDE)       |

---

## Project Structure
```
freertos-semaphore-demo/
│
├── 📄 README.md
├── 📄 .gitignore
│
├── 📁 Core/
│   ├── 📁 Inc/
│   │   ├── main.h
│   │   ├── stm32f4xx_hal_conf.h
│   │   └── stm32f4xx_it.h
│   │
│   └── 📁 Src/
│       ├── main.c                  ← YOUR APPLICATION CODE
│       ├── stm32f4xx_hal_msp.c
│       ├── stm32f4xx_it.c
│       └── syscalls.c
│
├── 📁 Thirdparty/
│   └── 📁 FreeRTOS/
│       ├── 📁 include/
│       │   ├── FreeRTOS.h
│       │   ├── task.h
│       │   ├── semphr.h
│       │   ├── queue.h
│       │   ├── timers.h
│       │   ├── event_groups.h
│       │   ├── list.h
│       │   ├── portable.h
│       │   └── projdefs.h
│       │
│       ├── 📁 portable/
│       │   └── 📁 GCC/
│       │       └── 📁 ARM_CM4F/
│       │           ├── port.c
│       │           └── portmacro.h
│       │
│       ├── croutine.c
│       ├── event_groups.c
│       ├── list.c
│       ├── queue.c
│       ├── stream_buffer.c
│       ├── tasks.c
│       └── timers.c
│
├── 📁 Drivers/
│   ├── 📁 STM32F4xx_HAL_Driver/
│   │   ├── 📁 Inc/
│   │   │   ├── stm32f4xx_hal.h
│   │   │   ├── stm32f4xx_hal_gpio.h
│   │   │   ├── stm32f4xx_hal_uart.h
│   │   │   ├── stm32f4xx_hal_rcc.h
│   │   │   ├── stm32f4xx_hal_cortex.h
│   │   │   └── stm32f4xx_hal_pwr.h
│   │   │
│   │   └── 📁 Src/
│   │       ├── stm32f4xx_hal.c
│   │       ├── stm32f4xx_hal_gpio.c
│   │       ├── stm32f4xx_hal_uart.c
│   │       ├── stm32f4xx_hal_rcc.c
│   │       ├── stm32f4xx_hal_cortex.c
│   │       └── stm32f4xx_hal_pwr.c
│   │
│   └── 📁 CMSIS/
│       ├── 📁 Device/
│       │   └── 📁 ST/
│       │       └── 📁 STM32F4xx/
│       │           └── 📁 Include/
│       │               ├── stm32f401xe.h
│       │               └── system_stm32f4xx.h
│       │
│       └── 📁 Include/
│           ├── core_cm4.h
│           ├── cmsis_gcc.h
│           └── cmsis_version.h
│
├── 📁 Debug/                       ← AUTO GENERATED by IDE
│   ├── Core/
│   ├── Drivers/
│   ├── Thirdparty/
│   ├── makefile
│   ├── new_naveen.elf
│   ├── new_naveen.map
│   └── new_naveen.bin
│
├── 📄 STM32F401RETX_FLASH.ld       ← Linker script
├── 📄 STM32F401RETX_RAM.ld
└── 📄 .project                     ← STM32CubeIDE project file
```

---

## System Overview
```
STM32F401RE @ 84 MHz
│
├── Task-1  Priority 3  (Highest)
├── Task-2  Priority 2  (Medium)
├── Task-3  Priority 1  (Lowest — never runs)
└── Binary Semaphore    (controls access)
```

---

## How It Works

### Binary Semaphore
Only one task can hold the semaphore at a time.
When a task takes it, others wait (BLOCKED state).
When released, the highest priority waiting task gets it next.

### Task Execution Order
```
Semaphore free
      │
      ▼
Task-1 takes (Priority 3 — highest)
Task-1 works 300ms
Task-1 gives semaphore
Task-1 sleeps 100ms
      │
      ▼
Task-2 takes (Priority 2)
Task-2 works 300ms
Task-2 gives semaphore
Task-2 sleeps 100ms
      │
      ▼
Task-1 takes again (already woke up)
      │
      └── repeats forever
```

### Why Task-3 Never Runs
Task-3 has the lowest priority (1).
Task-1 (priority 3) and Task-2 (priority 2) always
grab the semaphore before Task-3 gets a chance.
This demonstrates **task starvation**.

---

## UART Configuration

| Parameter    | Value        |
|--------------|-------------|
| Port         | USART2       |
| TX Pin       | PA2          |
| RX Pin       | PA3          |
| Baud Rate    | 115200       |
| Data Bits    | 8            |
| Stop Bits    | 1            |
| Parity       | None         |
| Flow Control | None         |

---

## Clock Configuration

| Parameter    | Value              |
|--------------|--------------------|
| Source       | HSI (16 MHz)       |
| PLL          | PLLM=16 PLLN=336   |
| PLLP         | DIV4               |
| System Clock | 84 MHz             |
| APB1         | 42 MHz             |
| APB2         | 84 MHz             |

---

## How to Build and Flash

### Step 1 — Clone the Repository
```bash
git clone https://github.com/yourusername/freertos-semaphore-demo.git
```

### Step 2 — Open in STM32CubeIDE
```
File → Open Projects from File System
Select the cloned folder
Click Finish
```

### Step 3 — Build
```
Project → Build Project
or
Ctrl + B
```

### Step 4 — Flash to Board
```
Connect STM32 Nucleo via USB
Run → Debug (F11)
or
Run → Run (Ctrl + F11)
```

---

## Viewing Output

### Using PuTTY (Linux / Windows)
```bash
# Find port
ls /dev/ttyACM*

# Open PuTTY directly
sudo putty -serial /dev/ttyACM0 -sercfg 115200,8,n,1,N
```

PuTTY Settings:
```
Connection Type : Serial
Serial Line     : /dev/ttyACM0  (Linux) or COM3 (Windows)
Speed           : 115200
Data Bits       : 8
Stop Bits       : 1
Parity          : None
Flow Control    : None
```

### Using Minicom (Linux)
```bash
sudo minicom -D /dev/ttyACM0 -b 115200 --color=on
```

### Using Android Phone
```
Hardware : USB OTG adapter
App      : Serial USB Terminal (Play Store — by Kai Morich)
Baud     : 115200
```

---

## Expected Serial Output
```
========================================
  FreeRTOS Simple Semaphore Demo
  Task-1 Prio:3  Task-2 Prio:2  Task-3 Prio:1
========================================
[Task-1] RUNNING  | Run#1 | Tick:1ms
[Task-1] DONE     | Semaphore given
------------------------------------------
[Task-2] RUNNING  | Run#1 | Tick:327ms
[Task-2] DONE     | Semaphore given
------------------------------------------
[Task-1] RUNNING  | Run#2 | Tick:653ms
[Task-1] DONE     | Semaphore given
------------------------------------------
[Task-2] RUNNING  | Run#2 | Tick:979ms
[Task-2] DONE     | Semaphore given
------------------------------------------
```

> Task-3 never prints — this is expected behavior
> demonstrating task starvation due to lower priority.

---

## FreeRTOS Concepts Demonstrated

| Concept              | Where Used                          |
|----------------------|-------------------------------------|
| Binary Semaphore     | Controls task execution order       |
| Task Priority        | Task-1 always runs before Task-2    |
| Task Starvation      | Task-3 never gets semaphore         |
| vTaskDelay           | Blocking delay inside each task     |
| xSemaphoreTake       | Blocks task until semaphore free    |
| xSemaphoreGive       | Releases semaphore for next task    |
| DWT Cycle Counter    | Used for FreeRTOS runtime stats     |
| UART Communication   | Serial output via HAL               |

---

## Key FreeRTOS APIs Used
```c
xSemaphoreCreateBinary()          /* create binary semaphore    */
xSemaphoreGive(xSemaphore)        /* release semaphore          */
xSemaphoreTake(xSemaphore, ticks) /* acquire semaphore or block */
xTaskCreate(...)                  /* create a task              */
vTaskDelay(pdMS_TO_TICKS(ms))     /* delay in milliseconds      */
xTaskGetTickCount()               /* get current tick count     */
vTaskStartScheduler()             /* start FreeRTOS scheduler   */
```

---

## Common Problems and Fixes

| Problem                  | Cause                        | Fix                              |
|--------------------------|------------------------------|----------------------------------|
| No output in terminal    | Wrong baud rate              | Set exactly 115200               |
| Garbled text             | Flow control enabled         | Set flow control to None         |
| Permission denied        | No access to serial port     | sudo usermod -a -G dialout $USER |
| Task-3 never prints      | Expected — priority too low  | Change priority to 3 to test     |
| Build error              | Missing function definition  | Check forward declarations       |
| Board not detected       | Driver issue                 | Reinstall ST-Link driver         |

---

## Modifying the Code

### Make Task-3 Run
Change priority from 1 to 3:
```c
xTaskCreate(task3_handler, "Task-3", 256, NULL, 3, NULL);
```
Now all tasks have equal priority and take turns.

### Change Task Timing
```c
vTaskDelay(pdMS_TO_TICKS(300));  /* work duration  */
vTaskDelay(pdMS_TO_TICKS(100));  /* gap after give */
```
Reduce gap to 0 and observe Task-3 starvation more clearly.

### Add More Tasks
```c
static void task4_handler(void* parameters)
{
    char     buf[100];
    uint32_t run = 0;
    while(1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        run++;
        snprintf(buf, sizeof(buf),
            "[Task-4] RUNNING | Run#%lu", (unsigned long)run);
        uart_println(buf);
        vTaskDelay(pdMS_TO_TICKS(300));
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

---

## Related Concepts to Study
```
After understanding this project, study:

1. Mutex            — ownership based semaphore
2. Counting Semaphore — multiple resource management
3. Queue            — passing data between tasks
4. Event Groups     — multiple flag synchronization
5. Task Notifications — direct task to task signal
6. Deadlock         — tasks waiting for each other
7. Priority Inversion — high priority blocked by low
8. Stack Overflow   — task using too much stack memory
```

---

## References

| Resource                          | Link                                          |
|-----------------------------------|-----------------------------------------------|
| FreeRTOS Official Documentation   | https://www.freertos.org/Documentation        |
| Mastering FreeRTOS (Free Book)    | https://www.freertos.org/fr-content-sourcecode|
| STM32F401RE Datasheet             | https://www.st.com                            |
| STM32 HAL Driver Guide            | https://www.st.com/stm32cube                  |
| ARM Cortex-M4 Technical Reference | https://developer.arm.com                     |

---

## Author
```
Name    : Naveen
Board   : STM32F401RE Nucleo-64
IDE     : STM32CubeIDE
RTOS    : FreeRTOS
```

---

*Built with STM32 + FreeRTOS for learning embedded systems*
