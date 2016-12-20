/* balance_control.c */

#include "app_inc.h"
#include "MPU6050.h"
#include "math.h"

typedef struct
{
    float  Angle;
    int16_t Gyro_Y;
} PosInfo_T;

#define BL_BLANCE_ANGLE_OFFSET    0 /* 指定小车平衡站立的倾斜角度 */
#define GRY_GAIN						2000.0/32768.0
#define GRY_OFFSET					0
#define MPU_ADDR            (0x68)
#define MPU6050_RX_BUFFER_SIZE 14
volatile uint32_t MPU6050_RxBuffer[MPU6050_RX_BUFFER_SIZE];
volatile MPU6050_PosInfo_T MPU6050_PosInfoStruct, MPU6050_PostInfoOffset;
volatile float MPU6050_AngleByAccel;
volatile float MPU6050_GyroZ, MPU6050_GyroY, MPU6050_AngleKlman;
volatile PosInfo_T PosInfoStruct;
volatile int16_t BL_AnglePidPwm;
volatile int32_t Accel_X_offset, Accel_Y_offset, Accel_Z_offset, Gyro_X_offset, Gyro_Y_offset, Gyro_Z_offset;
volatile bool Angle_Is_Updated;
volatile int16_t Accel_Y;

extern volatile uint16_t get_acc_gyro_offset_cnt;
extern volatile bool get_acc_gyro_offset_flag, dma_irq_is_done;

static void BL_GetSensorPos(MPU6050_PosInfo_T *info);

void Kalman_Filter(void)      
{
    static float klm_angle = 0;
    static float Q_gyro = 0.003;
    static float Q_angle = 0.001;   // 过程噪声的协方差
    static float R_angle = 0.5;     // 测量噪声的协方差 既测量偏差
//    static float dt = 0.005; /* 5ms 执行一次采样*/
    static float dt = 0.002; /* 5ms 执行一次采样*/
    static char C_0 = 1;
    static float Q_bias=0, Angle_err;
    static float PCt_0, PCt_1, E;
    static float K_0, K_1, t_0, t_1;
    static float Pdot[4] ={0,0,0,0};
    static float PP[2][2] = { { 1, 0 },{ 0, 1 } };

    static float Gyro;
		static float angleA, angleG=0;

    //angleA = atan2(MPU6050_PosInfoStruct.Accel_X, MPU6050_PosInfoStruct.Accel_Z)*180/3.141593653; // calculate angle from accelor                //计算倾角    //angleA = atan(MPU6050_PosInfoStruct.Accel_X/MPU6050_PosInfoStruct.Accel_Z)*180/3.141593653; // calculate angle from accelor                //计算倾角
    angleA = atan2((MPU6050_PosInfoStruct.Accel_X - Accel_X_offset), (MPU6050_PosInfoStruct.Accel_Z - Accel_Z_offset + 16384))*180/3.141593653; // calculate angle from accelor                //计算倾角
    //printf("%d,", (int)(Accel*1000));
    //Gyro = -MPU6050_PosInfoStruct.Gyro_Y/16.384;                                    // convert gyroscope result
		Gyro = (-MPU6050_PosInfoStruct.Gyro_Y + Gyro_Y_offset) * GRY_GAIN;
		MPU6050_GyroZ = (-MPU6050_PosInfoStruct.Gyro_Z + Gyro_Z_offset) * GRY_GAIN;
		angleG = angleG + Gyro * dt;
    klm_angle += (Gyro - Q_bias) * dt; //先验估计
    //printf("%d,", (int)(angle*1000));
    Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

    Pdot[1]=-PP[1][1];
    Pdot[2]=-PP[1][1];
    Pdot[3]=Q_gyro;
    PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
    PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
    PP[1][0] += Pdot[2] * dt;
    PP[1][1] += Pdot[3] * dt;
        
    Angle_err = angleA - klm_angle;  //zk-先验估计
    
    PCt_0 = C_0 * PP[0][0];
    PCt_1 = C_0 * PP[1][0];
    
    E = R_angle + C_0 * PCt_0;
    
    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;
    
    t_0 = PCt_0;
    t_1 = C_0 * PP[0][1];

    PP[0][0] -= K_0 * t_0;       //后验估计误差协方差
    PP[0][1] -= K_0 * t_1;
    PP[1][0] -= K_1 * t_0;
    PP[1][1] -= K_1 * t_1;
        
    klm_angle   += K_0 * Angle_err;  //后验估计
    Q_bias  += K_1 * Angle_err;  //后验估计
    PosInfoStruct.Angle = klm_angle;
    PosInfoStruct.Gyro_Y = Gyro;

		//printf("%f,%f,%f,%f,%d,%d,%d\r\n", angleA, angleG, klm_angle, Gyro, MPU6050_PosInfoStruct.Gyro_Y, MPU6050_PosInfoStruct.Gyro_X, MPU6050_PosInfoStruct.Gyro_Z);
		//printf("%f,%f,%f\r\n", angleA, angleG, klm_angle);

}

