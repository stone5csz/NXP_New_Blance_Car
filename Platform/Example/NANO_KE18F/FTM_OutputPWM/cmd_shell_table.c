/* cmd_shell_table.c */
#include "app_inc.h"

static int32_t do_show_help(int32_t argc, char *argv[]);
static int32_t do_PIN_TestChannel(int32_t argc, char *argv[]);
static int32_t do_FTM_ConfigPWM(int32_t argc, char *argv[]);
static int32_t do_FTM_OutputPWM(int32_t argc, char *argv[]);

CMD_TableItem_T DemoCmdTable[] =
{
    {"help",   1, do_show_help},
    {"pin_testchannel", 1, do_PIN_TestChannel},
    {"ftm_config_pwm", 2, do_FTM_ConfigPWM},
    {"ftm_output_pwm", 2, do_FTM_OutputPWM},
    {NULL}
};

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

static void MyDelay(void)
{
#if 0
    uint32_t i;

    for (i = 0U; i < 10000U; i++)
    {
        __ASM("NOP");
    }
#endif
    printf("Input char\r\n");
    getchar();
}

/* PTD15, FTM0_CH0(ALT2) */
static int32_t do_PIN_TestChannel(int32_t argc, char *argv[])
{
    PORTD->PCR[15] = PORT_PCR_MUX(1); /* Pinmux = GPIO. */
    GPIO_SetPinDir(GPIOD, 15U, true);
    GPIO_SetPinLogic(GPIOD, 15U, true);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);

    MyDelay();
    GPIO_TogglePinLogic(GPIOD, 15U);
        
    return 0;
}

/*
* argv[1]: 定时器计数周期
*/
/* PTD15, FTM0_CH0. */
#define DEMO_FTM_PWM_CHANNEL_NUMBER  0U
static int32_t do_FTM_ConfigPWM(int32_t argc, char *argv[])
{
    FTM_TimerConfig_T ftmTimerConfigStruct;

    /* Configure pin mux for FTM0_CH0. */
    PORTD->PCR[15] = PORT_PCR_MUX(2); /* FTM0_CH0. */

    /* Enable clock for FTM0. */
    PCC->CLKCFG[PCC_FLEXTMR0_INDEX] = PCC_CLKCFG_CGC_MASK;

    /* Configure the FTM module. */
    ftmTimerConfigStruct.SrcClkDivider = eFTM_SrcClkDiv_16;
    ftmTimerConfigStruct.ModuloValue = atol(argv[1]);
    ftmTimerConfigStruct.InitValue = 0U;
    ftmTimerConfigStruct.TimerCountingMode = eFTM_TimerCountingUp;
    FTM_ConfigTimer(FTM0, &ftmTimerConfigStruct);

    FTM_SetChannelMode(FTM0, DEMO_FTM_PWM_CHANNEL_NUMBER, eFTM_ChannelMode_PWM_HighTruePulse);
    FTM_EnableChannelOutput(FTM0, 1U<<DEMO_FTM_PWM_CHANNEL_NUMBER, true);
	
    printf("FTM_ConfigPWM Done.\r\n");

    return 0;
}

/*
* argv[1]: PWM信号反转事件点
*/
static int32_t do_FTM_OutputPWM(int32_t argc, char *argv[])
{
    uint32_t chnVal;

    FTM_StartTimer(FTM0, eFTM_ClkSrcDisabled); 

    chnVal = atol(argv[1]);
    FTM_SetChannelOutputValue(FTM0, DEMO_FTM_PWM_CHANNEL_NUMBER, chnVal);
	
    FTM_StartTimer(FTM0, eFTM_ClkSrcAlt1);  /* System clock. */

    printf("FTM_OutputPWM Done.\r\n");
    return 0;
}

/* EOF. */

