#include "test.h"
#include "app_bat.h"
#include "usart.h"

#include <stdio.h>

void Test_Init(void)
{
    if (App_Bat_Init() != HAL_OK)
    {
        Error_Handler();
    }

    /* 等待第一次 TIM2 触发采样，避免上电时立即输出初始值 0。 */
    HAL_Delay(1U);
}

void Test_Loop(void)
{
    char buffer[32];
    uint32_t millivolts;
    int length;

    millivolts = App_Bat_GetMillivolts();

    /* 用整数格式输出伏特值，如 6.697；兼容 VOFA FireWater。 */
    length = snprintf(
        buffer,
        sizeof(buffer),
        "%lu.%03lu\n",
        (unsigned long)(millivolts / 1000U),
        (unsigned long)(millivolts % 1000U)
    );

    if ((length > 0) && ((unsigned int)length < sizeof(buffer)))
    {
        HAL_UART_Transmit(
            &huart2,
            (uint8_t *)buffer,
            (uint16_t)length,
            100
        );
    }

    HAL_Delay(100);
}