static void BL_PCC_Configuration(void)
{
    PCC->CLKCFG[PCC_PORTA_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_LPI2C0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6U); /* PLL. */
    PCC->CLKCFG[PCC_DMAMUX0_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_DMA0_INDEX] = PCC_CLKCFG_CGC_MASK;
}

static void BL_PORT_Configuration(void)
{
    PORTA->PCR[2] = 0x303;  //lpi2c0_sda
    PORTA->PCR[3] = 0x303;  //lpi2c0_scl
}

static void BL_NVIC_Configuration(void)
{
    NVIC_EnableIRQ(DMA0_IRQn);
		NVIC_SetPriority(DMA0_IRQn, 0);	
}

static void BL_LPI2C_Configuration(void)
{
    LPI2C0->MCFGR1 |= LPI2C_MCFGR1_AUTOSTOP_MASK
                    | LPI2C_MCFGR1_PINCFG(2)
                    | LPI2C_MCFGR1_PRESCALE(3);
    LPI2C0->MCCR0 = 0x1613161c;
    LPI2C0->MCCR1 = 0x01070308; // clk & baudrate cfg
    // disable dose mode
    LPI2C0->MCR |= LPI2C_MCR_DOZEN_MASK
                |  LPI2C_MCR_MEN_MASK;  // enable master mode
    LPI2C0->MDER |= LPI2C_MDER_RDDE_MASK
                 |  LPI2C_MDER_TDDE_MASK;
}

static void BL_DMA_Configuration(void)
{
    EDMA_TransferConfig_T edmaTransferConfigStruct;

    EDMA_HAL_ResetEngine();

    /* 使用DMA的0通道作为I2C的输入通道 */
    DMAMUX_HAL_SetTriggerForDMA(0U, eDMAMUX_TriggerSource_Disabled);
    EDMA_HAL_ResetChannel(0U);

    edmaTransferConfigStruct.SrcAddr = (uint32_t)(&(LPI2C0->MRDR));
    edmaTransferConfigStruct.DestAddr = (uint32_t)(MPU6050_RxBuffer);
    edmaTransferConfigStruct.MinorLoopByteCount = 4U;
    edmaTransferConfigStruct.SrcBusWidthMode = eEDMA_BusWidth_4Byte;
    edmaTransferConfigStruct.DestBusWidthMode = eEDMA_BusWidth_4Byte;
    edmaTransferConfigStruct.SrcAddrCycleMode = eEDMA_AddrCycle_Disabled;
    edmaTransferConfigStruct.DestAddrCycleMode = eEDMA_AddrCycle_Disabled;
    edmaTransferConfigStruct.SrcAddrIncPerTransfer = 0;
    edmaTransferConfigStruct.DestAddrIncPerTransfer = 4;
    edmaTransferConfigStruct.SrcAddrMinorLoopDoneOffset = 0;
    edmaTransferConfigStruct.DestAddrMinorLoopDoneOffset = 0;
    edmaTransferConfigStruct.MinorLoopCount = MPU6050_RX_BUFFER_SIZE;
    edmaTransferConfigStruct.SrcAddrMajorLoopDoneOffset = 0;
    edmaTransferConfigStruct.DestAddrMajorLoopDoneOffset = -(4 * MPU6050_RX_BUFFER_SIZE);
    EDMA_HAL_ConfigTransfer(0U, &edmaTransferConfigStruct);
    EDMA_HAL_EnableHwTrigger(0U, true);
    EDMA_HAL_SetHwTriggerAutoDisabledOnMajorLoopDone(0U, false); /* 在传输完成后仍能够继续响应下次传输 */

    EDMA_HAL_EnableInterruptOnMajorLoopDone(0U, true); /* 启动传输完成中断 */

    DMAMUX_HAL_SetTriggerForDMA(0U, eDMAMUX_TriggerSource_LPI2C0_Rx);
}


