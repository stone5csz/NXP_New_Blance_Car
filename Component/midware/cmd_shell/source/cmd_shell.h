/****************************************************************************
* FileName: cmd_shell.h
* Author  : suyong_yq@126.com
* Data    : 2015.9
****************************************************************************/

#ifndef __CMD_SHELL_H__
#define __CMD_SHELL_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	void (*SER_PutCharFunc)(char txData);
	char (*SER_GetCharFunc)(void);
	char *PromptStr;
} CMD_HandlerCallback_T;

typedef struct
{
	char    *CmdName; 
	int32_t ArgsMaxCount;
	int32_t (*CmdFunc)(int32_t argc, char *argv[]);
} CMD_TableItem_T;

bool CMD_InstallHandler(const CMD_HandlerCallback_T *callbackPtr);
uint32_t CMD_LoopShell(CMD_TableItem_T *cmdTblPtr);

#endif /* __CMD_HANDLER_H__ */
