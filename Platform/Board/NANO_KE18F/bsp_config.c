/* bsp_config.c */
#include "bsp_config.h"
#include "hal_lpuart.h"

static void CLK_Config_IRC48_150M(void);
static void CLK_Config_IRC48_180M(void);
static void CLK_Config_IRC48_200M(void);
#if 0
static void CLK_Config_HSRUN_OSC8MHz_160M(void);
#endif

void BSP_InitSystem(void)
{
    /* Enable all the PORT. */
    PCC->CLKCFG[PCC_PORTA_INDEX] |= PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTC_INDEX] |= PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTD_INDEX] |= PCC_CLKCFG_CGC_MASK;
    PCC->CLKCFG[PCC_PORTE_INDEX] |= PCC_CLKCFG_CGC_MASK;

#if CLOCK_SETUP==1
    CLK_Config_IRC48_150M();
#elif CLOCK_SETUP==2
    CLK_Config_HSRUN_OSC8MHz_160M();
#elif CLOCK_SETUP==3
    CLK_Config_IRC48_180M();
#elif CLOCK_SETUP==4
    CLK_Config_IRC48_200M();
#endif /* CLOCK_SETUP */
    SystemCoreClockUpdate();
	
		SCG->FIRCDIV = 0x00000100; /* Enable FIRC div2 out */
}

#if 0
static void CLK_Config_HSRUN_OSC8MHz_160M(void)
{
    /* Enable and enter into the HSRUN. */
    SMC->PMPROT = SMC_PMPROT_AHSRUN_MASK | SMC_PMPROT_AVLP_MASK; /* HSRUN and VLPR are allowed. */ 
    SMC->PMCTRL = SMC_PMCTRL_RUNM(3);    /* High speed run. */
    /* Wait till the system transfer to HSRUN. */
    while(SMC->PMSTAT != 0x80)
    {}

    /* Enable the System OSC. */
    /* Configure the Core/Bus clock when System OSC is used as the SCG's main output clock. */
    //SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK; /* Disable the System OSC before configuring it. */
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV2(0x2)  /* BusClk = OSC/2 */
                 | SCG_SOSCDIV_SOSCDIV1(0x1); /* CoreClk = OSC/1 */
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(0x3) /* High frequency range selected for the crystal oscillator of 8 Mhz to 32 Mhz. */
                 | SCG_SOSCCFG_HGO_MASK   /* Configure crystal oscillator for high-gain operation. */
                 | SCG_SOSCCFG_EREFS_MASK; /* Internal crystal oscillator of OSC requested. */
    SCG->SOSCCSR = SCG_SOSCCSR_SOSCERR_MASK /* Clear error flag. */
                 | SCG_SOSCCSR_SOSCLPEN_MASK /* System OSC is enabled in VLP modes. */
                 | SCG_SOSCCSR_SOSCSTEN_MASK /* System OSC is enabled in Stop modes if SOSCEN=1. */
                 | SCG_SOSCCSR_SOSCEN_MASK;  /* System OSC is enabled. */
    /* Wait till the SOSC output is available. */
    while (SCG_SOSCCSR_SOSCVLD_MASK != (SCG_SOSCCSR_SOSCVLD_MASK & SCG->SOSCCSR))
    {}

    /* PLL clock source: System OSC. PLL input divide factor: 1, Multiple factor: 40
     * SOSC = 8MHz, VCO = (8MHz / 1) * 40 = 320MHz, CoreClk = 320MHz / 2 = 160MHz
     */
    /* SCG->SPLLCFG[SOURCE] = 0 */ /* Configures the input clock source for the System PLL as System OSC. */
    SCG->SPLLCFG = SCG_SPLLCFG_PREDIV(0x0)   /* Pre-divide factor: 1. */
                 | SCG_SPLLCFG_MULT(0x18); /* Multiple factor: 40 */
    /* Configure the Core/Bus clock when PLL is used as the SCG's main output clock. */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV2(0x3) /* Divider for System Clock: 4, 320MHz/4 = 80MHz. */
                 | SCG_SPLLDIV_SPLLDIV1(0x2);/* Divider for Core Clock: 2, 320MHz/2 = 160MHz. */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLERR_MASK  /* Clear error flag. */
                 | SCG_SPLLCSR_SPLLSTEN_MASK /* System PLL is enabled in Stop modes */
                 | SCG_SPLLCSR_SPLLEN_MASK;  /* System PLL is enabled. */
    /* Wait till the PLL output is available. */
    while (SCG_SPLLCSR_SPLLVLD_MASK != (SCG_SPLLCSR_SPLLVLD_MASK & SCG->SPLLCSR))
    {}

    /* Config Core/Bus/Flash clock in HSRUN. */
    SCG->HCCR = SCG_HCCR_SCS(0x6) /* Selects the System PLL generating the system clock in HSRUN mode. */
              | SCG_HCCR_DIVCORE(0x0)  /* CoreCLk = PLL/2/(1) = 160MHz. */
              | SCG_HCCR_DIVBUS(0x1)   /* BusClk = CoreClk/(2) = 80MHz. */
              | SCG_HCCR_DIVSLOW(0x5); /* FlashClk = CoreClk/(6) = 26.7MHz. */
}
#endif

