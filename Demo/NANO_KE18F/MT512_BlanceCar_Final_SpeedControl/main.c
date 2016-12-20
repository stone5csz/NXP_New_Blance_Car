/* main.c */
#include "app_inc.h"

#define DEMO_LPIT_MOVEMENT_CHANNEL 0U
#define DEMO_LPIT_BALANCE_CHANNEL  1U

static void NVIC_Configuration(void);
static void LPIT_Configuration(void);

extern void MoveCtrlSetup(void);
extern void MoveCtrlLoop(void);

extern void BalanceSenseSetup(void);
extern void BalanceSenseLoop(void);
extern void GetSenseOffsetLoop(void);
extern void BSP_InitRF(void);

extern void MPU6050_Offset_Init(void);
extern void SetTargetVelocity(float Velocity);

extern void RF24L01_InitDevice(void);
extern void RF24L01_Init(void);
extern void SetRX_Mode(void);
extern unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
extern void nRF24L01_TxPacket(unsigned char * tx_buf);

extern void CaculateTargetVelocity(void);
extern void GetVelocityPulse(void);
extern void VelocityPwmControl(void);
extern void VelocityPwmControlOut(void);
extern void AnglePwmControlOut(void);
extern void DirectionPwmControl(void);
extern void DirectionPwmControlOut(void);
extern void RefreshMotorPwmController(void);
extern void DirectionPwmControl(void);

volatile uint16_t get_acc_gyro_offset_cnt;
volatile bool get_acc_gyro_offset_flag, dma_irq_is_done;
volatile int8_t pitch, roll;
volatile int8_t RF_RxBuf[3];
volatile int8_t RF_TxBuf[3];
volatile bool EID_Left_Is_Ready, EID_Right_Is_Ready;

extern volatile uint32_t LeftEID_Measure_Sum, RightEID_Measure_Sum;
extern volatile bool Angle_Is_Updated;
extern volatile float Target_Velocity;
extern volatile uint8_t movement_mode;

extern volatile float DirectionPWM_Out_Old, DirectionPWM_Out_New, Direction_Diff;
extern volatile int16_t Accel_Y;


volatile uint32_t lpit_cnt;
volatile int32_t DirectionPWM;

int main(void)
{
		RF_RxBuf[0] = 0;
		RF_RxBuf[1] = 0;
		RF_RxBuf[2] = 0;
		
	  
		BSP_InitSystem();
    NVIC_Configuration(); /* Enable interrupt for modules. */

    BSP_InitStdioLPUART(115200U);
    BSP_InitGPIO_LED();
		BSP_InitGPIO_SWITCH();
    /* Initialize hardware for motor. */
    BSP_InitMotorTurn();
    BSP_InitMotorQuadDecoder();
	
		/* 初始化ADC0和ADC2用于采集电磁感应信号 */
		BSP_InitEIDetect();
	
		/* 初始化SPI0，用于控制RF模块 */	
	  BSP_InitRF();

    /* 初始化GPIO口，用于控制LED指示灯. */
    GPIO_SetPinDir(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true); /* LED1. */
    GPIO_SetPinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true);
    GPIO_SetPinDir(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN, true); /* LED2. */
    GPIO_SetPinLogic(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN, true);
		
		/* 初始化GPIO口，用于检测跳线状态，决定小车运行模式		*/
		GPIO_SetPinDir(BSP_GPIO_SWITCH_PORT, BSP_GPIO_SWITCH_PIN, false);
		
		if(GPIO_GetPinLogic(BSP_GPIO_SWITCH_PORT, BSP_GPIO_SWITCH_PIN) == true) {		/* 如果跳线短接，则进入遥控模式 */
			movement_mode = REMOTE_LOADING_MODE;
		} else {																																		/* 如果跳线开路，则进入导航模式 */
			movement_mode = NAVIGATION_MODE_CONFIRM;
		}
		
		/* 初始化LPIT0，为小车产生基本控制周期定时中断*/
    LPIT_Configuration();

		/* 初始化LPIT0，为小车产生基本控制周期定时中断*/		
    Motor_InitDevice();
    Motor_EnableTurn(true);

    printf("Motor initialization done.\r\n");
	
		EIDetect_InitDevice();
    printf("EIDetect initialization done.\r\n");

		RF24L01_InitDevice();

		delay_s(1);

		get_acc_gyro_offset_flag = 1;
		dma_irq_is_done = 1;
		get_acc_gyro_offset_cnt = 0;
		MPU6050_Offset_Init();

    RF24L01_Init();
		SetRX_Mode();
