//adc和tim2开始工作
//tim2触发adc采样
//采样完成,进入中断
//hal调用回调函数,保存采样值
//转换函数把采样值转换成电压


//
//简介@对adc测电压模块的函数的初始化
//
#include "app_bat.h"
#include "adc.h"//获取句柄
#include "tim.h"//获取句柄

static volatile uint16_t battery_adc_raw;//全局变量电压

HAL_StatusTypeDef App_Bat_Init(void)
{
    HAL_StatusTypeDef status;

    battery_adc_raw = 0U;

    /* 校准后使能注入组中断，再启动 TIM2 的周期触发。 */
    status = HAL_ADCEx_Calibration_Start(&hadc1);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_ADCEx_InjectedStart_IT(&hadc1);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_Base_Start(&htim2);
    if (status != HAL_OK)
    {
        (void)HAL_ADCEx_InjectedStop_IT(&hadc1);
    }

    return status;
}
//中断响应函数
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        battery_adc_raw = (uint16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);

        /* 本工程的 F1 HAL 会在回调前关闭 JEOC 中断。
         * 仅在中断被关闭时重新使能，继续等待下一次 TIM2 外部触发。
         * 当前配置为外部触发，此调用不会立即软件触发新的转换。
         */
        if (__HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_JEOC) == RESET)
        {
            (void)HAL_ADCEx_InjectedStart_IT(hadc);
        }
    }
}
//@简介:采样值
uint16_t App_Bat_GetRaw(void)
{
    return battery_adc_raw;
}
/* 读取最近一次结果并换算成毫伏，四舍五入到 1 mV。 */
uint32_t App_Bat_GetMillivolts(void)
{
    uint32_t raw = App_Bat_GetRaw();

    return (raw * APP_BAT_FULL_SCALE_MV + 2047U) / 4095U;
}

//@简介:当前电池电压，单位 V
float App_Bat_GetVoltage(void)
{
    return (float)App_Bat_GetRaw() * (float)APP_BAT_FULL_SCALE_MV
           / (4095.0f * 1000.0f);
}
