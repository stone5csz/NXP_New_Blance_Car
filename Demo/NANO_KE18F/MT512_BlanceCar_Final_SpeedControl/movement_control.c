/* car_control.c */

#include "app_inc.h"

volatile int32_t gLeftPwm_Ctrl=0, gRightPwm_Ctrl=0;
volatile int32_t gLeftPwm_Ctrl_test=0, gRightPwm_Ctrl_test=0;
volatile int32_t gLeftEnc_Sum=0, gRightEnc_Sum=0, BothEnc_Sum=0;
volatile uint32_t LeftEID_Measure_Val=0, RightEID_Measure_Val=0;
volatile uint32_t LeftEID_Measure_Sum=0, RightEID_Measure_Sum=0;
volatile float Target_Velocity=0.0;
volatile float VelocityPWM_Out_Old=0.0, VelocityPWM_Out_New=0.0;
volatile float Velocity_Integral;
volatile float DirectionPWM_Out_Old=0, DirectionPWM_Out_New=0, Direction_Diff=0;
volatile float AnglePWM_Out=0.0, VelocityPWM_Out=0.0, DirectionPWM_Out=0.0;


volatile uint8_t movement_mode;

volatile float LeftEID, RightEID;


extern volatile float MPU6050_GyroZ, MPU6050_GyroY, MPU6050_AngleKlman;
extern volatile int8_t pitch, roll;

int32_t velocity_pwm, turn_pwm;

void SetTargetVelocity(float Velocity)
{
	if(Velocity > (float)VELOCITY_MAX_LIMIT) {Velocity = (float)VELOCITY_MAX_LIMIT;}
	if(Velocity < (float)-VELOCITY_MAX_LIMIT) {Velocity = (float)-VELOCITY_MAX_LIMIT;}	
	Target_Velocity = Velocity;
}

void CaculateTargetVelocity(void)
{
	static bool start_flag=false;
	static float target_velocity;
	if(movement_mode == NAVIGATION_MODE) {
		if(pitch > 20) {start_flag = true;}
		if(pitch < -20) {start_flag = false;}
		if(start_flag == true) {
			SetTargetVelocity(70.0);			
		} else {
			SetTargetVelocity(0.0);						
		}
	} else {
		if(abs(pitch) > 5) {
			target_velocity = (VELOCITY_MAX_LIMIT*pitch)/40.0;
			if(target_velocity > (VELOCITY_MAX_LIMIT-20)) target_velocity = VELOCITY_MAX_LIMIT;
			if(target_velocity < -(VELOCITY_MAX_LIMIT-20)) target_velocity = -VELOCITY_MAX_LIMIT;
			SetTargetVelocity(target_velocity);			
		} else {
			SetTargetVelocity(0);			
		}			
	}
	
}

void GetVelocityPulse(void)
{
	static int32_t LeftEnc_Cnt, RightEnc_Cnt;
			
	LeftEnc_Cnt  = MOTOR_PWM_COUNTER_MAX - Motor_GetQuadDecoderValue(MOTOR_ID_A);
	RightEnc_Cnt = Motor_GetQuadDecoderValue(MOTOR_ID_B);

	Motor_ClearQuadDecoderValue(MOTOR_ID_A | MOTOR_ID_B);

	if (LeftEnc_Cnt >= MOTOR_PWM_COUNTER_MAX/2)
	{
			LeftEnc_Cnt -= MOTOR_PWM_COUNTER_MAX;
	}
	if (RightEnc_Cnt >= MOTOR_PWM_COUNTER_MAX/2)
	{
			RightEnc_Cnt -= MOTOR_PWM_COUNTER_MAX;
	}	
	
	gLeftEnc_Sum += LeftEnc_Cnt;
	gRightEnc_Sum += RightEnc_Cnt;	
}

