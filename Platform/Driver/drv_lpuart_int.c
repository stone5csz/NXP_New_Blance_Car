/* drv_lpuart_fifo.c */
#include "drv_lpuart_fifo.h"
#include "rbuf.h"

void LPUART_DRV_InitXfer(LPUART_DRV_XferHandler_T *handler, LPUART_DRV_InitHwConfig_T *hwConfigPtr)
{
    /* Initialize FIFO. */
    RBUF_Init(&handler->TxFIFO, handler->TxBufPtr, handler->TxBufLen);
    RBUF_Init(&handler->RxFIFO, handler->RxBufPtr, handler->RxBufLen);

    /* Initialize hardware. */
    LPUART_HAL_Reset(handler->DevPtr);
    LPUART_HAL_SetBaudrate(handler->DevPtr, hwConfigPtr->BusClkHz, hwConfigPtr->Baudrate);
    LPUART_HAL_EnableRx(handler->DevPtr, true);
}

void LPUART_DRV_DeinitXfer(LPUART_DRV_XferHandler_T *handler)
{
    LPUART_HAL_Reset(handler->DevPtr);
}

void LPUART_DRV_XferIRQHandler(LPUART_DRV_XferHandler_T *handler)
{
    uint8_t tmp8;

    if (LPUART_HAL_GetRxFullFlag(handler->DevPtr))
    {
        tmp8 = LPUART_HAL_GetRxData(handler->DevPtr); /* Clear Rx flag. */
        if (!RBUF_IsFull(&handler->RxFIFO))
        {
            RBUF_PutDataIn(&handler->RxFIFO, tmp8);
        }
    }

    /* Tx Process. */
    if (LPUART_HAL_GetTxInterruptEnabled(handler->DevPtr))
    {
        if (RBUF_IsEmpty(&handler->TxFIFO))
        {
            LPUART_HAL_EnableTxEmptyInterrupt(handler->DevPtr, false);
        }
        else
        {
            tmp8 = RBUF_GetDataOut(&handler->TxFIFO);
            LPUART_HAL_SetTxData(handler->DevPtr, tmp8); /* Clear Tx flag. */
        }
    }
}

uint32_t LPUART_DRV_Write(LPUART_DRV_XferHandler_T *handler, uint8_t *txBuf, uint32_t txLen)
{
    uint32_t i;

    /* Move the data to Uart Tx Buffer. */
    for (i = 0U; i < txLen; i++)
    {
        if (!RBUF_IsFull(&handler->TxFIFO))
        {
            RBUF_PutDataIn(&handler->TxFIFO, txBuf[i]);
        }
        else
        {
            break;
        }
    }
    /* Enable the Uart Tx interrupt to transfer the data. */
    if (!LPUART_HAL_GetTxInterruptEnabled(handler->DevPtr))
    {
        LPUART_HAL_EnableTxEmptyInterrupt(handler->DevPtr, true);
    }
    return i; /* Return the count of available Tx data. */
}

uint32_t LPUART_DRV_Read(LPUART_DRV_XferHandler_T *handler, uint8_t *rxBuf, uint32_t rxLen)
{
    for (i = 0U; i < rxLen; i++)
    {
        if (!RBUF_IsEmpty(&handler->RxFIFO))
        {
            rxBuf[i] = RBUF_GetDataOut(&handler->RxFIFO);
        }
        else
        {
            break;
        }
    }
    return i; /* Return the count of available Rx data. */
}

/* Wait for the Tx FIFO. */
void LPUART_DRV_FlushWrite(LPUART_DRV_XferHandler_T *handler)
{
    while (!RBUF_IsEmpty(&handler->TxFIFO))
}

/* Read out the Rx FIFO. */
void LPUART_DRV_FlushRead(LPUART_DRV_XferHandler_T *handler)
{
    while (!RBUF_IsEmpty(&handler->RxFIFO))
    {
        RBUF_GetDataOut(&handler->RxFIFO);
    }
}

/* EOF. */
