/* main.c */
#include "app_inc.h"
#include "MPU6050.h"

#define BSP_I2C_MPU6050_DEV_ADDR 0x68 /* AD0 = 0. */

typedef struct
{
    /* 重力加速度 */
    uint8_t AccX_H;
    uint8_t AccX_L;
    uint8_t AccY_H;
    uint8_t AccY_L;
    uint8_t AccZ_H;
    uint8_t AccZ_L;
    uint8_t TMP_H;  /* No used. */
    uint8_t TMP_L;  /* No used. */

    /* 角速度 */
    uint8_t GroX_H;
    uint8_t GroX_L;
    uint8_t GroY_H;
    uint8_t GroY_L;
    uint8_t GroZ_H;
    uint8_t GroZ_L;
    uint8_t NOTUSE_H; /* No used. */
    uint8_t NOTUSE_L; /* No used. */
} MPU6050_Info_T;

void PCC_Configuration(void);
void PORT_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

void GPIO_ToggleLED(void);

void MPU6050_Init(void);
void LPI2C_SingleWriteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t value);
void LPI2C_StartMultipleRead(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t count);


volatile MPU6050_Info_T gMpu6050InfoStruct;
volatile uint8_t gMpu6050FrameIntCounter = 0U;
volatile bool bMpu6050SampleDone;

int main(void)
{
    LPI2C_MasterConfig_T lpi2cMasterConfigStruct;

    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);

    PCC_Configuration();
    PORT_Configuration();
    GPIO_Configuration();
    NVIC_Configuration();

    printf("\r\n\r\nLPI2C Example.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);

    /* Initialize the I2C. */
    lpi2cMasterConfigStruct.BusClkHz = 150000000UL; /* 150MHz */
    lpi2cMasterConfigStruct.Baudrate = 100000UL; /* 100k. */
    lpi2cMasterConfigStruct.TxFIFOWatermarkCount = 4U;
    lpi2cMasterConfigStruct.RxFIFOWatermarkCount = 3U; /* Rx watermark is 4. */
    LPI2C_HAL_ConfigMaster(LPI2C0, &lpi2cMasterConfigStruct);
    LPI2C_HAL_EnableRxInterrupt(LPI2C0, true);

    /* Initialize the MPU6050 based on I2C. */
    MPU6050_Init();


    while (1)
    {
        getchar();
        GPIO_ToggleLED();
        bMpu6050SampleDone = false;
        LPI2C_StartMultipleRead(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_ACCEL_XOUT_H, 16U);
        while (!bMpu6050SampleDone) {}

        printf("AccX_H = %2x ,AccX_L = %2x\r\n", gMpu6050InfoStruct.AccX_H, gMpu6050InfoStruct.AccX_L);
        printf("AccY_H = %2x ,AccY_L = %2x\r\n", gMpu6050InfoStruct.AccY_H, gMpu6050InfoStruct.AccY_L);
        printf("AccZ_H = %2x ,AccZ_L = %2x\r\n", gMpu6050InfoStruct.AccZ_H, gMpu6050InfoStruct.AccZ_L);
        printf("TMP_H  = %2x  ,TMP_L = %2x\r\n", gMpu6050InfoStruct.TMP_H,  gMpu6050InfoStruct.TMP_L);
        printf("GroX_H = %2x ,GroX_L = %2x\r\n", gMpu6050InfoStruct.GroX_H, gMpu6050InfoStruct.GroX_L);
        printf("GroY_H = %2x ,GroY_L = %2x\r\n", gMpu6050InfoStruct.GroY_H, gMpu6050InfoStruct.GroY_L);
        printf("GroZ_H = %2x ,GroZ_L = %2x\r\n", gMpu6050InfoStruct.GroZ_H, gMpu6050InfoStruct.GroZ_L);
        printf("\r\n");
    }
}

void PCC_Configuration(void)
{
    /* PCC->PORTC |= PCC_CLKCFG_CGC_MASK;*/
    PCC->CLKCFG[PCC_PORTC_INDEX] = PCC_CLKCFG_CGC_MASK; /* Enable clock for PORTC. */
    PCC->CLKCFG[PCC_PORTA_INDEX] = PCC_CLKCFG_CGC_MASK; /* Enable clock for PORTA. */
    PCC->CLKCFG[PCC_LPI2C0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6); /* Enable clock for LPI2C0 with Fast IRC. */
}

