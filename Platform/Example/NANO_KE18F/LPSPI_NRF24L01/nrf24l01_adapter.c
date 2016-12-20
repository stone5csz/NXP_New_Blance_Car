/* nrf24l01_adapter.c */
#include "nrf24l01.h"
#include "app_inc.h"

#define BSP_GPIO_RF_CS_PORT  GPIOB
#define BSP_GPIO_RF_CS_PIN   0U
#define BSP_GPIO_RF_CE_PORT  GPIOE
#define BSP_GPIO_RF_CE_PIN   3U
#define BSP_GPIO_RF_IRQ_PORT GPIOE
#define BSP_GPIO_RF_IRQ_PIN  8U

/*
    device.config.baudrate = 1*1000*1000;
    device.config.data_width = 8;
    device.config.mode = SPI_MODE_0 | SPI_MASTER | SPI_MSB;
*/
void NRF24L01_HW_InitIO(void)
{
    LPSPI_MasterConfig_T lpspiMasterConfigStruct;

    /* Enable clock. */
    PCC->CLKCFG[PCC_LPSPI0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6U);
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTE_INDEX] = PCC_CLKCFG_CGC_MASK;

    /* Pin mux. */
    /* SPI. */
    //PORTB->PCR[0] = PORT_PCR_MUX(3); /* PTB0, RF_CSn,  LPSPI0_PCS0. */
    PORTB->PCR[0] = PORT_PCR_MUX(1); /* PTB0, RF_CSn,  GPIO_OUTPUT. */
    PORTB->PCR[1] = PORT_PCR_MUX(3); /* PTB1, RF_MOSI, LPSPI0_SOUT. */
    PORTE->PCR[0] = PORT_PCR_MUX(2); /* PTE0, RF_CLK,  LPSPI0_SCLK. */
    PORTE->PCR[1] = PORT_PCR_MUX(2); /* PTE1, RF_MISO, LPSPI0_SIN. */
    /* GPIO. */
    PORTE->PCR[3] = PORT_PCR_MUX(1); /* PTE3, RF_CE,  GPIO_OUTPUT. */
    PORTE->PCR[8] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* PTE8, RF_IRQ, GPIO_INPUT. */


    /* LPSPI. */
    LPSPI_HAL_Reset(LPSPI0);
    LPSPI_HAL_Enable(LPSPI0, true);

    lpspiMasterConfigStruct.enCSHigh = false;
    lpspiMasterConfigStruct.TxFIFOWatermark = 1U;
    lpspiMasterConfigStruct.RxFIFOWatermark = 0U;
    LPSPI_HAL_ConfigMaster(LPSPI0, &lpspiMasterConfigStruct);

    /* Configure GPIO. */
#if 1
    /* CS. */
    GPIO_SetPinLogic(BSP_GPIO_RF_CS_PORT, BSP_GPIO_RF_CS_PIN, true);
    GPIO_SetPinDir(BSP_GPIO_RF_CS_PORT, BSP_GPIO_RF_CS_PIN, true);
#endif
    /* CE. */
    GPIO_SetPinLogic(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, true);
    GPIO_SetPinDir(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, true);
    /* IRQ. */
    GPIO_SetPinLogic(BSP_GPIO_RF_IRQ_PORT, BSP_GPIO_RF_IRQ_PIN, false);
}

uint8_t NRF24L01_HW_SpiSwapData(uint8_t txData, bool enKeepCs)
{
    LPSPI_XferCommand_T lpspiXferCommandStruct;
    uint8_t tmp8;

    lpspiXferCommandStruct.PolPhaMode = eLPSPI_PolPhaMode0;
    lpspiXferCommandStruct.ClkDiv = eLPSPI_ClkDiv_64;
    lpspiXferCommandStruct.WhichCS = 0U;
    lpspiXferCommandStruct.enLSB = false;
    lpspiXferCommandStruct.enByteSwap = false;
    lpspiXferCommandStruct.enKeepCS = false; /* enKeepCs */
    lpspiXferCommandStruct.DataBitsPerCmd = 8U;

    GPIO_SetPinLogic(BSP_GPIO_RF_CS_PORT, BSP_GPIO_RF_CS_PIN, false); /* CS = 0. */

    /* Set the xfer command. */
    LPSPI_HAL_SetXferCommand(LPSPI0, &lpspiXferCommandStruct);

    /* Fill the data into Tx FIFO. */
    while (LPSPI_FLAG_TX_DONE_MASK != LPSPI_HAL_GetStatus(LPSPI0, LPSPI_FLAG_TX_DONE_MASK))
    {}
    LPSPI_HAL_SetTxData(LPSPI0, txData);

    /* Get the data from Rx FIFO. */
    while (LPSPI_FLAG_RX_DONE_MASK != LPSPI_HAL_GetStatus(LPSPI0, LPSPI_FLAG_RX_DONE_MASK))
    {}
    tmp8 = LPSPI_HAL_GetRxData(LPSPI0);
			
    if (!enKeepCs)
    {
        GPIO_SetPinLogic(BSP_GPIO_RF_CS_PORT, BSP_GPIO_RF_CS_PIN, true); /* CS = 1. */
    }

    return tmp8;
    /* return 0xFF; */
}

void NRF24L01_HW_GpioSetCEPin(bool enLogicOne)
{
    GPIO_SetPinLogic(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, enLogicOne);
}

bool NRF24L01_HW_GpioGetIrqPin(void)
{
    GPIO_GetPinLogic(BSP_GPIO_RF_IRQ_PORT, BSP_GPIO_RF_IRQ_PIN);
    return false;
}

/* EOF. */

