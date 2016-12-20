/* bsp_motor.c */
#include "bsp_motor.h"
#include "hal_ftm.h"
#include "hal_gpio.h"

void Motor_InitDevice(void)
{
    FTM_TimerConfig_T ftmTimerConfigStruct;
    FTM_QuadDecoderConfig_T ftmQuadDecoderConfigStruct;

    /* 初始化GPIO */
    /* Motor stand-by pin. */
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_STANDBY_PORT, BSP_GPIO_MOTOR_STANDBY_PIN, false); /* 0. */
    GPIO_SetPinDir(BSP_GPIO_MOTOR_STANDBY_PORT, BSP_GPIO_MOTOR_STANDBY_PIN, true); /* Output. */

    /* Motor A IN1 pin. */
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN1_PORT, BSP_GPIO_MOTOR_A_IN1_PIN, false); /* 0 */
    GPIO_SetPinDir(BSP_GPIO_MOTOR_A_IN1_PORT, BSP_GPIO_MOTOR_A_IN1_PIN, true); /* Output. */

    /* Motor A IN2 pin. */
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN2_PORT, BSP_GPIO_MOTOR_A_IN2_PIN, false); /* 0 */
    GPIO_SetPinDir(BSP_GPIO_MOTOR_A_IN2_PORT, BSP_GPIO_MOTOR_A_IN2_PIN, true); /* Output. */

    /* Motor B IN1 pin. */
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN1_PORT, BSP_GPIO_MOTOR_B_IN1_PIN, false); /* 0 */
    GPIO_SetPinDir(BSP_GPIO_MOTOR_B_IN1_PORT, BSP_GPIO_MOTOR_B_IN1_PIN, true); /* Output. */

    /* Motor B IN2 pin. */
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN2_PORT, BSP_GPIO_MOTOR_B_IN2_PIN, false); /* 0 */
    GPIO_SetPinDir(BSP_GPIO_MOTOR_B_IN2_PORT, BSP_GPIO_MOTOR_B_IN2_PIN, true); /* Output. */

    /* 初始化FTM用于产生的PWM部分 */
    /* 时钟源频率为150MHz，计数频率约为75MHz，调制频率约为10kHz */
    //ftmTimerConfigStruct.SrcClkDivider = eFTM_SrcClkDiv_8;
    ftmTimerConfigStruct.SrcClkDivider = eFTM_SrcClkDiv_2;
    ftmTimerConfigStruct.ModuloValue = MOTOR_PWM_COUNTER_MAX;
    ftmTimerConfigStruct.InitValue = 0U;
    ftmTimerConfigStruct.TimerCountingMode = eFTM_TimerCountingUp;
    FTM_ConfigTimer(BSP_FTM_MOTOR_PWM_PORT, &ftmTimerConfigStruct);

    FTM_SetChannelMode(BSP_FTM_MOTOR_PWM_PORT, BSP_FTM_MOTOR_A_PWM_CHANNEL_NUM, eFTM_ChannelMode_PWM_HighTruePulse);
    FTM_SetChannelMode(BSP_FTM_MOTOR_PWM_PORT, BSP_FTM_MOTOR_B_PWM_CHANNEL_NUM, eFTM_ChannelMode_PWM_HighTruePulse);
    FTM_EnableChannelOutput( BSP_FTM_MOTOR_PWM_PORT,
                             (1U<<BSP_FTM_MOTOR_A_PWM_CHANNEL_NUM) | (1U<<BSP_FTM_MOTOR_B_PWM_CHANNEL_NUM),
                             true);
    /* 初始化编码器 */
    ftmTimerConfigStruct.SrcClkDivider = eFTM_SrcClkDiv_2;
    ftmTimerConfigStruct.ModuloValue = MOTOR_PWM_COUNTER_MAX;
    ftmTimerConfigStruct.InitValue =  0U;
    ftmTimerConfigStruct.TimerCountingMode = eFTM_TimerCountingUp;
    FTM_ConfigTimer(BSP_FTM_MOTOR_A_QUAD_DECODER_PORT, &ftmTimerConfigStruct);
    FTM_ConfigTimer(BSP_FTM_MOTOR_B_QUAD_DECODER_PORT, &ftmTimerConfigStruct);

    ftmQuadDecoderConfigStruct.enPhAInputFilter = false;
    ftmQuadDecoderConfigStruct.enPhBInputFilter = false;
    ftmQuadDecoderConfigStruct.enPhAInvertPol = false;
    ftmQuadDecoderConfigStruct.enPhBInvertPol = false;
    ftmQuadDecoderConfigStruct.QuadEncodingMode = eFTM_QuadEncodingPhAPhB;
    ftmQuadDecoderConfigStruct.enInvertDir = true; /* Increase. */
    ftmQuadDecoderConfigStruct.QuadDecoderOverflowEvent = eFTM_QuadDecoderOverflowOnCountingUp;
    FTM_ConfigQuadDecoderMode(BSP_FTM_MOTOR_A_QUAD_DECODER_PORT, &ftmQuadDecoderConfigStruct);
    FTM_ConfigQuadDecoderMode(BSP_FTM_MOTOR_B_QUAD_DECODER_PORT, &ftmQuadDecoderConfigStruct);

    FTM_StartTimer(BSP_FTM_MOTOR_A_QUAD_DECODER_PORT, eFTM_ClkSrcAlt1);  /* System clock. */
    FTM_StartTimer(BSP_FTM_MOTOR_B_QUAD_DECODER_PORT, eFTM_ClkSrcAlt1);  /* System clock. */

}

