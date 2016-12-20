/* main.c */
#include "app_inc.h"

#define DEMO_LPIT_CHANNEL_NUMBER 0U

static void NVIC_Configuration(void);
static void LPIT_Configuration(void);
static void GPIO_Configuration(void);

int main(void)
{
    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);
    BSP_InitGPIO_LED();

    NVIC_Configuration();
    LPIT_Configuration();
    GPIO_Configuration();

    printf("\r\n\r\nLPIT_TimerBlinky.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);
    
    LPIT_HAL_StartChannel(LPIT0, DEMO_LPIT_CHANNEL_NUMBER);

    while (1)
    {
    }
}

static void NVIC_Configuration(void)
{
    NVIC_EnableIRQ(LPIT0_IRQn);
}

static void LPIT_Configuration(void)
{
    LPIT_ChannelConfig_T lpitChannelConfigStruct;

    /* Enable clock. */
    PCC->CLKCFG[PCC_LPIT0_INDEX] = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6U);

    /* LPIT. */
    LPIT_HAL_Enable(LPIT0, true);
    LPIT_HAL_Reset(LPIT0);
    LPIT_HAL_GetChannelConfigForNormalTimer(&lpitChannelConfigStruct);
    LPIT_HAL_ConfigChannel(LPIT0, DEMO_LPIT_CHANNEL_NUMBER, &lpitChannelConfigStruct);
    LPTI_HAL_SetChannelTimeoutValue(LPIT0, DEMO_LPIT_CHANNEL_NUMBER, 75000000U); /* BSP_CLK_SPLLDIV2_HZ = 75MHz */
    LPIT_HAL_EnableChannelInterrupt(LPIT0, DEMO_LPIT_CHANNEL_NUMBER, true);
}

static void GPIO_Configuration(void)
{
    GPIO_SetPinDir(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true);
    GPIO_SetPinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true);
}

void LPIT0_IRQHandler(void)
{
    if (LPIT_HAL_GetChannelTimeoutFlag(LPIT0, DEMO_LPIT_CHANNEL_NUMBER))
    {
        LPTI_HAL_ClearChannelTimeoutFlag(LPIT0, DEMO_LPIT_CHANNEL_NUMBER);
        GPIO_TogglePinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
}

/* EOF. */