void VelocityPwmControl(void)
{
	static float fVdelta=0, last_fVdelta=0, dfVdelta=0;
	static float fV=0;
	static float fP, fI, fD;
	static float FLEX_VELOCITY_I, FLEX_VELOCITY_P, FLEX_VELOCITY_D;
	
	fVdelta = gLeftEnc_Sum + gRightEnc_Sum - 0;
	BothEnc_Sum = gLeftEnc_Sum + gRightEnc_Sum;
	gLeftEnc_Sum = 0;
	gRightEnc_Sum = 0;
	
	dfVdelta = fVdelta - last_fVdelta;
	last_fVdelta = fVdelta;
	
	fV *= (float)0.7;
	fV += fVdelta*(float)0.3;
	
	fI += fV;
	//fI -= (Target_Velocity - fabs(Direction_Diff)*0.003);
	fI -= Target_Velocity;
	
	if(fI > VELOCITY_INTEGRAL_MAX) {fI = VELOCITY_INTEGRAL_MAX;}
	if(fI < VELOCITY_INTEGRAL_MIN) {fI = VELOCITY_INTEGRAL_MIN;}
	
	if(movement_mode == REMOTE_LOADING_MODE) {
		FLEX_VELOCITY_I = RML_VELOCITY_I;
		FLEX_VELOCITY_P = RML_VELOCITY_P;		
		FLEX_VELOCITY_D = RML_VELOCITY_D;
	} else {
		FLEX_VELOCITY_I = EMN_VELOCITY_I;
		FLEX_VELOCITY_P = EMN_VELOCITY_P;				
		FLEX_VELOCITY_D = EMN_VELOCITY_D;				
	}
	
	fP = fV*FLEX_VELOCITY_P;
	fD = dfVdelta*FLEX_VELOCITY_D;
		
	VelocityPWM_Out_Old = VelocityPWM_Out_New;
	VelocityPWM_Out_New =	fP + fI*FLEX_VELOCITY_I + fD;	
	//VelocityPWM_Out_New =	fP + fI*FLEX_VELOCITY_I + fD + fabs(Direction_Diff)*0.15;	
}
	
void VelocityPwmControlOut(void)
{
	static float Velocity_Diff_Val;
	static int8_t Velocity_Step_Cnt=0;
	
	Velocity_Diff_Val = VelocityPWM_Out_New - VelocityPWM_Out_Old;
	
	VelocityPWM_Out = Velocity_Diff_Val * (++Velocity_Step_Cnt)/(PERIOD_VELOCITY_CONTROL/PERIOD_ANGLE_CONTROL) + VelocityPWM_Out_Old;
	
	if(VelocityPWM_Out >= VELOCITY_PWM_OUT_MAX) {VelocityPWM_Out = VELOCITY_PWM_OUT_MAX;}
	if(VelocityPWM_Out <= VELOCITY_PWM_OUT_MIN) {VelocityPWM_Out = VELOCITY_PWM_OUT_MIN;}
	
	if(Velocity_Step_Cnt == (PERIOD_VELOCITY_CONTROL/PERIOD_ANGLE_CONTROL)) {Velocity_Step_Cnt = 0;}	
}

void AnglePwmControlOut(void)
{
	static float Bias, ANGLE_P, ANGLE_D;
	
	if(movement_mode == REMOTE_LOADING_MODE) {
		ANGLE_P = RML_ANGLE_P;
		ANGLE_D = RML_ANGLE_D;
	} else {
		ANGLE_P = EMN_ANGLE_P;
		ANGLE_D = EMN_ANGLE_D;
	}

	Bias = MPU6050_AngleKlman - VERTICAL_ANGLE_OFFSET;
	AnglePWM_Out = ANGLE_P*Bias + ANGLE_D*MPU6050_GyroY;
}

void DirectionPwmControl(void)
{
	static float ySub, yAdd;
//	static float LeftEID, RightEID;
	static float DIRECTION_P, DIRECTION_D; 
	
	if((movement_mode == REMOTE_LOADING_MODE) || (movement_mode == REMOTE_RUNNING_MODE)) {
		DIRECTION_P = RML_DIRECTION_P;
		DIRECTION_D = RML_DIRECTION_D;
	} else {
		if(BothEnc_Sum < 10) {
			DIRECTION_P = 2000;
		} else {
			DIRECTION_P = EMN_DIRECTION_P;
		}
		DIRECTION_D = EMN_DIRECTION_D;
	}
	
	LeftEID = LeftEID_Measure_Sum/(PERIOD_DIRECTION_CONTROL/PERIOD_DIRECTION_MEASURE) - DIRECTION_LEFTEDI_OFFSET;
	RightEID = RightEID_Measure_Sum/(PERIOD_DIRECTION_CONTROL/PERIOD_DIRECTION_MEASURE) - DIRECTION_RIGHTEDI_OFFSET;
	if(LeftEID < 0) LeftEID = 0;
	if(RightEID < 0) RightEID = 0;
	LeftEID_Measure_Sum = 0;
	RightEID_Measure_Sum = 0;
	
	/* 在导航确认模式下，如果检测不到电磁信号，切换为遥控行走模式；反之则进入电磁导航模式 */
	if(movement_mode == NAVIGATION_MODE_CONFIRM) {		
		if((LeftEID<5) && (RightEID <5)) {
			movement_mode = REMOTE_RUNNING_MODE;
			SetTargetVelocity(0.0);
		}	else {
			movement_mode = NAVIGATION_MODE;			
			SetTargetVelocity(0.0);
		}		
	}

	/* 电磁导航模式下，如果电磁信号丢失，则关闭电机输出；反之则根据左右电磁信号强度差比和计算转向PWM值 */	
	if(movement_mode == NAVIGATION_MODE) {		
		if((LeftEID<5) && (RightEID <5)) Motor_EnableTurn(false);	// 关闭电机输出								

		ySub = LeftEID - RightEID;
		yAdd = RightEID + LeftEID;
	
		DirectionPWM_Out_Old = DirectionPWM_Out_New;
		DirectionPWM_Out_New = (ySub/yAdd)*DIRECTION_P + MPU6050_GyroZ*DIRECTION_D;
		Direction_Diff = (ySub/yAdd)*5000;
	}
	
	/* 遥控行走和遥控载重模式下，根据遥控器的翻滚角度值，计算转向PWM值 */	
	if((movement_mode == REMOTE_RUNNING_MODE) || (movement_mode == REMOTE_LOADING_MODE)) {
		if(abs(roll) > 10) {
			DirectionPWM_Out_Old = DirectionPWM_Out_New;
			DirectionPWM_Out_New = (roll - 10)*DIRECTION_P + MPU6050_GyroZ*DIRECTION_D;			
		} else {
			DirectionPWM_Out_Old = 0;
			DirectionPWM_Out_New = 0;						
		}
	}		
}

