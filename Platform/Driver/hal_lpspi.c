/* hal_lpspi.c */
#include "hal_lpspi.h"

void LPSPI_HAL_Reset(LPSPI_Type *base)
{
    base->CR = LPSPI_CR_RRF_MASK
             | LPSPI_CR_RTF_MASK
             | LPSPI_CR_RST_MASK;
    base->CR = LPSPI_CR_DBGEN_MASK;
}

void LPSPI_HAL_ResetTxFIFO(LPSPI_Type *base)
{
    base->CR |= LPSPI_CR_RTF_MASK;
}
void LPSPI_HAL_ResetRxFIFO(LPSPI_Type *base)
{
    base->CR |= LPSPI_CR_RRF_MASK;
}

void LPSPI_HAL_Enable(LPSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->CR |= LPSPI_CR_MEN_MASK;
    }
    else
    {
        base->CR &= ~LPSPI_CR_MEN_MASK;
    }
}

uint32_t LPSPI_HAL_GetStatus(LPSPI_Type *base, uint32_t flagMask)
{
    return (flagMask & base->SR);
}
void LPSPI_HAL_ClearStatus(LPSPI_Type *base, uint32_t flagMask)
{
    base->SR = flagMask;
}

void LPSPI_HAL_EnableInterrupts(LPSPI_Type *base, uint32_t intMask, bool enable)
{
    if (enable)
    {
        base->IER |= intMask;
    }
    else
    {
        base->IER &= ~intMask;
    }
}

uint32_t LPSPI_HAL_GetEnabledInterrupts(LPSPI_Type *base, uint32_t intMask)
{
    return (intMask & base->IER);
}

void LPSPI_HAL_EnableTxDMA(LPSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->DER |= LPSPI_DER_TDDE_MASK;
    }
    else
    {
        base->DER &= ~LPSPI_DER_TDDE_MASK;
    }
}
void LPSPI_HAL_EnableRxDMA(LPSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->DER |= LPSPI_DER_RDDE_MASK;
    }
    else
    {
        base->DER &= ~LPSPI_DER_RDDE_MASK;
    } 
}

void LPSPI_HAL_ConfigMaster(LPSPI_Type *base, LPSPI_MasterConfig_T *configPtr)
{
    uint32_t tmp32 = 0U;

    base->CFGR0 = 0U;
    
    tmp32 = LPSPI_CFGR1_MASTER_MASK;
    if (configPtr->enCSHigh)
    {
        tmp32 |= LPSPI_CFGR1_PCSPOL_MASK;
    }
    base->CFGR1 = tmp32;

    base->FCR = LPSPI_FCR_RXWATER(configPtr->RxFIFOWatermark)
              | LPSPI_FCR_TXWATER(configPtr->TxFIFOWatermark);
    base->CCR = LPSPI_CCR_SCKPCS(0)
              | LPSPI_CCR_PCSSCK(0)
              | LPSPI_CCR_DBT(0)
              | LPSPI_CCR_SCKDIV(0);
}

void LPSPI_HAL_SetXferCommand(LPSPI_Type *base, LPSPI_XferCommand_T *cmdPtr)
{
    uint32_t tmp32 = 0U;

    /* CPOL & CPHA. */
    switch (cmdPtr->PolPhaMode)
    {
    case eLPSPI_PolPhaMode0:
        break;
    case eLPSPI_PolPhaMode1:
        tmp32 |= LPSPI_TCR_CPHA_MASK;
        break;
    case eLPSPI_PolPhaMode2:
        tmp32 |= LPSPI_TCR_CPOL_MASK;
        break;
    case eLPSPI_PolPhaMode3:
        tmp32 |= LPSPI_TCR_CPHA_MASK | LPSPI_TCR_CPOL_MASK;
        break;
    default:
        break;
    }

    /* PRESCALE. */
    tmp32 |= LPSPI_TCR_PRESCALE(cmdPtr->ClkDiv);

    /* PCS */
    tmp32 |= LPSPI_TCR_PCS(cmdPtr->WhichCS);

    /* LSBF */
    if (cmdPtr->enLSB)
    {
        tmp32 |= LPSPI_TCR_LSBF_MASK;
    }

    /* Byte swap. */
    if (cmdPtr->enByteSwap)
    {
        tmp32 |= LPSPI_TCR_BYSW_MASK;
    }

    /* Continuous Transfer.
    * In master mode, continuous transfer will keep the PCS asserted at the end of the frame size,
    * until a command word is received that starts a new frame.
    */
    if (cmdPtr->enKeepCS)
    {
        tmp32 |= LPSPI_TCR_CONT_MASK;
    }

    /* Continuing Command.
    * This feature is disabled in the driver.
    * The default condition is to use the command word for start of new transfer.
    */
   
    /* RXMSK & TXMSK.
    * These features are disabled in the driver.
    * The default condition is to use the normal transfer.
    */
   
    /* Transfer Width - WIDTH
    * The defualt condition is to use the single bit transfer.
    */
   
    /* Frame Size - FRAMESZ
    * Configures the frame size in number of bits equal to (FRAMESZ + 1). The minimum frame size is
    * 8 bits. If the frame size is larger than 32 bits, data will be loaded from the transmit FIFO
    * and stored to the receive FIFO every 32 bits. If the size of the transfer word is not
    * divisible by 32, then the last load of the transmit FIFO and store of the receive FIFO will
    * contain the remainder bits. */
    tmp32 |= LPSPI_TCR_FRAMESZ(cmdPtr->DataBitsPerCmd-1U);

    base->TCR = tmp32;
}

void LPSPI_HAL_SetTxData(LPSPI_Type *base, uint32_t dat)
{
    base->TDR = dat;
}

uint32_t LPSPI_HAL_GetRxData(LPSPI_Type *base)
{
    return base->RDR;
}

/* EOF. */
