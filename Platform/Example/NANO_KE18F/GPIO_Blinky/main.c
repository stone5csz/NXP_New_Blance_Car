/* main.c */
#include "app_inc.h"

void MyDelay(void)
{
    volatile uint32_t i;
    
    for (i = 0U; i < 2000000UL; i++)
    {
        __ASM("NOP");
    }
}

int main(void)
{
    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);
    BSP_InitGPIO_LED();

    GPIO_SetPinDir(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true);
    GPIO_SetPinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, true);
    
    GPIO_SetPinDir(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN, true);
    GPIO_SetPinLogic(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN, false);
    
    printf("\r\n\r\nGPIO_Blinky.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);
    
    while (1)
    {
        MyDelay();
        GPIO_TogglePinLogic(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
        GPIO_TogglePinLogic(BSP_GPIO_LED2_PORT, BSP_GPIO_LED2_PIN);
    }
}

/* EOF. */

