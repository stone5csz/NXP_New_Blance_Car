/* bsp_motor_config.h */
#ifndef __BSP_MOTOR_CONFIG_H__
#define __BSP_MOTOR_CONFIG_H__

#include "fsl_device_registers.h"

/* 控制是否切断电机驱动器内部电源 */
#define BSP_GPIO_MOTOR_STANDBY_PORT         GPIOD
#define BSP_GPIO_MOTOR_STANDBY_PIN          5U

#define BSP_FTM_MOTOR_PWM_PORT              FTM0

/*
 * MOTOR A.
 */
/* 控制方向 */
#define BSP_GPIO_MOTOR_A_IN1_PORT           GPIOE
#define BSP_GPIO_MOTOR_A_IN1_PIN            10U
#define BSP_GPIO_MOTOR_A_IN2_PORT           GPIOE
#define BSP_GPIO_MOTOR_A_IN2_PIN            11U
/* 控制转速 */
#define BSP_FTM_MOTOR_A_PWM_CHANNEL_NUM     1U
/* 编码器接口 */
#define BSP_FTM_MOTOR_A_QUAD_DECODER_PORT   FTM2

/*
 * MOTOR B.
 */
/* 控制方向 */
#define BSP_GPIO_MOTOR_B_IN1_PORT           GPIOB
#define BSP_GPIO_MOTOR_B_IN1_PIN            4U
#define BSP_GPIO_MOTOR_B_IN2_PORT           GPIOB
#define BSP_GPIO_MOTOR_B_IN2_PIN            5U
/* 控制转速 */
#define BSP_FTM_MOTOR_B_PWM_CHANNEL_NUM     0U
/* 编码器接口 */
#define BSP_FTM_MOTOR_B_QUAD_DECODER_PORT   FTM1

/*
 * Motor ID Mask.
 */
#define MOTOR_ID_A   (1U << 0U)
#define MOTOR_ID_B   (1U << 1U)

void BSP_InitMotorTurn(void);
void BSP_InitMotorQuadDecoder(void);

#endif /* __BSP_MOTOR_CONFIG_H__ */
