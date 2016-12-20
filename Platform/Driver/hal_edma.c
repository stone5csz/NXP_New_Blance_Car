/* hal_edma.c */

#include "hal_edma.h"

void DMAMUX_HAL_SetTriggerForDMA(uint32_t chnIdx, uint32_t trigger)
{
    if (0U == trigger)  /* eDMAMUX_TriggerSource_Disabled */
    {
        DMAMUX->CHCFG[chnIdx] = 0U;
    }
    else if (64U == trigger) /* eDMAMUX_TriggerSource_PIT */
    {
        DMAMUX->CHCFG[chnIdx] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE_MASK;
    }
    else
    {
        DMAMUX->CHCFG[chnIdx] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(trigger);
    }
}

/*
* EDMA.
*/

void EDMA_HAL_ResetEngine(void)
{
    DMA0->CR = 0U;
    DMA0->ERQ = 0U;
    DMA0->EEI = 0U;
    DMA0->INT = 0xFFU;
    DMA0->ERR = 0xFFU;
}

void EDMA_HAL_EnableMinorLoopOffset(bool enable)
{
    if (enable)
    {
        DMA0->CR |= DMA_CR_EMLM_MASK;
    }
    else
    {
        DMA0->CR &= ~DMA_CR_EMLM_MASK;
    }
}

void EDMA_HAL_ResetChannel(uint32_t chnIdx)
{
    DMA0->TCD[chnIdx].CSR = 0U;
}

bool EDMA_HAL_ConfigTransfer(uint32_t chnIdx, EDMA_TransferConfig_T *configPtr)
{
    uint32_t tmp32;

    /* Configure the minor loop. */
    /* Address Offset after minor loop. */
    if ( (configPtr->SrcAddrMinorLoopDoneOffset == 0) && (configPtr->DestAddrMinorLoopDoneOffset == 0) )
    {
        /* Once using the minor loop, the EMLM bit should be set. */
        if (0U != (DMA_CR_EMLM_MASK & DMA0->CR))
        {
            return false;
        }
        /*Minor Loop Disabled, use DMA_TCDn_NBYTES_MLNO. */
        DMA0->TCD[chnIdx].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(configPtr->MinorLoopByteCount);
    }
    else
    {
        if (0U == (DMA_CR_EMLM_MASK & DMA0->CR))
        {
            return false;
        }
        /* Minor Loop enabled, use DMA_TCDn_NBYTES_MLOFFYES. */
        if ( (configPtr->SrcAddrMinorLoopDoneOffset != 0) && (configPtr->DestAddrMinorLoopDoneOffset != 0) )
        {
            if (configPtr->SrcAddrMinorLoopDoneOffset != configPtr->DestAddrMinorLoopDoneOffset)
            {
                return false;
            }
        }
        tmp32 = 0U;
        if (configPtr->SrcAddrMinorLoopDoneOffset != 0)
        {
            tmp32 |= DMA_NBYTES_MLOFFYES_SMLOE_MASK 
                   | DMA_NBYTES_MLOFFYES_MLOFF(configPtr->SrcAddrMinorLoopDoneOffset);
        }
        if (configPtr->DestAddrMinorLoopDoneOffset != 0)
        {
            tmp32 |= DMA_NBYTES_MLOFFYES_DMLOE_MASK
                   | DMA_NBYTES_MLOFFYES_MLOFF(configPtr->DestAddrMinorLoopDoneOffset); 
        }
        DMA0->TCD[chnIdx].NBYTES_MLOFFYES = tmp32
                                          | DMA_NBYTES_MLOFFYES_NBYTES(configPtr->MinorLoopByteCount);
    }

    /* Configure the major loop. */
    DMA0->TCD[chnIdx].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(configPtr->MinorLoopCount);
    DMA0->TCD[chnIdx].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(configPtr->MinorLoopCount);

    DMA0->TCD[chnIdx].SLAST = configPtr->SrcAddrMajorLoopDoneOffset;
    DMA0->TCD[chnIdx].DLAST_SGA = configPtr->DestAddrMajorLoopDoneOffset;

    /* Configure the basic attributes of general DMA function. */
    /* Start address. */
    DMA0->TCD[chnIdx].SADDR = configPtr->SrcAddr;
    DMA0->TCD[chnIdx].DADDR = configPtr->DestAddr;

    /* Address Offset after each transfer. */
    DMA0->TCD[chnIdx].SOFF = configPtr->SrcAddrIncPerTransfer;
    DMA0->TCD[chnIdx].DOFF = configPtr->DestAddrIncPerTransfer;

    DMA0->TCD[chnIdx].ATTR =  DMA_ATTR_SMOD(configPtr->SrcAddrCycleMode)
                            | DMA_ATTR_SSIZE(configPtr->SrcBusWidthMode)
                            | DMA_ATTR_DMOD(configPtr->DestAddrCycleMode)
                            | DMA_ATTR_DSIZE(configPtr->DestBusWidthMode);
    
    return true;

}

bool EDMA_HAL_IsMajorLoopDone(uint32_t chnIdx)
{
    return (0U != (DMA_CSR_DONE_MASK & DMA0->TCD[chnIdx].CSR ));
}

void EDMA_HAL_ClearMajorLoopDoneFlag(uint32_t chnIdx)
{
    DMA0->CDNE = DMA_CDNE_CDNE(chnIdx);
}

/* Enable interrupt on half transfer is done. */
void EDMA_HAL_EnableInterruptOnHalfMajorLoopDone(uint32_t chnIdx, bool enable)
{
    if (enable)
    {
        DMA0->TCD[chnIdx].CSR |= DMA_CSR_INTHALF_MASK;
    }
    else
    {
        DMA0->TCD[chnIdx].CSR &= ~DMA_CSR_INTHALF_MASK;
    }
}

/* Enable interrupt on total transfer is done. */
void EDMA_HAL_EnableInterruptOnMajorLoopDone(uint32_t chnIdx, bool enable)
{
    if (enable)
    {
        DMA0->TCD[chnIdx].CSR |= DMA_CSR_INTMAJOR_MASK;
    }
    else
    {
        DMA0->TCD[chnIdx].CSR &= ~DMA_CSR_INTMAJOR_MASK;
    }
}

void EDMA_HAL_ClearInterruptFlagOfMajorLoopDone(uint32_t chnIdx)
{
    DMA0->CINT = DMA_CINT_CINT(chnIdx);
}

void EDMA_HAL_DoSoftTriggerCmd(uint32_t chnIdx)
{
    //DMA0->TCD[chnIdx].CSR |= DMA_CSR_START_MASK;
    DMA0->SSRT = DMA_SSRT_SSRT(chnIdx);
}

void EDMA_HAL_EnableHwTrigger(uint32_t chnIdx, bool enable)
{
    if (enable)
    {
        DMA0->SERQ = DMA_SERQ_SERQ(chnIdx);
    }
    else
    {
        DMA0->CERQ = DMA_CERQ_CERQ(chnIdx);
    }
}

void EDMA_HAL_SetHwTriggerAutoDisabledOnMajorLoopDone(uint32_t chnIdx, bool enable)
{
    if (enable)
    {
        DMA0->TCD[chnIdx].CSR |= DMA_CSR_DREQ_MASK;
    }
    else
    {
        DMA0->TCD[chnIdx].CSR &= ~DMA_CSR_DREQ_MASK;
    }
}
/* EOF. */

