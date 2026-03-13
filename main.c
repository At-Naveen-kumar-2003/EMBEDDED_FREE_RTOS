#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_Init(void);
void uart_print(const char* msg);
void uart_println(const char* msg);

UART_HandleTypeDef huart2;
SemaphoreHandle_t  xSemaphore = NULL;

static void task1_handler(void* parameters);
static void task2_handler(void* parameters);
static void task3_handler(void* parameters);

void uart_print(const char* msg)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void uart_println(const char* msg)
{
    uart_print(msg);
    uart_print("\r\n");
}

void vConfigureTimerForRunTimeStats(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t ulGetRunTimeCounterValue(void)
{
    return DWT->CYCCNT;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_Init();

    uart_println("========================================");
    uart_println("  FreeRTOS Simple Semaphore Demo");
    uart_println("  Task-1 Prio:3  Task-2 Prio:2  Task-3 Prio:1");
    uart_println("========================================");

    xSemaphore = xSemaphoreCreateBinary();
    configASSERT(xSemaphore != NULL);
    xSemaphoreGive(xSemaphore);

    xTaskCreate(task1_handler, "Task-1", 256, NULL, 3, NULL);
    xTaskCreate(task2_handler, "Task-2", 256, NULL, 2, NULL);
    xTaskCreate(task3_handler, "Task-3", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while(1) {}
}

static void task1_handler(void* parameters)
{
    char     buf[100];
    uint32_t run = 0;

    while(1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        run++;
        snprintf(buf, sizeof(buf),
            "[Task-1] RUNNING  | Run#%lu | Tick:%lums",
            (unsigned long)run,
            (unsigned long)xTaskGetTickCount());
        uart_println(buf);

        vTaskDelay(pdMS_TO_TICKS(300));

        xSemaphoreGive(xSemaphore);
        uart_println("[Task-1] DONE     | Semaphore given");
        uart_println("------------------------------------------");

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void task2_handler(void* parameters)
{
    char     buf[100];
    uint32_t run = 0;

    while(1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        run++;
        snprintf(buf, sizeof(buf),
            "[Task-2] RUNNING  | Run#%lu | Tick:%lums",
            (unsigned long)run,
            (unsigned long)xTaskGetTickCount());
        uart_println(buf);

        vTaskDelay(pdMS_TO_TICKS(300));

        xSemaphoreGive(xSemaphore);
        uart_println("[Task-2] DONE     | Semaphore given");
        uart_println("------------------------------------------");

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void task3_handler(void* parameters)
{
    char     buf[100];
    uint32_t run = 0;

    while(1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        run++;
        snprintf(buf, sizeof(buf),
            "[Task-3] RUNNING  | Run#%lu | Tick:%lums",
            (unsigned long)run,
            (unsigned long)xTaskGetTickCount());
        uart_println(buf);

        vTaskDelay(pdMS_TO_TICKS(300));

        xSemaphoreGive(xSemaphore);
        uart_println("[Task-3] DONE     | Semaphore given");
        uart_println("------------------------------------------");

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void MX_USART2_Init(void)
{
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
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
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

void Error_Handler(void)
{
    __disable_irq();
    while(1) {}
}