//		SetTX_Mode();
    BalanceSenseSetup(); /* 准备平衡控制 */
		
		/* 获取传感器偏移数据 */
		while(get_acc_gyro_offset_flag) {
			if(dma_irq_is_done) {
				dma_irq_is_done = 0;
				get_acc_gyro_offset_cnt += 1;
				delay_us(100);
        GetSenseOffsetLoop();				
			}					
		}
			
    LPIT_HAL_StartChannel(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL);
//    LPIT_HAL_StartChannel(LPIT0, DEMO_LPIT_BALANCE_CHANNEL);
    while (1)
    {
			SetRX_Mode();
			delay_us(1000);
			nRF24L01_RxPacket((unsigned char*)RF_RxBuf);
			if(movement_mode == NAVIGATION_MODE) {
//				delay_ms(2000);
//				pitch = 21;
				pitch = RF_RxBuf[0];
			} else {
				pitch = RF_RxBuf[0];
			}
			roll = RF_RxBuf[1];
			delay_us(1500);
    }
}

static void NVIC_Configuration(void)
{
    NVIC_EnableIRQ(LPIT0_IRQn);
//    NVIC_EnableIRQ(LPIT1_IRQn);
    NVIC_EnableIRQ(ADC0_IRQn);
    NVIC_EnableIRQ(ADC2_IRQn);
	
		NVIC_SetPriority(LPIT0_IRQn, 2);
		NVIC_SetPriority(ADC0_IRQn, 1);
		NVIC_SetPriority(ADC2_IRQn, 1);	
}

static void LPIT_Configuration(void)
{
    LPIT_ChannelConfig_T lpitChannelConfigStruct;

    PCC->CLKCFG[PCC_LPIT0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6U); /* PLL. */
    /* LPIT. */
    LPIT_HAL_Enable(LPIT0, true);
    LPIT_HAL_Reset(LPIT0);
    LPIT_HAL_GetChannelConfigForNormalTimer(&lpitChannelConfigStruct);

    /* 配置小车测量控制周期定时器 */
    LPIT_HAL_ConfigChannel(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL, &lpitChannelConfigStruct);
    LPTI_HAL_SetChannelTimeoutValue(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL, 75000 * PERIOD_SYSTICK); /* BSP_CLK_SPLLDIV2_HZ = 75MHz, 20ms */
//    LPTI_HAL_SetChannelTimeoutValue(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL, 100000 * PERIOD_SYSTICK); /* BSP_CLK_SPLLDIV2_HZ = 100MHz, 20ms */
    LPIT_HAL_EnableChannelInterrupt(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL, true);
}

/****************************************************************************
 * Interrupts Service.
 ***************************************************************************/
