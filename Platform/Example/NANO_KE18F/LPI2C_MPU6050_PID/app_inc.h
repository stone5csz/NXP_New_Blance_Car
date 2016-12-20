/* app_inc.h */
#ifndef __APP_INC_H__
#define __APP_INC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bsp_config.h"
#include "hal_lpuart.h"
#include "hal_lpi2c.h"
#include "hal_lpit.h"

#define BSP_I2C_MPU6050_DEV_ADDR 0x68 /* AD0 = 0. */
#define DEMO_LPIT_CHANNEL_NUMBER 0U

typedef struct
{
    /* ????? */
    uint8_t AccX_H;
    uint8_t AccX_L;
    uint8_t AccY_H;
    uint8_t AccY_L;
    uint8_t AccZ_H;
    uint8_t AccZ_L;
    uint8_t TMP_H;  /* No used. */
    uint8_t TMP_L;  /* No used. */

    /* ??? */
    uint8_t GroX_H;
    uint8_t GroX_L;
    uint8_t GroY_H;
    uint8_t GroY_L;
    uint8_t GroZ_H;
    uint8_t GroZ_L;
    uint8_t NOTUSE_H; /* No used. */
    uint8_t NOTUSE_L; /* No used. */

    /* ??? */
    int16_t AccX, AccY, AccZ, GroX, GroY, GroZ;
} MPU6050_Info_T;

typedef struct
{
    double Angle;
    float GroY;
} Pos_Angle_Info_T;

#endif /* __APP_INC_H__ */