static void IICwriteBits(uint8_t dev, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{
    uint8_t b, mask;
    //b = IICreadByte(dev, reg);

    b = LPI2C_HAL_ReadByteBlocking(LPI2C0, dev, reg);
    mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
    data <<= (8 - length);
    data >>= (7 - bitStart);
    b &= mask;
    b |= data;
    //IICwriteByte(dev, reg, b);
    LPI2C_HAL_WriteByteBlocking(LPI2C0, dev, reg, b);
}

/************************************************
* function: IICwriteBit
* description: wite a bit in slave 
* parameters:
*   dev, target slave address
*   reg, target slave register address  
*   bitNum, start bit of target register 
*   data, write 0 or 1 
* return:  none 
*************************************************/
static void IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    b = LPI2C_HAL_ReadByteBlocking(LPI2C0, dev, reg);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    LPI2C_HAL_WriteByteBlocking(LPI2C0, dev, reg, b);
}

static void BL_MPU6050_Init(void)
{
#if 1
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_PWR_MGMT_1, 0x00);			//解除休眠
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_PWR_MGMT_1, 0x01);			//解除休眠
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_SMPLRT_DIV, 0x07);			//陀螺仪采样率，1KHz
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_CONFIG, 0x06);					//低通滤波器的设置，截止频率是 1K，带宽是 5K
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_GYRO_CONFIG, 0x18);		//不自检，2000deg/s
		LPI2C_HAL_WriteByteBlocking(LPI2C0, MPU_ADDR, MPU6050_RA_ACCEL_CONFIG, 0x00);		//加速度传感器工作在 2G 模式，不自检
	
	  delay_ms(50);

#endif
}

static void BL_MPU6050_StartReadSensorCmd(void)
{
    /* LPI2C0_MDER |= LPI2C_MDER_RDDE_MASK; */

    LPI2C_HAL_EnableRxDMA(LPI2C0, true);

    LPI2C_HAL_WriteTxFIFOCmd(LPI2C0, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, (MPU_ADDR << 1) | I2C_WRITE_CMD_MASK);
    LPI2C_HAL_WriteTxFIFOCmd(LPI2C0, eLPI2C_Cmd_TxData, 0x3B);
    LPI2C_HAL_WriteTxFIFOCmd(LPI2C0, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, (MPU_ADDR << 1) | I2C_READ_CMD_MASK);
    LPI2C_HAL_WriteTxFIFOCmd(LPI2C0, eLPI2C_Cmd_RxCountOfNplus1Byte, 13U); /* 接收14字节 */
}

void BL_GetSensorPos(MPU6050_PosInfo_T *info)
{
    info->Accel_X = (int16_t)( ((uint16_t)MPU6050_RxBuffer[0] << 8)  | (uint16_t)MPU6050_RxBuffer[1] );
    info->Accel_Y = (int16_t)( ((uint16_t)MPU6050_RxBuffer[2] << 8)  | (uint16_t)MPU6050_RxBuffer[3] );
    info->Accel_Z = (int16_t)( ((uint16_t)MPU6050_RxBuffer[4] << 8)  | (uint16_t)MPU6050_RxBuffer[5] );
    info->Gyro_X  = (int16_t)( ((uint16_t)MPU6050_RxBuffer[8] << 8)  | (uint16_t)MPU6050_RxBuffer[9] );
    info->Gyro_Y  = (int16_t)( ((uint16_t)MPU6050_RxBuffer[10] << 8) | (uint16_t)MPU6050_RxBuffer[11]);
    info->Gyro_Z  = (int16_t)( ((uint16_t)MPU6050_RxBuffer[12] << 8) | (uint16_t)MPU6050_RxBuffer[13]);
}

