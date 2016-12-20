/* pid.c */
#include "pid.h"

void PID_Init(PID_Hanlder_T *pid, float kP, float kI, float kD, int limitError)
{
	pid->TargetValue = 0; 	/* 设定目标的期望值 */
	pid->SumError 	 = 0;   /* 误差累积 */
	pid->kP 		 = kP; 	/* 比例常数 Proportional Const */
	pid->kI 		 = kI;  /* 积分常数 Integral Const */
	pid->kD 		 = kD; 	/* 微分常数 Derivative Const */
	pid->LastError 	 = 0;  	/* Error[-1] */
	pid->PrevError 	 = 0;  	/* Error[-2] */
	pid->LimitError  = limitError;
}

void PID_SetTargetValue(PID_Hanlder_T *pid, int targetValue)
{
    pid->TargetValue = targetValue;
}

/* 相对增量式PID，输入当前的绝对值，预测到达目标值得增量 */
int  PID_GetIncCalc(PID_Hanlder_T *pid, int currentValue)
{
    int incError, incVal;

    /* 计算误差 */
    incError = pid->TargetValue - currentValue;
    if (incError > pid->LimitError)
    {
    	incError = pid->LimitError;
    }
    if (incError < -pid->LimitError)
    {
    	incError = -pid->LimitError;
    }
	/* 增量计算 */
	incVal =  pid->kP * incError 			/* E[k]项    */
			- pid->kI * pid->LastError   /* E[k－1]项 */
			+ pid->kD * pid->PrevError;  /* E[k－2]项 */
	/* 存储误差，用于下次计算 */
	pid->PrevError = pid->LastError;
	pid->LastError = incError;
	/* 返回增量值 */
	return incVal;
}

/* 绝对位置式PID: 输入当前的绝对值，预测接近目标值得下一个位置 */
int  PID_GetPosCalc(PID_Hanlder_T *pid, int currentValue)
{
	int incError,dError;
	int iLocVal;

	incError = pid->SetPoint - NextPoint; //偏差
	//增加阀值过滤功能
	if (incError > pid->LimitError)//判断是否超过上限
	{
		incError = pid->LimitError;
	}
	if (incError < -pid->LimitError)//判断是否超过下限
	{
		incError = -pid->LimitError;
	}

	pid->SumError += incError; //积分
	dError = incError - pid->LastError; //微分
	pid->LastError = iError;

	iLocVal = pid->kP * incError//比例项
			+ pid->kI * pid->SumError//积分项
			+ pid->kD * dError;//微分项
	if(iLocVal<0)//确保控制器输出的值为正
	{
		iLocVal=0;
	}

	return  iLocVal;
}

/* EOF. */

