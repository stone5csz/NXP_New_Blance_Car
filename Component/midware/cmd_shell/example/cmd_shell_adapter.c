#include "cmd_shell.h"

void CmdPutChar(char c)
{
    /* Put data to bus. */
}

char CmdGetChar(void)
{
    char ch;

    /* Fetch data from bus. */

    return ch;
}

const CMD_HandlerCallback_T gCmdHandlerStruct =
{
    .SER_PutCharFunc = CmdPutChar,
    .SER_GetCharFunc = CmdGetChar,
    ">"
};
