/****************************************************************************
* FileName: demo_inc.h
* Author  : suyong_yq@126.com
* Data    : 2015.9
****************************************************************************/

#ifndef __DEMO_INC_H__
#define __DEMO_INC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_shell.h"

/*---------------------------------------------------------------------------
 * Adapter would implement these APIs.
 *-------------------------------------------------------------------------*/
extern const CMD_HandlerCallback_T gCmdHandlerCallbackStruct;
bool init_platform(void);
void exit_platform(void);

#endif /* __DEMO_INC_H__ */
