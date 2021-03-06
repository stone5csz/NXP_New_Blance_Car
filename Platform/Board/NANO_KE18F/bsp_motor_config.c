/* bsp_motor_config.c */
#include "bsp_motor_config.h"

void BSP_InitMotorTurn(void)
{
    /* Enable clock. */
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTE_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTD_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_FLEXTMR0_INDEX] = PCC_CLKCFG_CGC_MASK; /* FTM0. */

    /* MOTOR_ID_A. */
    PORTE->PCR[10] = PORT_PCR_MUX(1); /* BSP_GPIO_MOTOR_A_IN1 */
    PORTE->PCR[11] = PORT_PCR_MUX(1); /* BSP_GPIO_MOTOR_A_IN2 */
    PORTD->PCR[16] = PORT_PCR_MUX(2); /* BSP_FTM_MOTOR_A */

    /* MOTOR_ID_B. */
    PORTB->PCR[5] = PORT_PCR_MUX(1); /* BSP_GPIO_MOTOR_B_IN1 */
    PORTB->PCR[4] = PORT_PCR_MUX(1); /* BSP_GPIO_MOTOR_B_IN2 */
    /* BSP_FTM_MOTOR_B */
    PORTD->PCR[15] = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;

    /* Stand by. */
    PORTD->PCR[5] = PORT_PCR_MUX(1);

}

void BSP_InitMotorQuadDecoder(void)
{
    PCC->CLKCFG[PCC_PORTE_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTB_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_FLEXTMR1_INDEX] = PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_FLEXTMR2_INDEX] = PCC_CLKCFG_CGC_MASK;

    /* MOTOR_ID_A. */
    PORTE->PCR[5] = PORT_PCR_MUX(3); /* FTM2_QD_PHA */
    PORTE->PCR[4] = PORT_PCR_MUX(3); /* FTM2_QD_PHB */

    /* MOTOR_ID_B. */
    PORTB->PCR[3] = PORT_PCR_MUX(4); /* FTM1_QD_PHA */
    PORTB->PCR[2] = PORT_PCR_MUX(4); /* FTM1_QD_PHB */
}

/* EOF. */
