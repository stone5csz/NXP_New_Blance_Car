/* bsp_eidetect_config.h */
#ifndef __BSP_EIDETECT_CONFIG_H__
#define __BSP_EIDETECT_CONFIG_H__

#include "fsl_device_registers.h"

/* EI detect port & pin */
/* ◊Û≤‡µÁ¥≈ºÏ≤‚ADC0[CH0] */
#define BSP_GPIO_EID_LEFT_PORT         GPIOA
#define BSP_GPIO_EID_LEFT_PIN          0U
#define BSP_EID_LEFT_ADC_MODULE_NUM    ADC0
#define BSP_EID_LEFT_ADC_CHANNLE_NUM   0U

/* ”“≤‡µÁ¥≈ºÏ≤‚ADC2[CH14] */
#define BSP_GPIO_EID_RIGHT_PORT         GPIOC
#define BSP_GPIO_EID_RIGHT_PIN          8U
#define BSP_EID_RIGHT_ADC_MODULE_NUM    ADC2
#define BSP_EID_RIGHT_ADC_CHANNLE_NUM   14U

/* EID CHANNEL ID Mask */
#define EID_CHANNEL_LEFT    (1U << 0U)
#define EID_CHANNEL_RIGHT   (1U << 1U)

void BSP_InitEIDetect(void);

#endif /* __BSP_EIDETECT_CONFIG_H__ */

