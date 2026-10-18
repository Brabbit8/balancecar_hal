#include "app_led.h"
#include "main.h"

static uint32_t LastTime = 0U;
static uint8_t stage = 0U;

/**
 * @brief 板载LED非阻塞闪烁处理
 * @note 需要在主循环中反复调用
 */
void OnboardLED_proc(void)
{
    uint32_t now = HAL_GetTick();

    if ((stage == 0U) && ((now - LastTime) >= 100U))
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

        stage = 1U;
        LastTime = now;
    }
    else if ((stage == 1U) && ((now - LastTime) >= 100U))
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

        stage = 0U;
        LastTime = now;
    }
}