/* main.c */
#include "app_inc.h"

extern CMD_TableItem_T DemoCmdTable[];

void CmdPutChar(char c)
{
    /* Put data to bus. */
    while (LPUART_HAL_FLAG_TX_EMPTY_MASK != (LPUART_HAL_FLAG_TX_EMPTY_MASK & LPUART_HAL_GetStatus(BSP_LPUART_DEBUG_PORT)))
    {}
    LPUART_HAL_SetTxData(BSP_LPUART_DEBUG_PORT, c);
}

char CmdGetChar(void)
{
    char ch;

    /* Fetch data from bus. */
    while (LPUART_HAL_FLAG_RX_FULL_MASK != (LPUART_HAL_FLAG_RX_FULL_MASK & LPUART_HAL_GetStatus(BSP_LPUART_DEBUG_PORT)))
    {}
    ch = LPUART_HAL_GetRxData(BSP_LPUART_DEBUG_PORT);
    return ch;
}

const CMD_HandlerCallback_T gCmdHandlerStruct =
{
    .SER_PutCharFunc = CmdPutChar,
    .SER_GetCharFunc = CmdGetChar,
    ">"
};

int main(void)
{
    BSP_InitSystem();
    BSP_InitStdioLPUART(115200U);

    CMD_InstallHandler(&gCmdHandlerStruct);
    
    printf("\r\n\r\nUART_CmdShell Example.\r\n");
    printf("Compiled on %s, %s\r\n\r\n", __DATE__, __TIME__);
    
    while (1)
    {
        CMD_LoopShell(DemoCmdTable);
    }
}

/* EOF. */