void Motor_EnableTurn(bool enable)
{
    GPIO_SetPinLogic(BSP_GPIO_MOTOR_STANDBY_PORT, BSP_GPIO_MOTOR_STANDBY_PIN, enable);
}

void Motor_SetTurnDirection(uint32_t motorIdMask, Motor_Direction_T dir)
{
    if (0U != (MOTOR_ID_A & motorIdMask))
    {
        switch (dir)
        {
        case eMotor_DirectionForward:
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN1_PORT, BSP_GPIO_MOTOR_A_IN1_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN2_PORT, BSP_GPIO_MOTOR_A_IN2_PIN, true); /* 1 */
            break;
        case eMotor_DirectionBackward:
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN2_PORT, BSP_GPIO_MOTOR_A_IN2_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN1_PORT, BSP_GPIO_MOTOR_A_IN1_PIN, true); /* 1 */
            break;
        default: /* eMotor_DirectionStop */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN1_PORT, BSP_GPIO_MOTOR_A_IN1_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_A_IN2_PORT, BSP_GPIO_MOTOR_A_IN2_PIN, false); /* 0 */
            break;
        }
    }
    if (0U != (MOTOR_ID_B & motorIdMask))
    {
        switch (dir)
        {
        case eMotor_DirectionForward:
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN1_PORT, BSP_GPIO_MOTOR_B_IN1_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN2_PORT, BSP_GPIO_MOTOR_B_IN2_PIN, true); /* 1 */
            break;
        case eMotor_DirectionBackward:
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN2_PORT, BSP_GPIO_MOTOR_B_IN2_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN1_PORT, BSP_GPIO_MOTOR_B_IN1_PIN, true); /* 1 */
            break;
        default: /* eMotor_DirectionStop */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN1_PORT, BSP_GPIO_MOTOR_B_IN1_PIN, false); /* 0 */
            GPIO_SetPinLogic(BSP_GPIO_MOTOR_B_IN2_PORT, BSP_GPIO_MOTOR_B_IN2_PIN, false); /* 0 */
            break;
        }
    }
}
void Motor_SetTrunSpeed(uint32_t motorIdMask, uint32_t counterValue)
{
    FTM_StartTimer(BSP_FTM_MOTOR_PWM_PORT, eFTM_ClkSrcDisabled);

    if (0U != (MOTOR_ID_A & motorIdMask)) /* For motor A. */
    {
        FTM_SetChannelOutputValue(BSP_FTM_MOTOR_PWM_PORT, BSP_FTM_MOTOR_A_PWM_CHANNEL_NUM, counterValue);
    }
    if (0U != (MOTOR_ID_B & motorIdMask)) /* For motor B. */
    {
        FTM_SetChannelOutputValue(BSP_FTM_MOTOR_PWM_PORT, BSP_FTM_MOTOR_B_PWM_CHANNEL_NUM, counterValue);
    }

    FTM_StartTimer(BSP_FTM_MOTOR_PWM_PORT, eFTM_ClkSrcAlt1);  /* System clock. */
}

/* 每次只能接收一个ID */
uint32_t Motor_GetQuadDecoderValue(uint32_t motorId)
{
    uint32_t tmp32;

    if (MOTOR_ID_A == motorId)
    {
        tmp32 = FTM_GetQuadDecoderValue(BSP_FTM_MOTOR_A_QUAD_DECODER_PORT);
    }
    else if (MOTOR_ID_B == motorId)
    {
        tmp32 = FTM_GetQuadDecoderValue(BSP_FTM_MOTOR_B_QUAD_DECODER_PORT);
    }
    
    return tmp32;
}
/* 可以接收多个ID同时传入 */
void Motor_ClearQuadDecoderValue(uint32_t motorIdMask)
{
    if (0U != (MOTOR_ID_A & motorIdMask))
    {
        FTM_ClearQuadDecoderValue(BSP_FTM_MOTOR_A_QUAD_DECODER_PORT);
    }

    if (0U != (MOTOR_ID_B & motorIdMask))
    {
        FTM_ClearQuadDecoderValue(BSP_FTM_MOTOR_B_QUAD_DECODER_PORT);
    }
}

/* EOF. */