void PORT_Configuration(void)
{
    /* Config pins for LEDs. */
    PORTC->PCR[14] = PORT_PCR_MUX(1);
    PORTC->PCR[15] = PORT_PCR_MUX(1);

    /* Configuure pins for I2C of MPU6050. */
    PORTA->PCR[2] = PORT_PCR_MUX(3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* LPI2C_SDA. */
    PORTA->PCR[3] = PORT_PCR_MUX(3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* LPI2C_SCL. */
}

void NVIC_Configuration(void)
{
    NVIC_EnableIRQ(LPI2C0_IRQn);
}

void GPIO_Configuration(void)
{
    GPIOC->PDDR |= (1 << 14); /* Set PTC14 as output direction. */
    GPIOC->PDDR |= (1 << 15); /* Set PTC15 as output direction. */

    GPIOC->PCOR = (1 << 14); /* Set PTC14 to output logic 0. */
    GPIOC->PSOR = (1 << 15); /* Set PTC15 to output logic 1. */
}

void GPIO_ToggleLED(void)
{
    GPIOC->PTOR = (1 << 14);
    GPIOC->PTOR = (1 << 15);
}

void LPI2C_SingleWriteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t value)
{

    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, devAddr << 1U); /* Send START condition with device address. */
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, regAddr);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, value);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTOP, 0U); /* Send STOP condition. */

#if 0
    LPI2C0->MTDR = (0x00000400|(0x68<<1));
    LPI2C0->MTDR = regAddr; /* reg addr. */
    LPI2C0->MTDR = value; /* writting data. */
    LPI2C0->MTDR = (2<<8); /* STOP command */
#endif
    /* Wait for the ACK, to make sure the write command is done. */
    while(base->MSR & LPI2C_MSR_BBF_MASK) {}
    while(!(base->MSR & LPI2C_MSR_SDF_MASK)) {}
    while(!(base->MSR & LPI2C_MSR_TDF_MASK)) {}
}

void LPI2C_StartMultipleRead(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t count)
{
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, devAddr << 1U);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, regAddr);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, devAddr << 1U | 1U); /* Read. */
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_RxCountOfNplus1Byte, count-1U);

    /* while(LPI2C0->MSR & LPI2C_MSR_BBF_MASK){}; */
}

void MPU6050_Init(void)
{
    LPI2C_SingleWriteBlocking(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_PWR_MGMT_1, 0x00);
    LPI2C_SingleWriteBlocking(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_PWR_MGMT_2, 0x00);
    LPI2C_SingleWriteBlocking(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_CONFIG, 0x06);
    LPI2C_SingleWriteBlocking(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_GYRO_CONFIG, 0x08);
    LPI2C_SingleWriteBlocking(LPI2C0, BSP_I2C_MPU6050_DEV_ADDR, MPU6050_ACCEL_CONFIG, 0x08);
}

void LPI2C0_IRQHandler(void)
{
    switch (gMpu6050FrameIntCounter)
    {
    case 0U:
        gMpu6050InfoStruct.AccX_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.AccX_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.AccY_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.AccY_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050FrameIntCounter++;
        break;
    case 1U:
        gMpu6050InfoStruct.AccZ_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.AccZ_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.TMP_H  = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.TMP_L  = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050FrameIntCounter++;
        break;
    case 2U:
        gMpu6050InfoStruct.GroX_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.GroX_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.GroY_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.GroY_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050FrameIntCounter++;
        break;
    case 3U:
        gMpu6050InfoStruct.GroZ_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.GroZ_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.NOTUSE_H = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050InfoStruct.NOTUSE_L = LPI2C_HAL_ReadRxFIFOData(LPI2C0);
        gMpu6050FrameIntCounter = 0U;

        bMpu6050SampleDone = true;
        break;
    default:
        gMpu6050FrameIntCounter = 0U;
        break;
    } /* switch (gMpu6050FrameIntCounter) */
}

/* EOF. */

