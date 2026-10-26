#include "app_motor.h"
#include "pid.h"
#include "task.h"
#include "app_encoder.h"
#include "app_bat.h"
#include "app_pwm.h"
static PID_Typedef pid_motor_l;
static PID_Typedef pid_motor_r;
void App_Motor_Init(void)
{
	PID_Init(&pid_motor_l,0.5,7,0);
	PID_Limitconfig(&pid_motor_l,+8.4f,-8.4f);
	PID_Init(&pid_motor_r,0.5,7,0);
	PID_Limitconfig(&pid_motor_r,+8.4f,-8.4f);
}

//电机调速系统进程函数

void APP_Motor_Proc(void)
{
	PERIODIC(1);//每间隔一毫秒执行一次
	//获取左右电机的角速度
	float omega_l=App_Encoder_GetSpeed_L();
	float omega_r=App_Encoder_GetSpeed_R();
	//计算pid控制器的输出
	float ua_l= PID_Compute(&pid_motor_l,omega_l);
	float ua_r= PID_Compute(&pid_motor_r,omega_r);

	//将电压ua设置到电压两端
	float vbat =App_Bat_GetVoltage();
	float duty_l=ua_l/vbat*100.0f;
	float duty_r=ua_r/vbat*100.0f;

	App_PWM_Set_L(duty_l);
	App_PWM_Set_R(duty_r);
}
//用户对左电机进行调速
void App_Motor_SetOmega_L(float omega)
{
	PID_ChangeSP(&pid_motor_l,omega);
}
//用户对右电机进行调速
void App_Motor_SetOmega_R(float omega)
{
	PID_ChangeSP(&pid_motor_r,omega);
}
//控制电机开关
void App_Motor_Cmd(uint8_t on)
{
	App_PWM_Cmd(on);
	PID_Reset(&pid_motor_l);
	PID_Reset(&pid_motor_r);
}
