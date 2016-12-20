/* pid.h */
#ifndef __PID_H__
#define __PID_H__

/*
 * PID主要用于惯性系的控制系统。我们当然能直接算出当前值与期望值的差距，并且一步
 * 到位，但是由于惯性，到达指定位置时会刹不住车，所以不能一次用例过猛。
 */
#include <stdint.h>

typedef struct
{
	int    TargetValue; /* 设定目标的期望值 */
	int    SumError;    /* 误差累积 */
	float  kP; 			/* 比例常数 Proportional Const */
	float  kI;   		/* 积分常数 Integral Const */
	float  kD; 			/* 微分常数 Derivative Const */
	int    LastError;  	/* Error[-1] */
	int    PrevError;  	/* Error[-2] */
	int    LimitError;  /* 调整限制偏差 */
} PID_Hanlder_T;

void PID_Init(PID_Hanlder_T *pid, float kP, float kI, float kD, int limitError);
void PID_SetTargetValue(PID_Hanlder_T *pid, int targetValue);

/* 相对增量式PID，输入当前的速度值，预测到达目标速度增量 */
int  PID_GetIncCalc(PID_Hanlder_T *pid, int currentValue);

/* 绝对位置式PID: 输入当前的位置值，预测接近目标值的下一个位置 */
int  PID_GetPosCalc(PID_Hanlder_T *pid, int currentValue);

#endif /* __PID_H__ */
