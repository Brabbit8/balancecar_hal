#ifndef APP_ENCODER_H
#define APP_ENCODER_H
#include "stm32f1xx_hal.h"
/* 位置单位：rad；速度单位：rad/s。 */
float App_Encoder_GetPos_L(void);
float App_Encoder_GetPos_R(void);
void App_Encoder_Init(void);
float App_Encoder_GetSpeed_L(void);
float App_Encoder_GetSpeed_R(void);
#endif
