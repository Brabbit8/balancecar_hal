#include "app_mpu6050.h"
#include "i2c.h"
#include "qmath.h"
static float ax,ay,az;
static float gx,gy,gz;
static float temperature;
static float yaw,pitch,roll;//欧拉角,单位是度
//
//简介:对面mpu6050初始化
//

static void reg_write(uint8_t reg,uint8_t value);//写寄存器
static uint8_t reg_read(uint8_t reg);//读寄存器
void App_Mpu6050_Init(void)
{
	//设置mpu6050的参数
	reg_write(0x6b,0x80);
	HAL_Delay(100);
	//从休眠模式唤醒
	reg_write(0x6b,0x00);
	//选择量程为+-2000°/s
	reg_write(0x1b,0x18);
	//将加速度的量程设置为+-2
	reg_write(0x1c,0x00);
}
//更新6050的值
void App_Mpu6050_Update(void)
{
	int16_t ax_raw=(int16_t)((reg_read(0x3b)<<8)+reg_read(0x3c));
	int16_t ay_raw=(int16_t)((reg_read(0x3d)<<8)+reg_read(0x3e));
	int16_t az_raw=(int16_t)((reg_read(0x3f)<<8)+reg_read(0x40));
	ax=ax_raw*6.1035e-5f;
	ay=ay_raw*6.1035e-5f;
	az=az_raw*6.1035e-5f;
	int16_t temperature_raw =(int16_t)((reg_read(0x41)<<8)+reg_read(0x42));//温度计
	int16_t gx_raw=(int16_t)((reg_read(0x43)<<8)+reg_read(0x44));
	int16_t gy_raw=(int16_t)((reg_read(0x45)<<8)+reg_read(0x46));
	int16_t gz_raw=(int16_t)((reg_read(0x47)<<8)+reg_read(0x48));	
	gx=gx_raw*6.1035e-2f;
	gy=gy_raw*6.1035e-2f;
	gz=gz_raw*6.1035e-2f;
	temperature=temperature_raw/333.87f+21.0f;
}
float App_MPU6050_GetAX(void)
{
	return ax;
}
float App_MPU6050_GetAY(void)
{
	return ay;
}
float App_MPU6050_GetAZ(void)
{
	return az;
}

float App_MPU6050_GetGX(void)
{
	return gx;
}
float App_MPU6050_GetGY(void)
{
	return gy;
}	

float App_MPU6050_GetGZ(void)
{
	return gz;
}

float App_MPU6050_Getyaw(void)
{	
	return yaw;
}
float App_MPU6050_Getpitch(void)
{
	return pitch;
}
float App_MPU6050_Getroll(void)
{
	return roll;
}
float App_MPU6050_GetTemperature(void)
{
	return temperature;
}	

static void reg_write(uint8_t reg,uint8_t value)
{
	uint8_t bytesTosend[]={reg,value};
	HAL_I2C_Master_Transmit(&hi2c1,0xd0,bytesTosend,2,HAL_MAX_DELAY);
}

static uint8_t reg_read(uint8_t reg)
{
	
	HAL_I2C_Master_Transmit(&hi2c1,0xd0,&reg,1,HAL_MAX_DELAY);
	uint8_t regvalue ;
	HAL_I2C_Master_Receive(&hi2c1,0xd0,&regvalue,1,HAL_MAX_DELAY);
	return regvalue;
}
//进程函数,计算欧拉角
void App_MPU6050_Proc(void)
{
	static uint32_t nxt=0;
	if(HAL_GetTick()<nxt)
	{
		return;
	}
	App_Mpu6050_Update();
	//通过加速度计算
	float pitch_a = QMath_Atan2Deg(ay, az);
	float roll_a = QMath_Atan2Deg(ax, az);
	//通过陀螺仪
	float yaw_g=yaw+gz*0.005;
	float pitch_g=pitch+gx*0.005;
	float roll_g=roll-gy*0.005;
	//是用互补滤波器
	yaw=yaw_g;
	pitch=0.95238*pitch_g+(1-0.95238)*pitch_a;
	roll=0.95238*roll_g+(1-0.95238)*roll_a;
	nxt+=5;
}
