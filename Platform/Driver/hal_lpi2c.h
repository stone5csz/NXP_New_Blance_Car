/* HAL_lpi2c.h */
#ifndef __HAL_LPI2C_H__
#define __HAL_LPI2C_H__

#include "hal_common.h"

#define I2C_READ_CMD_MASK                (0x01)
#define I2C_WRITE_CMD_MASK               (0x00)

typedef struct
{
    uint32_t BusClkHz;
    uint32_t Baudrate;
    uint32_t TxFIFOWatermarkCount; /* The Tx flag would be asserted when Tx data is more then this count. */
    uint32_t RxFIFOWatermarkCount; /* The Rx flag would be asserted when Tx data is more then this count. */
} LPI2C_MasterConfig_T;

typedef enum
{
    eLPI2C_Cmd_TxData = 0U, /* Transmit DATA[7:0]. */
    eLPI2C_Cmd_RxCountOfNplus1Byte = 1U, /* Receive (DATA[7:0] + 1) bytes. */
    eLPI2C_Cmd_GenerateSTOP = 2U, /* Generate STOP condition. */

    /* Receive and discard (DATA[7:0] + 1) bytes. */
    eLPI2C_Cmd_RxAndDiscardCountOfNplus1Byte = 3U,

    /* Generate (repeated) START and transmit address in DATA[7:0]. */
    eLPI2C_Cmd_GenerateSTARTAndSendAddrByte = 4U,

    /*
    * Generate (repeated) START and transmit address in DATA[7:0].
    * This transfer expects a NACK to returned.
    */
    eLPI2C_Cmd_GenerateSTARTAndSendAddrByteWithNACKBack = 5U,

    /*
    * Generate (repeated) START and transmit address in DATA[7:0]
    * using high speed mode.
    */
    eLPI2C_Cmd_GenerateSTARTAndSendAddrByteInHighSpeed = 6U,

    /*
    * Generate (repeated) START and transmit address in DATA[7:0]
    * using high speed mode. This transfer expects a NACK to be returned.
    */
    eLPI2C_Cmd_GenerateSTARTAndSendAddrByteWithNACKBackInHighSpeed = 7U,
} LPI2C_TxFIFOCmd_T;

void LPI2C_HAL_ConfigMaster(LPI2C_Type *base, LPI2C_MasterConfig_T *configPtr);
void LPI2C_HAL_EnableRxInterrupt(LPI2C_Type *base, bool enable);
void LPI2C_HAL_ResetTxFIFO(LPI2C_Type *base);
void LPI2C_HAL_ResetRxFIFO(LPI2C_Type *base);
void LPI2C_HAL_WriteTxFIFOCmd(LPI2C_Type *base, LPI2C_TxFIFOCmd_T cmd, uint8_t data);
uint8_t LPI2C_HAL_ReadRxFIFOData(LPI2C_Type *base);
bool LPI2C_HAL_ReadRxFIFODataWithCheck(LPI2C_Type *base, uint8_t *data);
uint32_t LPI2C_HAL_GetStatusFlags(LPI2C_Type *base);

void LPI2C_HAL_WriteByteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr, uint8_t dat);
uint8_t LPI2C_HAL_ReadByteBlocking(LPI2C_Type *base, uint8_t devAddr, uint8_t regAddr);

void LPI2C_HAL_EnableRxDMA(LPI2C_Type *base, bool enable);

#endif /* __HAL_LPI2C_H__ */
