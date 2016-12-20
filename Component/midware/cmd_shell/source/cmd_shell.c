/****************************************************************************
* FileName: cmd_shell.c
* Author  : suyong_yq@126.com
* Data    : 2015.9
****************************************************************************/

#include "cmd_shell.h"
#include <string.h>

#ifndef DEBUG_PARSER
#define DEBUG_PARSER    (0U)
#endif /* DEBUG_PARSER */

#if DEBUG_PARSER
#include <stdio.h>
#endif /* DEBUG_PARSER */

#ifndef NULL
#define NULL    (void *)(0U)
#endif

#define MAX_DELAY_STOP_STR  32
#define CB_SIZE         64  /* max size for console buffer. */
#define MAX_ARGS        4

#define CMD_ARRAY_SIZE(cmdTbl)   (sizeof(cmdTbl)/sizeof(cmdTbl[0]))

const CMD_HandlerCallback_T * gCmdHandlerCallbackPtr;
static char gConsoleBuffer[CB_SIZE];        /* console I/O buffer */
static char cEraseSeq[] = "\b \b";    /* erase sequence */
static char cTabSeq[] = "        ";       /* used to expand TABs */

/*---------------------------------------------------------------------------
 * Interface functions.
 *-------------------------------------------------------------------------*/
static void CMD_PutChar(char ch);
static void CMD_PutString(char *str);
static uint8_t CMD_GetChar(void);

/*---------------------------------------------------------------------------
 * Internal functions.
 *-------------------------------------------------------------------------*/
static int32_t CMD_ReadLine(char *prompt);
static char *CMD_DeleteChar(char *buffer, char *p, int32_t *colp, int32_t *np, int32_t plen);
static int CMD_ParseLine(char *line, char *argv[]);
static CMD_TableItem_T *CMD_FindCmd(const char *cmd, CMD_TableItem_T *table);
//static CMD_TableItem_T *CMD_FindCmd(const char *cmd, CMD_TableItem_T *table, int32_t table_len);
static int32_t CMD_ExecuteCmd(char *cmd, int32_t flag, CMD_TableItem_T cmdTbl[]);

bool CMD_InstallHandler(const CMD_HandlerCallback_T *callbackPtr)
{
    if (!callbackPtr)
    {
        return false;
    }
    gCmdHandlerCallbackPtr = callbackPtr;
    return true;
}

uint32_t CMD_LoopShell(CMD_TableItem_T cmdTbl[])
{
    static char lastcommand[CB_SIZE] = { 0 };
    int32_t len;
    int32_t rc = 1;
    int32_t flag;

    /* Main Loop for Monitor Command Processing */
    for (;;)
    {
        len = CMD_ReadLine(gCmdHandlerCallbackPtr->PromptStr);

        flag = 0;    /* assume no special flags for now */

        if (len == -1)
        {
            CMD_PutString("<INTERRUPT>\r\n");
        }
        else
        {
            if (len > 0)
            {
                strcpy(lastcommand, gConsoleBuffer);
            }
            rc = CMD_ExecuteCmd(lastcommand, flag, cmdTbl);
        }

        if (rc <= 0)
        {
            /* invalid command or not repeatable, forget it */
            lastcommand[0] = 0;
        }
    }
}

/*---------------------------------------------------------------------------
 * Interface functions.
 *-------------------------------------------------------------------------*/
static void CMD_PutChar(char ch)
{
    (*(gCmdHandlerCallbackPtr->SER_PutCharFunc))((uint8_t)ch);
}
static void CMD_PutString(char *str)
{
    while (*str)
    {
        CMD_PutChar(*str++);
    }
}
static uint8_t CMD_GetChar(void)
{
    return (*(gCmdHandlerCallbackPtr->SER_GetCharFunc))();
}

/*---------------------------------------------------------------------------
 * Internal functions.
 *-------------------------------------------------------------------------*/

/* This function is to deal with the input stream and echo back to terminal. */
static int32_t CMD_ReadLine(char *prompt)
{
    char *p = gConsoleBuffer;
    char *p_buf = p;
    int32_t   n = 0;              /* buffer index */
    int32_t   plen = 0;           /* prompt length */
    int32_t   col;                /* output column cnt */
    char  c;
    int32_t ret = 0;

    /* print prompt */
    if (prompt)
    {
        plen = strlen((char const *)prompt);
        CMD_PutString(prompt);
    }
    col = plen;

    for (;;)
    {
        int32_t is_continue = 0;
        /* remove the white space before strings. */
        c = 0;
        while (1)
        {
            c = CMD_GetChar();
            if ((0xff != (uint8_t)c) && (0x0 != (uint8_t)c))
            {
                break;
            }
        }

        /* Special character handling */
        switch (c)
        {
        case '\r':                /* Enter */
        case '\n':
            *p = '\0';
            CMD_PutString("\r\n");
            ret = p - p_buf;
            break;

        case '\0':                /* nul */
            is_continue = 1;
            break;

        case 0x03:                /* ^C - break */
            p_buf[0] = '\0';      /* discard input */
            ret = 1;
            break;

        case 0x15:                /* ^U - erase line */
            while (col > plen)
            {
                CMD_PutString(cEraseSeq);
                --col;
            }
            p = p_buf;
            n = 0;
            is_continue = 1;
            break;

        case 0x17:                /* ^W - erase word */
            p = CMD_DeleteChar(p_buf, p, &col, &n, plen);
            while ((n > 0) && (*p != ' '))
            {
                p = CMD_DeleteChar(p_buf, p, &col, &n, plen);
            }
            is_continue = 1;
            break;

        case 0x08:                /* ^H  - backspace */
        case 0x7F:                /* DEL - backspace */
            p = CMD_DeleteChar(p_buf, p, &col, &n, plen);
            is_continue = 1;
            break;

        default:
            /* Must be a normal character then */
            if (n < CB_SIZE - 2)
            {
                if (c == '\t')
                {
                    /* expand TABs */
                    CMD_PutString(cTabSeq + ((uint32_t)col & 0x7));
                    col += 8 - ((uint32_t)col & 0x7);
                } else {
                    ++col;        /* echo input */
                    CMD_PutChar(c);
                }
                *p++ = c;
                ++n;
            } else {              /* Buffer full */
                CMD_PutChar('\a');
            }
            is_continue = 1;
            break;
        }
        if (is_continue)
        {
            continue;
        }
        else
        {
            return ret;
        }
    }
}

