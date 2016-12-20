/* bsp_motor.h */
#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "bsp_motor_config.h"


#define MOTOR_PWM_COUNTER_MAX 			7500U

typedef enum
{
    eMotor_DirectionForward = 0U,
    eMotor_DirectionBackward = 1U,
    eMotor_DirectionStop = 2U,
} Motor_Direction_T;

void Motor_InitDevice(void);
void Motor_EnableTurn(bool enable);

void Motor_SetTurnDirection(uint32_t motorIdMask, Motor_Direction_T dir);
void Motor_SetTrunSpeed(uint32_t motorIdMask, uint32_t counterValue);

uint32_t Motor_GetQuadDecoderValue(uint32_t motorId);
void Motor_ClearQuadDecoderValue(uint32_t motorIdMask);


#endif /* __BSP_MOTOR_H__ */
