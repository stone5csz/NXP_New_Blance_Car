/* bsp_config.h */
#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include "fsl_device_registers.h"
#include <stdio.h>

#define CLOCK_SETUP 1

#ifndef CLOCK_SETUP
#define CLOCK_SETUP 0
#endif /* CLOCK_SETUP */

#if CLOCK_SETUP==0

#elif CLOCK_SETUP==1

#define BSP_CLK_OSCCLK_HZ    8000000UL
#define BSP_CLK_CORECLK_HZ   150000000UL
#define BSP_CLK_BUSCLK_HZ    75000000UL

#define BSP_CLK_SPLLDIV2_HZ  75000000UL
#define BSP_CLK_SOSCDIV2_HZ  /* External OSC, not used. */
#define BSP_CLK_SIRCDIV2_HZ  /* Disabled. */
#define BSP_CLK_FIRCDIV2_HZ  48000000UL

#elif CLOCK_SETUP==2

#define BSP_CLK_SPLLDIV2_HZ  320000000UL
#define BSP_CLK_SOSCDIV2_HZ  /* External OSC, not used. */
#define BSP_CLK_SIRCDIV2_HZ  /* Disabled. */
#define BSP_CLK_FIRCDIV2_HZ  48000000UL

#define BSP_CLK_OSCCLK_HZ    8000000UL
#define BSP_CLK_CORECLK_HZ   160000000UL
#define BSP_CLK_BUSCLK_HZ    80000000UL

#elif CLOCK_SETUP==3
#define BSP_CLK_OSCCLK_HZ    8000000UL
#define BSP_CLK_CORECLK_HZ   180000000UL
#define BSP_CLK_BUSCLK_HZ    90000000UL

#define BSP_CLK_SPLLDIV2_HZ  90000000UL
#define BSP_CLK_SOSCDIV2_HZ  /* External OSC, not used. */
#define BSP_CLK_SIRCDIV2_HZ  /* Disabled. */
#define BSP_CLK_FIRCDIV2_HZ  48000000UL

#elif CLOCK_SETUP==4
#define BSP_CLK_OSCCLK_HZ    8000000UL
#define BSP_CLK_CORECLK_HZ   200000000UL
#define BSP_CLK_BUSCLK_HZ    100000000UL

#define BSP_CLK_SPLLDIV2_HZ  100000000UL
#define BSP_CLK_SOSCDIV2_HZ  /* External OSC, not used. */
#define BSP_CLK_SIRCDIV2_HZ  /* Disabled. */
#define BSP_CLK_FIRCDIV2_HZ  48000000UL

#endif /* CLOCK_SETUP */

/*
* LPUART.
*/
#define BSP_LPUART_DEBUG_PORT LPUART0 /* LPUART1 */

/*
* GPIO.
*/
#define BSP_GPIO_LED1_PORT  GPIOC
#define BSP_GPIO_LED1_PIN   14U
#define BSP_GPIO_LED2_PORT  GPIOC
#define BSP_GPIO_LED2_PIN   15U
#define BSP_GPIO_SWITCH_PORT	GPIOC
#define BSP_GPIO_SWITCH_PIN		9U

void BSP_InitSystem(void);
void BSP_InitStdioLPUART(uint32_t baudrate);
void BSP_InitGPIO_LED(void);
void BSP_InitGPIO_SWITCH(void);


// PTA0 -- ADC0 CH0
// PTC8 -- ADC2 CH14

#endif /* __BSP_CONFIG_H__ */