void LPIT0_IRQHandler(void)
{
	static uint8_t Event_Ctrl_Cnt=0, Led_Ctrl_Cnt=0;

	if (LPIT_HAL_GetChannelTimeoutFlag(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL))
	{	
		LPTI_HAL_ClearChannelTimeoutFlag(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL);

		CaculateTargetVelocity();
		
		if(Led_Ctrl_Cnt == 60) {
			if(movement_mode == REMOTE_LOADING_MODE) {												/* 遥控载重模式 */
				GPIO_TogglePinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);			
			}else if (movement_mode == NAVIGATION_MODE) {											/* 电磁导航模式	*/
				GPIO_TogglePinLogic(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN);			
			}else{																														/* 遥控行走模式	*/ 
				GPIO_TogglePinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);			
				GPIO_TogglePinLogic(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN);							
			}
			Led_Ctrl_Cnt = 0;
		} else {
			Led_Ctrl_Cnt++;
		}
		
		if((movement_mode == NAVIGATION_MODE) || (movement_mode == NAVIGATION_MODE_CONFIRM)) {
			if(((Event_Ctrl_Cnt+1) % COUNT_DIRECTION_MEASURE) == 0) {
				EID_Left_Is_Ready = false;
				EID_Right_Is_Ready = false;
				StartEIDetect(EID_CHANNEL_LEFT);
				StartEIDetect(EID_CHANNEL_RIGHT);						
				while((EID_Left_Is_Ready == false) || (EID_Left_Is_Ready == false));	// 等待获取电磁测量数据
			}
		}	

		/* 计算方向控制PWM */
		if(((Event_Ctrl_Cnt+1) % COUNT_DIRECTION_CONTROL) == 0) {
			DirectionPwmControl();
			DirectionPWM = Direction_Diff;
//			DirectionPWM = DirectionPWM_Out_New; 
//			DirectionPWM = -1000; 
//			RF_TxBuf[0] = (uint8_t)(DirectionPWM & 0x000000FF);
//			RF_TxBuf[1] = (uint8_t)((DirectionPWM>>8) & 0x000000FF);
			//RF_TxBuf[0] = (uint8_t)LeftEID_Measure_Sum;
			//RF_TxBuf[1] = (uint8_t)RightEID_Measure_Sum;
			//nRF24L01_TxPacket(RF_TxBuf);
		}
		
		/* 获取速度测量数据 */
		if(((Event_Ctrl_Cnt+1) % COUNT_VELOCITY_MEASURE) == 0) {
			GetVelocityPulse();				
		}

		/* 计算速度PWM */
		if(((Event_Ctrl_Cnt+1) % COUNT_VELOCITY_CONTROL) == 0) {
			VelocityPwmControl();
		}

		/* 获取角度测量数据 */		
		if(((Event_Ctrl_Cnt+1) % COUNT_ANGLE_MEASURE) == 0) {							
			Angle_Is_Updated = false;
			BalanceSenseLoop();
			while(Angle_Is_Updated == false);																	// 等待角度值被更新
		}
		
		if(((Event_Ctrl_Cnt+1) % COUNT_ANGLE_CONTROL) == 0) {							
			VelocityPwmControlOut();																					// 输出速度PWM
			DirectionPwmControlOut();																					// 输出方向PWM
			AnglePwmControlOut();																							// 输出角度PWM
			RefreshMotorPwmController();																			// 刷新PWM控制器
		}
		
		if((Event_Ctrl_Cnt+1) == COUNT_VELOCITY_CONTROL) { 
			Event_Ctrl_Cnt = 0;
		} else {
			Event_Ctrl_Cnt++;
		}
							
		//lpit_cnt = LPIT_HAL_GetChannelCurrentValue(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL);
		
		if (LPIT_HAL_GetChannelTimeoutFlag(LPIT0, DEMO_LPIT_MOVEMENT_CHANNEL)) {
			GPIO_SetPinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, false);						
		}

	}
}

void ADC0_IRQHandler(void)
{
		LeftEID_Measure_Sum += BSP_EID_LEFT_ADC_MODULE_NUM->R[0];
	  RF_TxBuf[0] = LeftEID_Measure_Sum;
		EID_Left_Is_Ready = true;
}	

void ADC2_IRQHandler(void)
{
		RightEID_Measure_Sum += BSP_EID_RIGHT_ADC_MODULE_NUM->R[0];
	  RF_TxBuf[1] = RightEID_Measure_Sum;
		EID_Right_Is_Ready = true;
}	


/* EOF. */