static void CLK_Config_IRC48_150M(void)
{
    /* Enable and enter into the HSRUN. */
    SMC->PMPROT = SMC_PMPROT_AHSRUN_MASK | SMC_PMPROT_AVLP_MASK; /* HSRUN and VLPR are allowed. */
    SMC->PMCTRL = SMC_PMCTRL_RUNM(3); /* High speed run. */
    /* Wait till the system transfer to HSRUN. */
    while(SMC->PMSTAT != 0x80)
    {}

    /* Configure the Core/Bus Clock when PLL is used as SCG's main clock output.
     * FIRC = 48MHz, PLL VCO = (FIRC / (4)) * (25) = 300MHz,
     * Core Clock = PLL VCO / 2 / (1) = 150MHz,
     * Bus Clock  = Core Clock / (2) = 75MHz
     */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2)   /* For SPLLDIV1 = PLL/2/(2) = 150MHz/(2) = 75MHz. divider=2, used for FTM only. */
                 | SCG_SPLLDIV_SPLLDIV2(2);  /* For SPLLDIV2 = PLL/2/(2) = 150MHz/(2) = 75MHz. divider=2, used for other peripherals. */
    SCG->SPLLCFG = SCG_SPLLCFG_SOURCE_MASK   /* Source: 0-System OSC; 1-Firc48M */
                 | SCG_SPLLCFG_PREDIV(3)  /* PLL prediv factor = 4 */
                 | SCG_SPLLCFG_MULT(9);   /* PLL mult factor = 25 */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN_MASK; /* Enable the PLL */
    /* Waiting until SPLL output clock is valid. */
    while(SCG_SPLLCSR_SPLLVLD_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))
    {}

    /* Configure Core/Bus/Flash clock in HSRUN. */
    SCG->HCCR = SCG_HCCR_SCS(0x6) /* Selects the System PLL generating the system clock in HSRUN mode. */
              | SCG_HCCR_DIVCORE(0x0)  /* CoreCLk = PLL/2/(1) = 150MHz. */
              | SCG_HCCR_DIVBUS(0x1)   /* BusClk = CoreClk/(2) = 75MHz. */
              | SCG_HCCR_DIVSLOW(0x5); /* FlashClk = CoreClk/(6) = 25MHz. */
    //waiting until SPLL is selected as the system clock
    while (SCG_SPLLCSR_SPLLSEL_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLSEL_MASK))
    {}
}

static void CLK_Config_IRC48_180M(void)
{
    /* Enable and enter into the HSRUN. */
    SMC->PMPROT = SMC_PMPROT_AHSRUN_MASK | SMC_PMPROT_AVLP_MASK; /* HSRUN and VLPR are allowed. */
    SMC->PMCTRL = SMC_PMCTRL_RUNM(3); /* High speed run. */
    /* Wait till the system transfer to HSRUN. */
    while(SMC->PMSTAT != 0x80)
    {}

    /* Configure the Core/Bus Clock when PLL is used as SCG's main clock output.
     * FIRC = 48MHz, PLL VCO = (FIRC / (4)) * (30) = 360MHz,
     * Core Clock = PLL VCO / 2 / (1) = 180MHz,
     * Bus Clock  = Core Clock / (2) = 90MHz
     */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2)   /* For SPLLDIV1 = PLL/2/(2) = 180MHz/(2) = 90MHz. divider=2, used for FTM only. */
                 | SCG_SPLLDIV_SPLLDIV2(2);  /* For SPLLDIV2 = PLL/2/(2) = 180MHz/(2) = 90MHz. divider=2, used for other peripherals. */
    SCG->SPLLCFG = SCG_SPLLCFG_SOURCE_MASK   /* Source: 0-System OSC; 1-Firc48M */
                 | SCG_SPLLCFG_PREDIV(3)     /* PLL prediv factor = 4 */
                 | SCG_SPLLCFG_MULT(14);     /* PLL mult factor = 30 */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN_MASK; /* Enable the PLL */
    /* Waiting until SPLL output clock is valid. */
    while(SCG_SPLLCSR_SPLLVLD_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))
    {}

    /* Configure Core/Bus/Flash clock in HSRUN. */
    SCG->HCCR = SCG_HCCR_SCS(0x6) /* Selects the System PLL generating the system clock in HSRUN mode. */
              | SCG_HCCR_DIVCORE(0x0)  /* CoreCLk = PLL/2/(1) = 180MHz. */
              | SCG_HCCR_DIVBUS(0x1)   /* BusClk = CoreClk/(2) = 90MHz. */
              | SCG_HCCR_DIVSLOW(0x6); /* FlashClk = CoreClk/(7) = 25MHz. */
    //waiting until SPLL is selected as the system clock
    while (SCG_SPLLCSR_SPLLSEL_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLSEL_MASK))
    {}
}

