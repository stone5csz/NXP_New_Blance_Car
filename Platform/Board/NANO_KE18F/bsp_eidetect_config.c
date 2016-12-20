/* bsp_eid_config.c */
#include "bsp_eidetect_config.h"

void BSP_InitEIDetect(void)
{
    /* Enable clock. */
    PCC->CLKCFG[PCC_PORTA_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTC_INDEX] = PCC_CLKCFG_CGC_MASK;
		PCC->CLKCFG[PCC_ADC0_INDEX] = 0U;		// Disable ADC0 clock before reconfigure PCC
		PCC->CLKCFG[PCC_ADC2_INDEX] = 0U;		// Disable ADC2 clock before reconfigure PCC
    PCC->CLKCFG[PCC_ADC0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(3U); /* ADC0. FIRC */
    PCC->CLKCFG[PCC_ADC2_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(3U); /* ADC2. FIRC */

    /* EID_ID_LEFT. */
    PORTA->PCR[0] = PORT_PCR_MUX(0); /* EID_LEFT */

    /* EID_ID_RIGHT. */
    PORTC->PCR[8] = PORT_PCR_MUX(0); /* EID_RIGHT */

}

/* EOF. */