static char *CMD_DeleteChar(char *buffer, char *p, int32_t *colp, int32_t *np, int32_t plen)
{
    char *s;

    if (*np == 0)
    {
        return (p);
    }

    if (*(--p) == '\t')
    {
        /* will retype the whole line */
        while (*colp > plen)
        {
            CMD_PutString(cEraseSeq);
            (*colp)--;
        }
        for (s = buffer; s < p; ++s)
        {
            if (*s == '\t')
        {
                CMD_PutString(cTabSeq+(((uint32_t)(*colp)) & 0x7));
                *colp += 8 - (((uint32_t)(*colp)) & 0x7);
            } else {
                ++(*colp);
                CMD_PutChar(*s);
            }
        }
    } else {
        CMD_PutString(cEraseSeq);
        (*colp)--;
    }
    (*np)--;

    return (p);
}

/* This function is to divide a line of string into pieces. */
static int CMD_ParseLine(char *line, char *argv[])
{
    int32_t nargs = 0;

#if DEBUG_PARSER
    printf("parse_line: \"%s\"\r\n", line);
#endif
    while (nargs < MAX_ARGS)
    {

        /* skip any white space */
        while ((*line == ' ') || (*line == '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {
            /* end of line, no more args */
            argv[nargs] = NULL;
#if DEBUG_PARSER
            printf("parse_line: nargs=%d\r\n", nargs);
#endif
            return (nargs);
        }

        argv[nargs++] = line;    /* begin of argument string */

        /* find end of string */
        while ((*line) && (*line != ' ') && (*line != '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {
            /* end of line, no more args */
            argv[nargs] = NULL;
#if DEBUG_PARSER
            printf("parse_line: nargs=%d\r\n", nargs);
#endif
            return (nargs);
        }

        *line = '\0';        /* terminate current arg */
        line++;
    }

    //printf("** Too many args (max. %d) **\r\n", MAX_ARGS);
    CMD_PutString("** Too many args. **\r\n");
#if DEBUG_PARSER
    printf("parse_line: nargs=%d\r\n", nargs);
#endif
    return (nargs);
}

static CMD_TableItem_T *CMD_FindCmd(const char *cmd, CMD_TableItem_T *table)
//static CMD_TableItem_T *CMD_FindCmd(const char *cmd, CMD_TableItem_T *table, int32_t table_len)
{
    CMD_TableItem_T *cmdtp;
    CMD_TableItem_T *cmdtp_temp = table;    /* Init value */
    const char *p;
    int32_t len;
    int32_t n_found = 0;

    len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);
    for (cmdtp = table;
        //cmdtp != table + table_len;
	      cmdtp->CmdFunc != NULL;
        cmdtp++)
    {
        if (strncmp((char const *)cmd, (char const *)cmdtp->CmdName, strlen((char const *)cmd)) == 0)
        {
            if (len == strlen((char const *)cmdtp->CmdName))
            {
                return cmdtp;    /* full match */
            }

            cmdtp_temp = cmdtp;    /* abbreviated command ? */
            n_found++;
        }
    }
    if (n_found == 1)            /* exactly one match */
    {
        return cmdtp_temp;
    }

    return NULL;    /* not found or ambiguous command */
}

static int32_t CMD_ExecuteCmd(char *cmd, int32_t flag, CMD_TableItem_T cmdTbl[])
{
    CMD_TableItem_T *cmdtp;
    char *argv[MAX_ARGS + 1];    /* NULL terminated */
    int32_t argc;

#if DEBUG_PARSER
    printf("[RUN_COMMAND] cmd[%p]=\"", cmd);
    printf(cmd ? cmd : "NULL");    /* use puts - string may be loooong */
    printf("\"\r\n");
#endif

    if ((!cmd) || (!*cmd))
    {
        return 1;    /* empty command */
    }

    if (strlen(cmd) >= CB_SIZE)
    {
        CMD_PutString("## Command too long!\r\n");
        return 1;
    }

    /*
     * Process separators and check for invalid
     * repeatable commands
     */

#if DEBUG_PARSER
    printf("[PROCESS_SEPARATORS] %s\r\n", cmd);
#endif

    /* Extract arguments */
    argc = CMD_ParseLine(cmd, argv);
    if (argc == 0)
    {
        return 1;
    }

    /* Look up command in command table */
		//if ((cmdtp = CMD_FindCmd(argv[0], cmdTbl, (CMD_ARRAY_SIZE(cmdTbl)))) == NULL)
    if ((cmdtp = CMD_FindCmd(argv[0], cmdTbl)) == NULL)
    {
        //printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
			  CMD_PutString("Unknown command '");
			  CMD_PutString(argv[0]);
			  CMD_PutString("' - try 'help'\r\n");
        return 1;
    }

    /* found - check max args */
    if (argc > cmdtp->ArgsMaxCount)
    {
        return 1;
    }

    /* OK - call function to do the command */
    if ((cmdtp->CmdFunc)(argc, argv) != 0)
    {
        return 1;
    }

    return 0;
}

/* EOF. */
