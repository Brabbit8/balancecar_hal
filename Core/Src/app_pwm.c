#include "app_pwm.h"
#include "tim.h"
#include "math.h"
static void Motor_L_Init(void);
static void Motor_R_Init(void);
void App_PWM_Set_L(float Duty);
void App_PWM_Set_R(float Duty);

//
//@简介对tb6612初始化
//
void App_PWM_Init(void)
{
	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0U);

    App_PWM_Cmd(0U);
}
//
//@简介:原来的视频里有对引脚初始化的部分,在hal库中由cubemx提供
//

//
//@简介:控制启动或者关闭 
//@参数:on  0关闭  非零开启 
void App_PWM_Cmd(uint8_t on)
{
	if(on==0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//休眠
	}
	if(on==1)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);//活动
	}
}
//简介,左电机占空比
void App_PWM_Set_L(float Duty)
{
	float sign;//符号
	if(Duty>=0) sign=1;
	else sign=-1;
	Duty=fabsf(Duty);
	
	if(sign>0)//正转
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
	}
	else//反转
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);
	}
	uint16_t ccr=Duty/100.0f*999;//算出ccr
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,ccr);//把ccr写回寄存器
}
//简介,右电机占空比
void App_PWM_Set_R(float Duty)
{
	float sign;//符号
	if(Duty>=0) sign=1;
	else sign=-1;
	Duty=fabsf(Duty);
	
	if(sign>0)//正转
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	else//反转
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
	}
	uint16_t ccr=Duty/100.0f*999;//算出ccr
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,ccr);//把ccr写回寄存器
}

//
//原视频在此有对电机引脚初始化的代码,此为cubemx自动配置
//@参数:左电机为ain1-pa9-out_pp   ain2-pa10-out_pp   右电机bin1-pb5-out_pp  bin2-pb7-out_pp
static void Motor_L_Init(void)
{
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
}

static void Motor_R_Init(void)
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
}

