/* hal_lpspi.h */
#ifndef __HAL_LPSPI_H__
#define __HAL_LPSPI_H__

#include "hal_common.h"

/* Status flags. */
#define LPSPI_FLAG_MODULE_BUSY_MASK LPSPI_SR_MBF_MASK
#define LPSPI_FLAG_DATA_MATCH_MASK  LPSPI_SR_DMF_MASK
#define LPSPI_FLAG_RX_ERROR_MASK    LPSPI_SR_REF_MASK
#define LPSPI_FLAG_TX_ERROR_MASK    LPSPI_SR_TEF_MASK
#define LPSPI_FLAG_XFER_DONE_MASK   LPSPI_SR_TCF_MASK /* LPSPI returns to idle state with the transmit FIFO empty. */
#define LPSPI_FLAG_FRAME_DONE_MASK  LPSPI_SR_FCF_MASK /* At the end of each frame transfer, when the PCS negates. */
#define LPSPI_FLAG_WORD_DONE_MASK   LPSPI_SR_WCF_MASK /* Set when the last bit of a received word is sampled. */
#define LPSPI_FLAG_RX_DONE_MASK     LPSPI_SR_RDF_MASK /* Set whenever the number of words in the Rx FIFO is greater than RXWATER. */
#define LPSPI_FLAG_TX_DONE_MASK     LPSPI_SR_TDF_MASK /* Set whenever the number of words in the Tx FIFO is equal or less than TXWATER. */

/* Interrupt events. */
#define LPSPI_INTERRUPT_DATA_MATCH_MASK  LPSPI_IER_DMIE_MASK
#define LPSPI_INTERRUPT_RX_ERROR_MASK    LPSPI_IER_REIE_MASK
#define LPSPI_INTERRUPT_TX_ERROR_MASK    LPSPI_IER_TEIE_MASK
#define LPSPI_INTERRUPT_XFER_DONE_MASK   LPSPI_IER_TCIE_MASK
#define LPSPI_INTERRUPT_FRAME_DONE_MASK  LPSPI_IER_FCIE_MASK
#define LPSPI_INTERRUPT_WORD_DONE_MASK   LPSPI_IER_WCIE_MASK
#define LPSPI_INTERRUPT_RX_DONE_MASK     LPSPI_IER_RDIE_MASK
#define LPSPI_INTERRUPT_TX_DONE_MASK     LPSPI_IER_TDIE_MASK

typedef enum
{
    /* Clock line is low when idle. Tx data line is high when idle.
     * Data valid when at rising edge.
     * CPOL = 0, CPHA = 0 */
    eLPSPI_PolPhaMode0 = 0U,
    /* Clock line is low when idle. Tx data line is high when idle.
     * Data valid when at falling edge.
     * CPOL = 0, CPHA = 1 */
    eLPSPI_PolPhaMode1 = 1U,
    /* Clock line is high when idle. Tx data line is high when idle.
     * Data valid when at falling edge.
     * CPOL = 1, CPHA = 0 */
    eLPSPI_PolPhaMode2 = 2U,
    /* Clock line is high when idle. Tx data line is high when idle.
     * Data valid when at rising edge.
     * CPOL = 1, CPHA = 1 */
    eLPSPI_PolPhaMode3 = 3U
} LPSPI_PolPhaMode_T;

typedef enum 
{
    eLPSPI_ClkDiv_1   = 0U,
    eLPSPI_ClkDiv_2   = 1U,
    eLPSPI_ClkDiv_4   = 2U,
    eLPSPI_ClkDiv_8   = 3U,
    eLPSPI_ClkDiv_16  = 4U,
    eLPSPI_ClkDiv_32  = 5U,
    eLPSPI_ClkDiv_64  = 6U,
    eLPSPI_ClkDiv_128 = 7U
} LPSPI_ClkDiv_T;

typedef struct
{
    bool     enCSHigh;        /* CS pin is high when selecting the chip. Low is default. */
    uint32_t TxFIFOWatermark;
    uint32_t RxFIFOWatermark;
} LPSPI_MasterConfig_T;

typedef struct
{
    LPSPI_PolPhaMode_T PolPhaMode; /* Polarity & Phase. */
    LPSPI_ClkDiv_T     ClkDiv;     /* Baudrate divider from root clock source. */
    uint32_t           WhichCS;    /* The available range is 0 - 3. */
    bool               enLSB;      /* MSB is default. */
    bool               enByteSwap; /* Byte swap will swap the contents of [31:24] with [7:0] and [23:16] with [15:8] for each transmit data word. */
    bool               enKeepCS;   /* Continuous transfer. */
    uint32_t           DataBitsPerCmd; /* Frame size in bit for each command. */
} LPSPI_XferCommand_T;

void LPSPI_HAL_Reset(LPSPI_Type *base);
void LPSPI_HAL_ResetTxFIFO(LPSPI_Type *base);
void LPSPI_HAL_ResetRxFIFO(LPSPI_Type *base);
void LPSPI_HAL_Enable(LPSPI_Type *base, bool enable);

uint32_t LPSPI_HAL_GetStatus(LPSPI_Type *base, uint32_t flagMask);
void LPSPI_HAL_ClearStatus(LPSPI_Type *base, uint32_t flagMask);

void LPSPI_HAL_EnableInterrupts(LPSPI_Type *base, uint32_t intMask, bool enable);
uint32_t LPSPI_HAL_GetEnabledInterrupts(LPSPI_Type *base, uint32_t intMask);

void LPSPI_HAL_EnableTxDMA(LPSPI_Type *base, bool enable);
void LPSPI_HAL_EnableRxDMA(LPSPI_Type *base, bool enable);

void LPSPI_HAL_ConfigMaster(LPSPI_Type *base, LPSPI_MasterConfig_T *configPtr);
void LPSPI_HAL_SetXferCommand(LPSPI_Type *base, LPSPI_XferCommand_T *cmdPtr);
void LPSPI_HAL_SetTxData(LPSPI_Type *base, uint32_t dat);
uint32_t LPSPI_HAL_GetRxData(LPSPI_Type *base);

#endif /* __HAL_LPSPI_H__ */