void DirectionPwmControlOut(void)
{
//	static float Direction_Diff_Val;
	static int8_t Direction_Step_Cnt=0;
	
//	Direction_Diff_Val = DirectionPWM_Out_New - DirectionPWM_Out_Old;

//	DirectionPWM_Out = Direction_Diff_Val * (++Direction_Step_Cnt)/(PERIOD_DIRECTION_CONTROL/PERIOD_ANGLE_CONTROL) + DirectionPWM_Out_Old;
//	DirectionPWM_Out = DirectionPWM_Out_New;
//	DirectionPWM_Out_New = 0;
	DirectionPWM_Out = DirectionPWM_Out_New / (PERIOD_DIRECTION_CONTROL/PERIOD_ANGLE_CONTROL);
	
	if(DirectionPWM_Out >= DIRECTION_PWM_OUT_MAX) {DirectionPWM_Out = DIRECTION_PWM_OUT_MAX;}
	if(DirectionPWM_Out <= DIRECTION_PWM_OUT_MIN) {DirectionPWM_Out = DIRECTION_PWM_OUT_MIN;}
	
	if(Direction_Step_Cnt == (PERIOD_DIRECTION_CONTROL/PERIOD_ANGLE_CONTROL)) {Direction_Step_Cnt = 0;}		
}


void RefreshMotorPwmController(void)
{
	gLeftPwm_Ctrl  = (-AnglePWM_Out) + VelocityPWM_Out + DirectionPWM_Out;
	gRightPwm_Ctrl = (-AnglePWM_Out) + VelocityPWM_Out - DirectionPWM_Out;

	if(gLeftPwm_Ctrl < -MOTOR_PWM_MAX_LIMIT) gLeftPwm_Ctrl = -MOTOR_PWM_MAX_LIMIT;
	if(gLeftPwm_Ctrl > MOTOR_PWM_MAX_LIMIT) gLeftPwm_Ctrl = MOTOR_PWM_MAX_LIMIT;
	if(gRightPwm_Ctrl < -MOTOR_PWM_MAX_LIMIT) gRightPwm_Ctrl = -MOTOR_PWM_MAX_LIMIT;
	if(gRightPwm_Ctrl > MOTOR_PWM_MAX_LIMIT) gRightPwm_Ctrl = MOTOR_PWM_MAX_LIMIT;

	/* Update the PWM for speed. */
#if 1
	if (gLeftPwm_Ctrl >= 0)
	{
			Motor_SetTurnDirection(MOTOR_ID_A, eMotor_DirectionForward);
			Motor_SetTrunSpeed(MOTOR_ID_A, gLeftPwm_Ctrl+MOTORA_DEAD_TIME);  //425
	}
	else /* gLeftPwm_Ctrl < 0 */
	{
			Motor_SetTurnDirection(MOTOR_ID_A, eMotor_DirectionBackward);
			Motor_SetTrunSpeed(MOTOR_ID_A, (-gLeftPwm_Ctrl)+MOTORA_DEAD_TIME);  
	}
#endif
#if 1
	if (gRightPwm_Ctrl >= 0)
	{
			Motor_SetTurnDirection(MOTOR_ID_B, eMotor_DirectionForward);
			Motor_SetTrunSpeed(MOTOR_ID_B, gRightPwm_Ctrl+MOTORB_DEAD_TIME);
	}
	else
	{
			Motor_SetTurnDirection(MOTOR_ID_B, eMotor_DirectionBackward);
			Motor_SetTrunSpeed(MOTOR_ID_B, (-gRightPwm_Ctrl)+MOTORB_DEAD_TIME); //450
	}
#endif	
}

/* EOF. */

