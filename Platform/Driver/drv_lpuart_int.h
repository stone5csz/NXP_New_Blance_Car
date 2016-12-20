/* drv_lpuart_fifo.h */
#ifndef __DRV_LPUART_FIFO_H__
#define __DRV_LPUART_FIFO_H__

#include "hal_lpuart_fifo.h"
#include "rbuf.h"

typedef struct
{
    uint32_t BusClkHz;
    uint32_t Baudrate;
} LPUART_DRV_InitHwConfig_T;

typedef struct
{
    /* Hardware. */
    LPUART_Type *DevPtr; /* [In] */

    /* Tx Buffer */
    uint32_t TxBufLen; /* [In] */
    uint8_t *TxBufPtr; /* [In] */

    /* Rx Buffer. */
    uint32_t RxBufLen; /* [In] */
    uint8_t *RxBufPtr; /* [In] */

    /* Internal FIFO. */
    RBUF_Handler_T TxFIFO;
    RBUF_Handler_T RxFIFO;
} LPUART_DRV_XferHandler_T;

void LPUART_DRV_InitXfer(LPUART_DRV_XferHandler_T *handler, LPUART_DRV_InitHwConfig_T *hwConfigPtr);
void LPUART_DRV_DeinitXfer(LPUART_DRV_XferHandler_T *handler);
void LPUART_DRV_XferIRQHandler(LPUART_DRV_XferHandler_T *handler);

uint32_t LPUART_DRV_Write(LPUART_DRV_XferHandler_T *handler, uint8_t *txBuf, uint32_t txLen);
uint32_t LPUART_DRV_Read(LPUART_DRV_XferHandler_T *handler, uint8_t *rxBuf, uint32_t rxLen);
void LPUART_DRV_FlushWrite(LPUART_DRV_XferHandler_T *handler);
void LPUART_DRV_FlushRead(LPUART_DRV_XferHandler_T *handler);
#endif /* __DRV_LPUART_FIFO_H__ */
