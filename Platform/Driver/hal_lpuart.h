/* hal_lpuart.h */
#ifndef __HAL_LPUART_H__
#define __HAL_LPUART_H__

#include "hal_common.h"

#define LPUART_HAL_INT_TX_EMPTY_MASK LPUART_CTRL_TIE_MASK
#define LPUART_HAL_INT_RX_FULL_MASK LPUART_CTRL_RIE_MASK

#define LPUART_HAL_FLAG_TX_EMPTY_MASK LPUART_STAT_TDRE_MASK
#define LPUART_HAL_FLAG_RX_FULL_MASK LPUART_STAT_RDRF_MASK

void LPUART_HAL_Reset(LPUART_Type *base);
void LPUART_HAL_SetBaudrate(LPUART_Type *base, uint32_t busClkHz, uint32_t baudrate);
void LPUART_HAL_EnableInterrupts(LPUART_Type *base, uint32_t intMask, bool enable);

void LPUART_HAL_EnableTx(LPUART_Type *base, bool enable);
void LPUART_HAL_EnableRx(LPUART_Type *base, bool enable);

uint32_t LPUART_HAL_GetStatus(LPUART_Type *base);
bool LPUART_HAL_GetTxInterruptEnabled(LPUART_Type *base);
void LPUART_HAL_SetTxData(LPUART_Type *base, uint8_t dat);
uint8_t LPUART_HAL_GetRxData(LPUART_Type *base);

#endif /* __HAL_LPUART_H__ */