static void CLK_Config_IRC48_200M(void)
{
    /* Enable and enter into the HSRUN. */
    SMC->PMPROT = SMC_PMPROT_AHSRUN_MASK | SMC_PMPROT_AVLP_MASK; /* HSRUN and VLPR are allowed. */
    SMC->PMCTRL = SMC_PMCTRL_RUNM(3); /* High speed run. */
    /* Wait till the system transfer to HSRUN. */
    while(SMC->PMSTAT != 0x80)
    {}

    /* Configure the Core/Bus Clock when PLL is used as SCG's main clock output.
     * FIRC = 48MHz, PLL VCO = (FIRC / (3)) * (25) = 400MHz,
     * Core Clock = PLL VCO / 2 / (1) = 200MHz,
     * Bus Clock  = Core Clock / (2) = 100MHz
     */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2)   /* For SPLLDIV1 = PLL/2/(2) = 200MHz/(2) = 100MHz. divider=2, used for FTM only. */
                 | SCG_SPLLDIV_SPLLDIV2(2);  /* For SPLLDIV2 = PLL/2/(2) = 200MHz/(2) = 100MHz. divider=2, used for other peripherals. */
    SCG->SPLLCFG = SCG_SPLLCFG_SOURCE_MASK   /* Source: 0-System OSC; 1-Firc48M */
                 | SCG_SPLLCFG_PREDIV(2)     /* PLL prediv factor = 3 */
                 | SCG_SPLLCFG_MULT(9);     /* PLL mult factor = 25 */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN_MASK; /* Enable the PLL */
    /* Waiting until SPLL output clock is valid. */
    while(SCG_SPLLCSR_SPLLVLD_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))
    {}

    /* Configure Core/Bus/Flash clock in HSRUN. */
    SCG->HCCR = SCG_HCCR_SCS(0x6) /* Selects the System PLL generating the system clock in HSRUN mode. */
              | SCG_HCCR_DIVCORE(0x0)  /* CoreCLk = PLL/2/(1) = 200MHz. */
              | SCG_HCCR_DIVBUS(0x1)   /* BusClk = CoreClk/(2) = 100MHz. */
              | SCG_HCCR_DIVSLOW(0xf); /* FlashClk = CoreClk/(16) = 25MHz. */
    //waiting until SPLL is selected as the system clock
    while (SCG_SPLLCSR_SPLLSEL_MASK != (SCG->SPLLCSR & SCG_SPLLCSR_SPLLSEL_MASK))
    {}
}


void BSP_InitStdioLPUART(uint32_t baudrate)
{
#if 0
    /* LPUART2. */
    /* Configure pins' mux. */
    PORTD->PCR[6] = PORT_PCR_MUX(2); /* LPUART2_RX. */
    PORTD->PCR[7] = PORT_PCR_MUX(2); /* LPUART2_TX. */
    /* Configure PCC to enable clock for LPUART2. */
    PCC->CLKCFG[PCC_LPUART2_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6);
#endif

#if 0 /* LPUART1 */
    /* LPUART1. */
    /* Configure pins' mux. */
    PORTC->PCR[8] = PORT_PCR_MUX(2); /* LPUART1_RX. */
    PORTC->PCR[9] = PORT_PCR_MUX(2); /* LPUART1_TX. */
    /* Configure PCC to enable clock for LPUART1. */
    PCC->CLKCFG[PCC_LPUART1_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6);
#endif

    /* LPUART0 */
    /* Configure pins' mux. */
    PORTA->PCR[10] = PORT_PCR_MUX(3); /* LPUART0_TX. */
    PORTA->PCR[11] = PORT_PCR_MUX(3); /* LPUART0_RX. */
    /* Configure PCC to enable clock for LPUART0. */
    PCC->CLKCFG[PCC_LPUART0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6);

    /* Configure LPUART module. */
    LPUART_HAL_Reset(BSP_LPUART_DEBUG_PORT);
    LPUART_HAL_SetBaudrate(BSP_LPUART_DEBUG_PORT, BSP_CLK_SPLLDIV2_HZ, baudrate);
    LPUART_HAL_EnableTx(BSP_LPUART_DEBUG_PORT, true);
    LPUART_HAL_EnableRx(BSP_LPUART_DEBUG_PORT, true);

}

int fputc(int c, FILE *f)
{
    while (LPUART_HAL_FLAG_TX_EMPTY_MASK != (LPUART_HAL_FLAG_TX_EMPTY_MASK & LPUART_HAL_GetStatus(BSP_LPUART_DEBUG_PORT)))
    {}
    LPUART_HAL_SetTxData(BSP_LPUART_DEBUG_PORT, c);
    return c;
}

int fgetc(FILE *f)
{
    while (LPUART_HAL_FLAG_RX_FULL_MASK != (LPUART_HAL_FLAG_RX_FULL_MASK & LPUART_HAL_GetStatus(BSP_LPUART_DEBUG_PORT)))
    {}
    return LPUART_HAL_GetRxData(BSP_LPUART_DEBUG_PORT);
}

void BSP_InitGPIO_LED(void)
{
    PORTC->PCR[14] = PORT_PCR_MUX(1);
    PORTC->PCR[15] = PORT_PCR_MUX(1);
}

void BSP_InitGPIO_SWITCH(void)
{
    PORTC->PCR[9] = PORT_PCR_MUX(1);
}


/* EOF. */
