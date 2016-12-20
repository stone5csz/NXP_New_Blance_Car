/* main.c */
#include "app_inc.h"

static void FTM_Configuration(void);

int main(void)
{
    int32_t decoderValue;

    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);
    
    printf("\r\n\r\nFTM_QuadDecoder Example.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);

    FTM_Configuration();

    while (1)
    {
        getchar();

        decoderValue = FTM_GetQuadDecoderValue(FTM1);
        FTM1->CNT = 0U;
        printf("decoderValue = %d\r\n", decoderValue);


    }
}

/* Only FTM1 and FTM2 have the feature of QuadDecoder. */
static void FTM_Configuration(void)
{
    FTM_TimerConfig_T ftmTimerConfigStruct;
    FTM_QuadDecoderConfig_T ftmQuadDecoderConfigStruct;

    /* Enable clock. */
    PCC->CLKCFG[PCC_FLEXTMR1_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;

    /* Pin mux. */
    PORTB->PCR[2] = PORT_PCR_MUX(4) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* FTM1_QD_PHB. */
    PORTB->PCR[3] = PORT_PCR_MUX(4) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* FTM1_QD_PHA. */

    /* FTM module. */
    ftmTimerConfigStruct.SrcClkDivider = eFTM_SrcClkDiv_32;
    ftmTimerConfigStruct.ModuloValue = 10000U;
    ftmTimerConfigStruct.InitValue =  0U;
    ftmTimerConfigStruct.TimerCountingMode = eFTM_TimerCountingUp;
    FTM_ConfigTimer(FTM1, &ftmTimerConfigStruct);

    ftmQuadDecoderConfigStruct.enPhAInputFilter = false;
    ftmQuadDecoderConfigStruct.enPhBInputFilter = false;
    ftmQuadDecoderConfigStruct.enPhAInvertPol = false;
    ftmQuadDecoderConfigStruct.enPhBInvertPol = false;
    ftmQuadDecoderConfigStruct.QuadEncodingMode = eFTM_QuadEncodingPhAPhB;
    ftmQuadDecoderConfigStruct.enInvertDir = true; /* Increase. */
    ftmQuadDecoderConfigStruct.QuadDecoderOverflowEvent = eFTM_QuadDecoderOverflowOnCountingUp;
    FTM_ConfigQuadDecoderMode(FTM1, &ftmQuadDecoderConfigStruct);

    FTM1->CNT = 0U;

    FTM_StartTimer(FTM1, eFTM_ClkSrcAlt1);  /* System clock. */
}

/* EOF. */

