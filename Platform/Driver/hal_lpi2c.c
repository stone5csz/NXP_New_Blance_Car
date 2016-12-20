/* hal_lpi2c.c */
#include "hal_lpi2c.h"

void LPI2C_HAL_ResetTxFIFO(LPI2C_Type *base)
{
    base->MCR |= LPI2C_MCR_RTF_MASK;
}

void LPI2C_HAL_ResetRxFIFO(LPI2C_Type *base)
{
    base->MCR |= LPI2C_MCR_RRF_MASK;
}

/*
* Master mode:
* 1. 8-bit address
* 2. Normal speed mode.
* 3. Tx FIFO watermark & Rx FIFO watermark.
*/
void LPI2C_HAL_ConfigMaster(LPI2C_Type *base, LPI2C_MasterConfig_T *configPtr)
{
#if 0
    /* Reset the master configuration. */
    base->MCR = LPI2C_MCR_RTF_MASK   /* Reset Tx FIFO. */
              | LPI2C_MCR_RRF_MASK   /* Reset Rx FIFO. */
              | LPI2C_MCR_DBGEN_MASK /* Enable the module in debug mode. */
              | LPI2C_MCR_RST_MASK   /* Reset other control logic. */
              | LPI2C_MCR_MEN_MASK;  /* Enable the master mode. */
#endif

    /* Configure the baudrate. */
    base->MCCR0 = 0x0603060c;
    base->MCCR1 = 0x01070308; // clk & baudrate cfg

    base->MCFGR1 = LPI2C_MCFGR1_PINCFG(2)
                 | LPI2C_MCFGR1_PRESCALE(4)
                 | LPI2C_MCFGR1_AUTOSTOP_MASK
                 | LPI2C_MCFGR1_IGNACK_MASK;

    base->MFCR = LPI2C_MFCR_RXWATER(configPtr->RxFIFOWatermarkCount) /* Rx FIFO watermark. */
               | LPI2C_MFCR_TXWATER(configPtr->TxFIFOWatermarkCount);/* Tx FIFO watermark. */

    base->MCR = LPI2C_MCR_MEN_MASK;  /* nable master mode. */
}

void LPI2C_HAL_EnableRxInterrupt(LPI2C_Type *base, bool enable)
{
    if (enable)
    {
        base->MIER |= LPI2C_MIER_RDIE_MASK;
    }
    else
    {
        base->MIER &= ~LPI2C_MIER_RDIE_MASK;
    }
}

void LPI2C_HAL_WriteTxFIFOCmd(LPI2C_Type *base, LPI2C_TxFIFOCmd_T cmd, uint8_t data)
{
    base->MTDR = LPI2C_MTDR_CMD(cmd)
               | LPI2C_MTDR_DATA(data);
}

uint8_t LPI2C_HAL_ReadRxFIFOData(LPI2C_Type *base)
{
    return (uint8_t)(base->MRDR & LPI2C_MRDR_DATA_MASK);
}

/* Return true if an available data is fetched. */
bool LPI2C_HAL_ReadRxFIFODataWithCheck(LPI2C_Type *base, uint8_t *data)
{
    uint32_t tmp32;
    bool bRet = false;

    tmp32 = base->MRDR;

    if (0U == (LPI2C_MRDR_RXEMPTY_MASK & tmp32))
    {
        bRet = true;
        *data = (uint8_t)(LPI2C_MRDR_DATA_MASK & tmp32);
    }

    return bRet;
}

uint32_t LPI2C_HAL_GetStatusFlags(LPI2C_Type *base)
{
    return base->MSR;
}

void LPI2C_HAL_ClearStatusFlags(LPI2C_Type *base, uint32_t flags)
{
    base->MSR = flags;
}


/****************************************************************************
 * 
 ***************************************************************************/
void LPI2C_HAL_WriteByteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t dat)
{
    while(base->MSR & LPI2C_MSR_BBF_MASK)
    {}

    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, (devAddr << 1) | I2C_WRITE_CMD_MASK);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, regAddr);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, dat);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTOP, 0U);

    while (base->MSR & LPI2C_MSR_BBF_MASK)
    {}
}

uint8_t LPI2C_HAL_ReadByteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr)
{
    while(base->MSR & LPI2C_MSR_BBF_MASK)
    {}

    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, (devAddr << 1) | I2C_WRITE_CMD_MASK);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_TxData, regAddr);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_GenerateSTARTAndSendAddrByte, (devAddr << 1) | I2C_READ_CMD_MASK);
    LPI2C_HAL_WriteTxFIFOCmd(base, eLPI2C_Cmd_RxCountOfNplus1Byte, 0U); /* 接收1字节 */

    while(base->MSR & LPI2C_MSR_BBF_MASK)
    {}

    return LPI2C_HAL_ReadRxFIFOData(base);
}

void LPI2C_HAL_EnableRxDMA(LPI2C_Type *base, bool enable)
{
    if (enable)
    {
        base->MDER |= LPI2C_MDER_RDDE_MASK;
    }
    else
    {
        base->MDER &= ~LPI2C_MDER_RDDE_MASK;
    }
}

/* EOF. */
