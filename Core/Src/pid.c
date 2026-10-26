#include "pid.h"
#include "delay_us.h"
//对pid控制器进行初始化
void PID_Init(PID_Typedef *PID,float kp,float ki,float kd)
{
	PID->kp=kp;
	PID->ki=ki;
	PID->kd=kd;
	PID->sp=0.0f;
	PID->t_k_1=0;
	PID->err_k_1=0.0f;
	PID->err_int_k_1=0.0f;
	PID->uppderlimit=3.4e+38f;
	PID->lowerlimit=-3.4e+38f;
}

//改变设定值
void PID_ChangeSP(PID_Typedef *pid,float sp)
{
	pid->sp=sp;
}
//执行一次pid运算
//参数,反馈值
//返回值pid控制器计算的结果
float PID_Compute(PID_Typedef *pid,float FB)
{
	uint64_t t_k=GetUs();
	float deltaT=(t_k-pid->t_k_1)*1.0e-6f;
	float err=pid->sp - FB;
	float err_dev=0.0f;
	float err_int=0.0f;
	if(pid->t_k_1!=0)
	{
	err_dev=(err-pid->err_k_1)/deltaT;
	err_int=pid->err_int_k_1+(err+pid->err_k_1)*deltaT*0.5f;
	}
	float COp=pid->kp*err;
	float COi=pid->ki * err_int;
	float COd=pid->kd*err_dev;
	float CO=COp+COi+COd;
	//更新
	pid->t_k_1=t_k;
	pid->err_k_1=err;
	pid->err_int_k_1=err_int;
	if(CO>pid->uppderlimit)
	{
		CO=pid->uppderlimit;
	}
	if(CO<pid->lowerlimit)
	{
		CO=pid->lowerlimit;
	}
	if(pid->err_int_k_1>pid->uppderlimit)pid->err_int_k_1=pid->uppderlimit;
	if(pid->err_int_k_1<pid->lowerlimit)pid->err_int_k_1=pid->lowerlimit;
	return CO;
}
//设置pid控制器门限
void PID_Limitconfig(PID_Typedef *pid,float upperlimit,float lowerlimit)
{
	pid->uppderlimit=upperlimit;
	pid->lowerlimit=lowerlimit;
}
//对pid控制器进行复位
void PID_Reset(PID_Typedef * pid)
{
	pid->t_k_1=0;
	pid->err_k_1=0.0f;
	pid->err_int_k_1=0.0f;
}