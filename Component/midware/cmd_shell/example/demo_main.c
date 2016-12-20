/****************************************************************************
* FileName: demo_main.c
* Author  : suyong_yq@126.com
* Data    : 2015.9
****************************************************************************/

#include "demo_inc.h"

static int32_t do_show_help(int32_t argc, char *argv[]);
static int32_t do_show_logo(int32_t argc, char *argv[]);
static int32_t do_show_param(int32_t argc, char *argv[]);
static int32_t do_show_param2(int32_t argc, char *argv[]);

CMD_TableItem_T DemoCmdTable[] =
{
    {"help",   1, do_show_help},
    {"logo",   1, do_show_logo},
    {"param",  4, do_show_param},
    {"param2", 2, do_show_param2},
    {NULL}
};

int main(void)
{
    init_platform();

    printf("\r\n*----------------------------------------\r\n");
    printf("* Welcome to cmd_shell demo project.\r\n");
    printf("* Compiled at %s on %s\r\n", __TIME__, __DATE__);
    printf("*----------------------------------------*\r\n\r\n");
    
    CMD_InstallHandler(&gCmdHandlerCallbackStruct);

    while (1)
    {
        CMD_LoopShell(DemoCmdTable);
    }
}

/*
 * This cmd will display all the registed commands in the list.
 */
static int32_t do_show_help(int32_t argc, char *argv[])
{
    uint32_t idx = 0U;
    printf("# do_show_help()\r\n\r\n");
    while (DemoCmdTable[idx].CmdName != NULL)
    {
        printf(" - %s\r\n", DemoCmdTable[idx].CmdName);
        idx++;
    }
    printf("\r\n");
    return 0;
}

static int32_t do_show_logo(int32_t argc, char *argv[])
{
    printf("# do_print_logo()\r\n\r\n");
    printf("*----------------------------------------*\r\n");
    printf("Hello, This is mini command shell.\r\n");
    printf("*----------------------------------------\r\n");
    printf("\r\n");
    return 0;
}

/*
 * This cmd shows how to use the parameter when executing
 * the function.
 */
static int32_t do_show_param(int32_t argc, char *argv[])
{
    int32_t i;
    printf("# do_show_param()\r\n");
    for (i = 0; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }
    printf("\r\n");
    return 0;
}

/*
 * This cmd shows how to convert the parameter to number
 * by using standard function in stdlib.h
 */
static int32_t do_show_param2(int32_t argc, char *argv[])
{
    printf("# do_show_param2()\r\n");
    printf("argv[0]: %s\r\n", argv[0]);
    printf("argv[1] number value: %d\r\n\r\n", atoi(argv[1]));
    /*
     - atof(string to float)
     - atoi(string to integer)
     - atol(string to long)
     - strtod(string to float)
     - strtol(string to long)
     - strtoul(string to unsigned long)
     - toascii(char to ASCII code)
     - toupper(char to capital case)
     - tolower(char to low case)
    */
    return 0;
}

/* EOF. */
