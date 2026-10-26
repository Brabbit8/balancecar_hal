#ifndef PID_H
#define PID_H

#include "stm32f1xx_hal.h"

typedef struct
{
	float kp;//比例系数
	float ki;//积分项系数
	float kd;//微分项系数
	float sp;//用户设定值
	uint64_t t_k_1;//t[k-1],上次运行pid的时间
	float err_k_1;//err[k-1],上次运行pid的的误差
	float err_int_k_1;//err_int[k-1],上次运行的积分制;
	float uppderlimit;
	float lowerlimit;
}PID_Typedef;
void PID_Init(PID_Typedef *PID,float kp,float ki,float kd);
void PID_ChangeSP(PID_Typedef *pid,float sp);
float PID_Compute(PID_Typedef *pid,float FB);
void PID_Limitconfig(PID_Typedef *pid, float upperlimit,float lowerlimit);
void PID_Reset(PID_Typedef * pid);
#endif
