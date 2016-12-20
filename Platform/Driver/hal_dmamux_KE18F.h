/* hal_dmamux_KE18F.h */

#ifndef __HAL_DMAMUX_H__
#define __HAL_DMAMUX_H__

typedef enum
{
    eDMAMUX_TriggerSource_Disabled   = 0U,
    eDMAMUX_TriggerSource_LPUART0_Rx = 2U,
    eDMAMUX_TriggerSource_LPUART0_Tx = 3U,
    eDMAMUX_TriggerSource_LPUART1_Rx = 4U,
    eDMAMUX_TriggerSource_LPUART1_Tx = 5U,
    eDMAMUX_TriggerSource_LPUART2_Rx = 6U,
    eDMAMUX_TriggerSource_LPUART2_Tx = 7U,
    eDMAMUX_TriggerSource_LPI2C0_Rx = 18U,
    eDMAMUX_TriggerSource_LPI2C0_Tx = 19U,

    eDMAMUX_TriggerSource_ADC0  = 40U,
    eDMAMUX_TriggerSource_ADC1  = 41U,
    eDMAMUX_TriggerSource_ADC2  = 42U,
    eDMAMUX_TriggerSource_DMAMUX_AlwaysEnable0 = 60U,
    eDMAMUX_TriggerSource_DMAMUX_AlwaysEnable1 = 61U,
    eDMAMUX_TriggerSource_DMAMUX_AlwaysEnable2 = 62U,
    eDMAMUX_TriggerSource_DMAMUX_AlwaysEnable3 = 63U,
    eDMAMUX_TriggerSource_PIT = 64U,
} DMAMUX_TriggerSource_T;

#endif /* __HAL_DMAMUX_H__ */

