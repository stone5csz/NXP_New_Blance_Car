/* main.c */
#include "app_inc.h"

static void LPSPI_Configuration(void);

int main(void)
{
    LPSPI_XferCommand_T lpspiXferCommandStruct;
    volatile uint32_t tmp32;

    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);

    printf("\r\n\r\nLPSPI_SelfLoop Example.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);
	
    LPSPI_Configuration();

    lpspiXferCommandStruct.PolPhaMode = eLPSPI_PolPhaMode0;
    lpspiXferCommandStruct.ClkDiv = eLPSPI_ClkDiv_64;
    lpspiXferCommandStruct.WhichCS = 0U;
    lpspiXferCommandStruct.enLSB = false;
    lpspiXferCommandStruct.enByteSwap = false;
    lpspiXferCommandStruct.enKeepCS = false;
    lpspiXferCommandStruct.DataBitsPerCmd = 8U;

    while (1)
    {
        getchar();

        /* Set the xfer command. */
        LPSPI_HAL_SetXferCommand(LPSPI0, &lpspiXferCommandStruct);

        /* Fill the data into Tx FIFO. */
        while (LPSPI_FLAG_TX_DONE_MASK != LPSPI_HAL_GetStatus(LPSPI0, LPSPI_FLAG_TX_DONE_MASK))
        {}
        LPSPI_HAL_SetTxData(LPSPI0, 0x1234);
        /* In 8-bit mode, only the "0x34" would be shifted out. */

        /* Get the data from Rx FIFO. */
        while (LPSPI_FLAG_RX_DONE_MASK != LPSPI_HAL_GetStatus(LPSPI0, LPSPI_FLAG_RX_DONE_MASK))
        {}
        tmp32 = LPSPI_HAL_GetRxData(LPSPI0);

        if (tmp32 == 0x34)
        {
            printf("Succeed.\r\n");
        }
        else
        {
            printf("Failed.\r\n");
        }
    }
}

static void LPSPI_Configuration(void)
{
    LPSPI_MasterConfig_T lpspiMasterConfigStruct;

    /* Enable clock. */
    PCC->CLKCFG[PCC_LPSPI0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6U);
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTE_INDEX] = PCC_CLKCFG_CGC_MASK;

    /* Pin mux. */
    PORTB->PCR[0] = PORT_PCR_MUX(3); /* LPSPI0_PCS0. */
    PORTB->PCR[1] = PORT_PCR_MUX(3); /* LPSPI0_SOUT. */
    PORTE->PCR[0] = PORT_PCR_MUX(2); /* LPSPI0_SCLK. */
    PORTE->PCR[1] = PORT_PCR_MUX(2); /* LPSPI0_SIN. */

    /* LPSPI. */
    LPSPI_HAL_Reset(LPSPI0);
    LPSPI_HAL_Enable(LPSPI0, true);

    lpspiMasterConfigStruct.enCSHigh = false;
    lpspiMasterConfigStruct.TxFIFOWatermark = 1U;
    lpspiMasterConfigStruct.RxFIFOWatermark = 0U;
    LPSPI_HAL_ConfigMaster(LPSPI0, &lpspiMasterConfigStruct);

}



/* EOF. */

