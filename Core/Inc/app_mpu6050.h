#ifndef APP_MPU6050_H
#define APP_MPU6050_H

#include "stm32f1xx_hal.h"
void App_Mpu6050_Init(void);
void App_MPU6050_Proc(void);
void App_Mpu6050_Update(void);
float App_MPU6050_GetAX(void);
float App_MPU6050_GetAY(void);
float App_MPU6050_GetAZ(void);

float App_MPU6050_GetGX(void);
float App_MPU6050_GetGY(void);
float App_MPU6050_GetGZ(void);

float App_MPU6050_Getyaw(void);
float App_MPU6050_Getpitch(void);
float App_MPU6050_Getroll(void);
float App_MPU6050_GetTemperature(void);
#endif