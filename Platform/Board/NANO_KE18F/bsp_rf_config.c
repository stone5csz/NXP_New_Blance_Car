/* bsp_rf_config.c */
#include "bsp_rf_config.h"

void BSP_InitRF(void)
{
    /* Enable clock. */
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTE_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_LPSPI0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(3U); /* LPSPI0. FIRC */

    /* CE. */
    PORTE->PCR[3] = PORT_PCR_MUX(1); /* CE */

    /* SPI Interface. */
    PORTB->PCR[0] = PORT_PCR_MUX(3); /* CSn  */
    PORTB->PCR[1] = PORT_PCR_MUX(3); /* MOSI */
    PORTE->PCR[1] = PORT_PCR_MUX(2); /* MISO */
    PORTE->PCR[0] = PORT_PCR_MUX(2); /* SCK  */
}

