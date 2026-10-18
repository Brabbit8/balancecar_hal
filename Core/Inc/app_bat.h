#ifndef APP_BAT_H
#define APP_BAT_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

/* 仅当 ADC 满量程对应电池 8.4 V 时使用；按实际分压电路修改。 */
#define APP_BAT_FULL_SCALE_MV 8400U

HAL_StatusTypeDef App_Bat_Init(void);//初始化电池采样
uint16_t App_Bat_GetRaw(void);//获取采样数据
uint32_t App_Bat_GetMillivolts(void);//获取电池电压，单位 mV
float App_Bat_GetVoltage(void);//把采样数据转换成电压

#endif
