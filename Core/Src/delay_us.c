#include "delay_us.h"
#include "core_cm3.h"

static uint32_t last_cycle;
static uint64_t total_cycles;
static uint8_t delay_us_initialized = 0;

void DelayUs_Init(void)
{
    if (delay_us_initialized)
    {
        return;
    }

    /*
     * 开启 DWT 和 CYCCNT
     */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    last_cycle = DWT->CYCCNT;
    total_cycles = 0;
    delay_us_initialized = 1;
}

uint64_t GetUs(void)
{
    uint32_t current_cycle;
    uint32_t delta_cycle;

    DelayUs_Init();

    /*
     * DWT->CYCCNT 是 32 位计数器。
     * 两个 uint32_t 相减可以自动处理计数器溢出。
     */
    __disable_irq();

    current_cycle = DWT->CYCCNT;
    delta_cycle = current_cycle - last_cycle;
    last_cycle = current_cycle;
    total_cycles += delta_cycle;

    __enable_irq();

    return (total_cycles * 1000000ULL) / SystemCoreClock;
}

void DelayUs(uint32_t us)
{
    uint64_t start_time;

    start_time = GetUs();

    while ((GetUs() - start_time) < us)
    {
    }
}