/* I2C传输完成中断服务程序 */
void DMA0_IRQHandler(void)
{
    if ( EDMA_HAL_IsMajorLoopDone(0U) )
    {
        /* 清标志位 */
        EDMA_HAL_ClearMajorLoopDoneFlag(0U);
        EDMA_HAL_ClearInterruptFlagOfMajorLoopDone(0U);


        /* 处理数据 */
				if(get_acc_gyro_offset_flag) {
					BL_GetSensorPos((MPU6050_PosInfo_T *)(&MPU6050_PosInfoStruct));
					Accel_X_offset += (uint32_t)(MPU6050_PosInfoStruct.Accel_X);
					Accel_Y_offset += (uint32_t)(MPU6050_PosInfoStruct.Accel_Y);
					Accel_Z_offset += (uint32_t)(MPU6050_PosInfoStruct.Accel_Z);
					Gyro_X_offset += (uint32_t)(MPU6050_PosInfoStruct.Gyro_X);
					Gyro_Y_offset += (uint32_t)(MPU6050_PosInfoStruct.Gyro_Y);
					Gyro_Z_offset += (uint32_t)(MPU6050_PosInfoStruct.Gyro_Z);		
					if(get_acc_gyro_offset_cnt == 1000) {
						Accel_X_offset /= 1000;
						Accel_Y_offset /= 1000;
						Accel_Z_offset /= 1000;
						Gyro_X_offset /= 1000;
						Gyro_Y_offset /= 1000;
						Gyro_Z_offset /= 1000;
						get_acc_gyro_offset_flag = 0;
						dma_irq_is_done = 0;
					} else {
						dma_irq_is_done = 1;
					}
				} else {			
					BL_GetSensorPos((MPU6050_PosInfo_T *)(&MPU6050_PosInfoStruct));		
					Accel_Y = MPU6050_PosInfoStruct.Accel_Y;
					Kalman_Filter();
					if(abs(PosInfoStruct.Angle) > 60) {
						    Motor_EnableTurn(false);											
					}
					//printf("PosInfoStruct.Angle: %f, PosInfoStruct.Gyro_Y: %d\r\n", PosInfoStruct.Angle, PosInfoStruct.Gyro_Y);
					//printf("%f,%d,%d\r\n", PosInfoStruct.Angle, PosInfoStruct.Gyro_Y, MPU6050_PosInfoStruct.Accel_X);
					//BL_AnglePidPwm = BL_GetAnglePID(PosInfoStruct.Angle, PosInfoStruct.Gyro_Y);
					//printf("BL_AnglePidPwm: %d\r\n", BL_AnglePidPwm);
					MPU6050_AngleKlman = PosInfoStruct.Angle;
					MPU6050_GyroY = PosInfoStruct.Gyro_Y;
					Angle_Is_Updated = true;
				}
			}
}

void BalanceSenseSetup(void)
{
    /* 初始化硬件 */
    BL_PCC_Configuration();
    BL_PORT_Configuration();
    BL_NVIC_Configuration();


    /* 初始化MPU6050 */
    BL_LPI2C_Configuration();
    BL_MPU6050_Init();

    BL_DMA_Configuration();

}

/* 外部调用函数接口，在定时器中断服务程序中周期调用 */
void BalanceSenseLoop(void)
{
    /* 读取传感器姿态数据 */


    /* 计算 */

    BL_MPU6050_StartReadSensorCmd(); /* 启动触发链，计算过程将在触发链的最后一个环节 */
}

void GetSenseOffsetLoop(void)
{
    BL_MPU6050_StartReadSensorCmd(); /* 启动触发链，计算过程将在触发链的最后一个环节 */
}

void MPU6050_Offset_Init(void)
{
    Accel_X_offset = (int32_t)(0);
    Accel_Y_offset = (int32_t)(0);
    Accel_Z_offset = (int32_t)(0);
    Gyro_X_offset  = (int32_t)(0);
    Gyro_Y_offset  = (int32_t)(0);
    Gyro_Z_offset  = (int32_t)(0);	
}


