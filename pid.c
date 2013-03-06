#include "pid.h"

struct PID pid[PID_CHANNELS];
//----------------------------------------------------------------------------
float PID_Step(struct PID *n_pid,float purpose,float real)//функция делает шаг ПИД-алгоритма
{
	float result;

	n_pid->err=purpose-real;//вычисляем ошибку регулирования
	
	if(n_pid->effect>=n_pid->min && n_pid->effect<=n_pid->max)//если управляющее воздействие в рамках, то прибавляем ошибку к переменной интегрирования
	{
		if(n_pid->integral>=n_pid->imin && n_pid->integral<=n_pid->imax)
		{
			n_pid->integral+=n_pid->err;
		}
	}

	result=n_pid->Kp*n_pid->err+n_pid->Ki*n_pid->integral*n_pid->timestep-n_pid->Kd*(n_pid->err-n_pid->pre_err)/n_pid->timestep; //П+И+Д



	if(result<n_pid->min || result>n_pid->max)
	{
		if(result<n_pid->min)
		{
			result=n_pid->min;
		}

		if(result>n_pid->max)
		{
			result=n_pid->max;
		}
	}
	n_pid->effect=result;
	n_pid->pre_err=n_pid->err;

	return result;
}
//----------------------------------------------------------------------------
void PID_Init(void)//функция инициализации ПИД-регулятора
{
	pid[0].Kp=0;
	pid[0].Kd=0;
	pid[0].Ki=0;
	pid[0].err=0;
	pid[0].pre_err=0;
	pid[0].integral=0;

	pid[0].min=0.0;
	pid[0].max=255.0;

	pid[0].imin=-40.0;
	pid[0].imax=50.0;

	pid[0].effect=0;

	pid[0].timestep=1;

	return;
}
//----------------------------------------------------------------------------
