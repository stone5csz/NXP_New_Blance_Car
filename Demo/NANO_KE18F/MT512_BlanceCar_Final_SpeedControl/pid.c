///* pid.c */
//#include "pid.h"

//extern volatile float MPU6050_GroyZ;
//extern volatile uint32_t LeftEIDetectVal, RightEIDetecVal;
//extern volatile float velocity_kp, velocity_lowpass, velocity_lowpass_least;
//extern volatile int8_t pitch, roll;
//extern volatile float navigation_turn_kp, navigation_turn_kd;
//float velocity, encoder_least, encoder=0;
//float encoder_intergral=0;


//void PID_Init(PID_Hanlder_T *pid, float kP, float kI, float kD, float limitError)
//{
//    pid->TargetValue = 0;   /* 设定目标的期望值 */
//    pid->SumError    = 0;   /* 误差累积 */
//    pid->kP          = kP;  /* 比例常数 Proportional Const */
//    pid->kI          = kI;  /* 积分常数 Integral Const */
//    pid->kD          = kD;  /* 微分常数 Derivative Const */
//    pid->LastError   = 0;   /* Error[-1] */
//    pid->PrevError   = 0;   /* Error[-2] */
//    pid->LimitError  = limitError;
//}

//void PID_SetTargetValue(PID_Hanlder_T *pid, int targetValue)
//{
//    pid->TargetValue = targetValue;
//}

///* 相对增量式PID，输入当前的绝对值，预测到达目标值得增量 */
//int  PID_GetIncCalc(PID_Hanlder_T *pid, int currentValue)
//{
//    int incError, incVal;

//    /* 计算误差 */
//    incError = pid->TargetValue - currentValue;
//    if (incError > pid->LimitError)
//    {
//        incError = pid->LimitError;
//    }
//    if (incError < -pid->LimitError)
//    {
//        incError = -pid->LimitError;
//    }
//    /* 增量计算 */
//    incVal =  pid->kP * incError         /* E[k]项    */
//            - pid->kI * pid->LastError   /* E[k－1]项 */
//            + pid->kD * pid->PrevError;  /* E[k－2]项 */
//    /* 存储误差，用于下次计算 */
//    pid->PrevError = pid->LastError;
//    pid->LastError = incError;
//    /* 返回增量值 */
//    return incVal;
//}

//int  PID_GetIncCalc_new(PID_Hanlder_T *pid, int currentValue)
//{
//    float incError=0.0, incVal=0.0;

//    /* 计算误差 */
//    incError = pid->TargetValue - currentValue;
//    if (incError > pid->LimitError)
//    {
//        incError = pid->LimitError;
//    }
//    if (incError < -pid->LimitError)
//    {
//        incError = -pid->LimitError;
//    }
//    /* 增量计算 */
//    incVal =  pid->kP * (incError - pid->LastError)        /* E[k]项    */
//            + pid->kI * incError   /* E[k－1]项 */
//            + pid->kD * (incError - 2*pid->LastError + pid->PrevError);  /* E[k－2]项 */
//    /* 存储误差，用于下次计算 */
//    pid->PrevError = pid->LastError;
//    pid->LastError = incError;
//    /* 返回增量值 */
//    return incVal;
//}


//#if 0
///* 绝对位置式PID: 输入当前的绝对值，预测接近目标值得下一个位置 */
//int  PID_GetPosCalc(PID_Hanlder_T *pid, int currentValue)
//{
//    int incError,dError;
//    int iLocVal;

//    incError = pid->SetPoint - NextPoint; //偏差
//    //增加阀值过滤功能
//    if (incError > pid->LimitError)//判断是否超过上限
//    {
//        incError = pid->LimitError;
//    }
//    if (incError < -pid->LimitError)//判断是否超过下限
//    {
//        incError = -pid->LimitError;
//    }

//    pid->SumError += incError; //积分
//    dError = incError - pid->LastError; //微分
//    pid->LastError = iError;

//    iLocVal = pid->kP * incError//比例项
//            + pid->kI * pid->SumError//积分项
//            + pid->kD * dError;//微分项
//    if (iLocVal<0) //确保控制器输出的值为正
//    {
//        iLocVal=0;
//    }

//    return  iLocVal;
//}
//#endif

//int PI_Velocity(int encoder_left, int encoder_right, float movement)
//{
////		static float velocity, encoder_least, encoder=0;
////		static float encoder_intergral=0;
//		//float kp=280, ki=kp/200;
//		//float kp=150, ki=kp/200;
//		float kp=velocity_kp, ki=kp/200;  //390
//		
//		encoder_least = encoder_left + encoder_right - 0;
////		encoder *= 0.72;
////		encoder *= 0.7;
//		encoder *= velocity_lowpass;
////		encoder += encoder_least * 0.7;
////		encoder += encoder_least * 0.45;
//		encoder += encoder_least * velocity_lowpass_least;
//		encoder_intergral += encoder;
//		encoder_intergral -= movement;
//		if(encoder_intergral > 15000) encoder_intergral = 15000;
//		if(encoder_intergral < -15000) encoder_intergral = -15000;
//		velocity = encoder*kp + encoder_intergral*ki;
//		return velocity;	
//}

//int PD_Navigation_Turn(void)
//{
//		static float ysub, yadd;
//		static float lefteid, righteid;
////		static float kp=5000.0, kd=16.0;
////		static float kp=7000.0, kd=20.0;
//		float kp=navigation_turn_kp, kd=navigation_turn_kd;
//		static float turn;
//		lefteid = LeftEIDetectVal - LEFT_EDI_OFFSET;
//		righteid = RightEIDetecVal - RIGHT_EDI_OFFSET;
//		ysub = lefteid - righteid;
//		yadd = righteid + lefteid;
//		turn = (ysub/yadd)*kp + MPU6050_GroyZ*kd;
//		return turn;	

////		float kp=10, ki=kp/200, kd=0.005;
////		static int bias, turn=0, last_bias=0, prev_bias=0;
////		bias = MPU6050_GroyZ-0;
//////		turn += kp*(bias-last_bias) + ki*bias + kd*(bias - 2*last_bias + prev_bias);
////		turn += kp*(bias-last_bias) + ki*bias;
////		last_bias=bias;
////		prev_bias=last_bias;
////		return turn;
//			
//	
//}

//int PD_Remote_Turn(void)
//{
//		static float kp=130.0, kd=16.0;
//		static float turn;
//		if(abs(roll) > 10) {
//			turn = kp*(roll-10) + MPU6050_GroyZ*kd;
//		}else{
//			turn = 0;
//		}
//		return turn;	
//}


///* EOF. */

