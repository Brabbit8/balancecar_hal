#include "mpu6050_test.h"
#include "usart.h"
#include "app_mpu6050.h"
#include <stdio.h>
void MPU6050_Test(void)
{
	static const uint8_t before[] = "before MPU init\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)before, sizeof(before) - 1, 100);
	App_Mpu6050_Init();
	static const uint8_t after[] = "after MPU init\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)after, sizeof(after) - 1, 100);
	while(1)
	{
		App_Mpu6050_Update();
		float ax=App_MPU6050_GetAX();
		float ay=App_MPU6050_GetAY();
		float az=App_MPU6050_GetAZ();
		
		float temperature =App_MPU6050_GetTemperature();
		float gx=App_MPU6050_GetGX();
		float gy=App_MPU6050_GetGY();
		float gz=App_MPU6050_GetGZ();
		
		
		char tx_buf[128];
    int len = snprintf(tx_buf, sizeof(tx_buf),"%.3f,%.3f,%.3f,%.2f,%.3f,%.3f,%.3f\r\n",ax, ay, az, temperature, gx, gy, gz);
    if (len > 0 && len < (int)sizeof(tx_buf))
    {
       HAL_UART_Transmit(&huart2, (uint8_t *)tx_buf,(uint16_t)len, 100);
    }

     HAL_Delay(10);
	}
}

static void uart2_Proc(void)	
{
	static uint32_t nxt=0;
	if(HAL_GetTick()<nxt)
	{
		return;
	}
	char tx_buf[128];
	float ax=App_MPU6050_GetAX();
	float ay=App_MPU6050_GetAY();
	float az=App_MPU6050_GetAZ();
	float temperature=App_MPU6050_GetTemperature();
	float gx=App_MPU6050_GetGX();
	float gy=App_MPU6050_GetGY();
	float gz=App_MPU6050_GetGZ();
	float yaw=App_MPU6050_Getyaw();
	float pitch=App_MPU6050_Getpitch();
	float roll=App_MPU6050_Getroll();
  int len = snprintf(tx_buf, sizeof(tx_buf),"%.3f,%.3f,%.3f,%.2f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",ax, ay, az, temperature, gx, gy, gz,yaw,pitch,roll);
  if (len > 0 && len < (int)sizeof(tx_buf))
  {
       HAL_UART_Transmit(&huart2, (uint8_t *)tx_buf,(uint16_t)len, 100);
  }
	nxt+=5;
}

//用来测试欧拉角
void MPU6050_EularAngleTest(void)
{	
	App_Mpu6050_Init();
	while(1)
	{
		App_MPU6050_Proc();
		uart2_Proc();
	}
}