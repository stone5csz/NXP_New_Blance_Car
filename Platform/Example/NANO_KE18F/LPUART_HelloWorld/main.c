/* main.c */
#include "app_inc.h"

void PCC_Configuration(void);
void PORT_Configuration(void);
void GPIO_Configuration(void);
void GPIO_ToggleLED(void);

int main(void)
{
    uint8_t ch;

    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);

    PCC_Configuration();
    PORT_Configuration();
    GPIO_Configuration();

    printf("\r\n\r\nHello, World.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);

    while (1)
    {
        ch = getchar();
        putchar(ch);
        GPIO_ToggleLED();
    }
}

void PCC_Configuration(void)
{
    /* PCC->PORTC |= PCC_CLKCFG_CGC_MASK;*/
    PCC->CLKCFG[PCC_PORTC_INDEX] = PCC_CLKCFG_CGC_MASK; /* Enable clock for PORTC. */
}

void PORT_Configuration(void)
{
    /* Config pins for LEDs. */
    PORTC->PCR[14] = PORT_PCR_MUX(1);
    PORTC->PCR[15] = PORT_PCR_MUX(1);
}

void GPIO_Configuration(void)
{
    GPIOC->PDDR |= (1 << 14); /* Set PTC14 as output direction. */
    GPIOC->PDDR |= (1 << 15); /* Set PTC15 as output direction. */

    GPIOC->PCOR = (1 << 14); /* Set PTC14 to output logic 0. */
    GPIOC->PSOR = (1 << 15); /* Set PTC15 to output logic 1. */
}

void GPIO_ToggleLED(void)
{
    GPIOC->PTOR = (1 << 14);
    GPIOC->PTOR = (1 << 15);
}

/* EOF. */

