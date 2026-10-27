#include "app_control.h"
#include "pid.h"
#include "task.h"
#include "app_mpu6050.h"
#include "math.h"
#include "app_motor.h"
#include "delay_us.h"
static PID_Typedef pid_theta;
static PID_Typedef pid_theta_dot;
static const float g=9.8f;//重力加速度
static const float lp=0.062f;//;轮胎中心到电池中心的距离
static const float rw=0.032f;//轮胎半径
//负责初始化


void App_Control_Init(void)
{
	PID_Init(&pid_theta,4.0f,0.0f,0.0f);
	PID_Limitconfig(&pid_theta,+12.57f,-12.57f);//将θ环限制
	PID_Init(&pid_theta_dot,10.0f,10.0f,0.0f);
	PID_Limitconfig(&pid_theta_dot,+125.7f,-125.7f);//将角加速度限制
}

static float omega_ref=0.0f;
static uint64_t lasttime=0.0f;
//进程函数
void App_Control_Proc(void)
{
	uint64_t now=GetUs();
	if (lasttime == 0U)
	{
    lasttime = now;
    return;
	}
	float deltaT = (float)(now - lasttime) * 1.0e-6f;
	//最低限度为五毫秒执行一次
	PERIODIC(5);
	//保持静止要把角度设为垂直,就是0
	PID_ChangeSP(&pid_theta,0.0f);
	//读取传感器的数据,
	float theta = App_MPU6050_Getpitch()*0.174533f;
	float theta_dot =App_MPU6050_GetGX()*0.174533f;
	
	//计算外环pid的输出
	float theta_dot_ref=PID_Compute(&pid_theta,theta);
	//改变内环的设定值
	PID_ChangeSP(&pid_theta_dot,theta_dot_ref);
	//计算内环输出
	float theta_dot_dot_ref=PID_Compute(&pid_theta_dot,theta_dot);
	//计算x..
	float x_dot_dot_ref=(g*sin(theta)-theta_dot_dot_ref*lp)/cos(theta);
	//计算积分
	omega_ref+=1.0f/rw*x_dot_dot_ref*deltaT;
	App_Motor_SetOmega_L(omega_ref);
	App_Motor_SetOmega_R(omega_ref);
	//设置轮胎转速
	lasttime=now;
}