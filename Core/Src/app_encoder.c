#include "app_encoder.h"

static volatile int8_t diretion_l = 0; // +/-1、+/-2表示方向和A相阶段
static volatile int8_t diretion_r = 0;
static volatile int8_t last_diretion_l = 0;
static volatile int8_t last_diretion_r = 0;
static volatile int32_t encoder_l = 0; // 左电机累计编码器值
static volatile int32_t encoder_r = 0; // 右电机累计编码器值
static volatile uint32_t t0_l = 0, t1_l = 0; // 左电机最近两次边沿的DWT计数
static volatile uint32_t t0_r = 0, t1_r = 0; // 右电机最近两次边沿的DWT计数

/* 参考UP主公式：每个编码器计数对应的车轮角度，单位rad/count。 */
#define ENCODER_RAD_PER_COUNT 0.0139940220892036059f

static void Encode_L_Init(void);
static void Encode_R_Init(void);

void App_Encoder_Init(void)
{
    encoder_l = 0;
    encoder_r = 0;
    diretion_l = 0;
    diretion_r = 0;
    last_diretion_l = 0;
    last_diretion_r = 0;
    t0_l = 0U;
    t1_l = 0U;
    t0_r = 0U;
    t1_r = 0U;

    /* F103 Cortex-M3 DWT周期计数器用于微秒级边沿计时。 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    Encode_L_Init();
    Encode_R_Init();
}

// 读取左轮累计角度，单位rad。
float App_Encoder_GetPos_L(void)
{
    return (float)encoder_l * ENCODER_RAD_PER_COUNT;
}

// 读取右轮累计角度，按UP主代码保留右轮反向符号，单位rad。
float App_Encoder_GetPos_R(void)
{
    return -(float)encoder_r * ENCODER_RAD_PER_COUNT;
}

// GPIO和EXTI由CubeMX配置。
static void Encode_L_Init(void)
{
}

static void Encode_R_Init(void)
{
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_3)
    {
        uint8_t a = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
        uint8_t b = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
        uint32_t now = DWT->CYCCNT;

        t1_r = t0_r;
        t0_r = now;
        last_diretion_r = diretion_r;

        if (a == GPIO_PIN_SET) // A相上升沿
        {
            if (b == GPIO_PIN_SET) // 正转阶段2
            {
                diretion_r = 2;
                encoder_r++;
            }
            else // 反转阶段-2
            {
                diretion_r = -2;
                encoder_r--;
            }
        }
        else // A相下降沿
        {
            if (b == GPIO_PIN_RESET) // 正转阶段1
            {
                diretion_r = 1;
                encoder_r++;
            }
            else // 反转阶段-1
            {
                diretion_r = -1;
                encoder_r--;
            }
        }
    }
    else if (GPIO_Pin == GPIO_PIN_14)
    {
        uint8_t a = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
        uint8_t b = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
        uint32_t now = DWT->CYCCNT;

        t1_l = t0_l;
        t0_l = now;
        last_diretion_l = diretion_l;

        if (a == GPIO_PIN_SET) // A相上升沿
        {
            if (b == GPIO_PIN_SET) // 正转阶段2
            {
                diretion_l = 2;
                encoder_l++;
            }
            else // 反转阶段-2
            {
                diretion_l = -2;
                encoder_l--;
            }
        }
        else // A相下降沿
        {
            if (b == GPIO_PIN_RESET) // 正转阶段1
            {
                diretion_l = 1;
                encoder_l++;
            }
            else // 反转阶段-1
            {
                diretion_l = -1;
                encoder_l--;
            }
        }
    }
}

static float Encoder_GetSpeed(int8_t direction,
                              int8_t previous_direction,
                              uint32_t edge_time,
                              uint32_t previous_edge_time,
                              float wheel_sign)
{
    /* 初次采样尚无完整边沿周期；方向刚反转时暂报0。 */
    if ((direction == 0) || (previous_direction == 0) ||
        ((int16_t)direction * (int16_t)previous_direction <= 0))
    {
        return 0.0f;
    }

    uint32_t previous_period = edge_time - previous_edge_time;
    if ((previous_period == 0U) || (SystemCoreClock == 0U))
    {
        return 0.0f;
    }

    /* T法：最近边沿周期决定速度。若边沿迟迟未到，按减速趋势估算，
     * 超过两个周期仍无新边沿则视为车轮停止，速度归零。
     */
    uint32_t current_period = DWT->CYCCNT - edge_time;
    if ((uint64_t)current_period > (uint64_t)previous_period * 2U)
    {
        return 0.0f;
    }

    uint32_t period = (current_period > previous_period)
                    ? current_period : previous_period;
    float sign = (direction > 0) ? 1.0f : -1.0f;

    return wheel_sign * sign * ENCODER_RAD_PER_COUNT
           * (float)SystemCoreClock / (float)period;
}

// 读取左轮角速度，单位rad/s。
float App_Encoder_GetSpeed_L(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    int8_t d0 = diretion_l;
    int8_t d1 = last_diretion_l;
    uint32_t t0 = t0_l;
    uint32_t t1 = t1_l;
    __set_PRIMASK(primask);

    return Encoder_GetSpeed(d0, d1, t0, t1, 1.0f);
}

// 读取右轮角速度，保留UP主代码中的右轮反向符号，单位rad/s。
float App_Encoder_GetSpeed_R(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    int8_t d0 = diretion_r;
    int8_t d1 = last_diretion_r;
    uint32_t t0 = t0_r;
    uint32_t t1 = t1_r;
    __set_PRIMASK(primask);

    return Encoder_GetSpeed(d0, d1, t0, t1, -1.0f);
}
