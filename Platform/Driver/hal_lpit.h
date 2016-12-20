/* hal_lpit.h */
#ifndef __HAL_LPIT_H__
#define __HAL_LPIT_H__

#include "hal_common.h"

typedef enum
{
    eLPIT_TrgSel_Alt0 = 0U,
    eLPIT_TrgSel_Alt1 = 1U,
    eLPIT_TrgSel_Alt2 = 2U,
    eLPIT_TrgSel_Alt3 = 3U,
    eLPIT_TrgSel_Alt4 = 4U,
    eLPIT_TrgSel_Alt5 = 5U,
    eLPIT_TrgSel_Alt6 = 6U,
    eLPIT_TrgSel_Alt7 = 7U
} LPIT_TrgSel_T;

typedef enum
{
    eLPIT_TrgSrc_External = 0U,
    eLPIT_TrgSrc_Internal = 1U
} LPIT_TrgSrc_T;

typedef enum 
{
    eLPIT_WorkMode_32bitPeriodicCounter = 0U,
    /* In this mode the counter will load and then decrement down to zero. It will then set the
       timer interrupt flag and assert the output pre-trigger. */

    eLPIT_WorkMode_Dual16BitPeriodicCounter = 1U,
    /* In this mode, the counter will load and then the lower 16-bits will decrement down to zero,
       which will assert the output pre-trigger. The upper 16-bits will then decrement down to zero,
       which will negate the output pre-trigger and set the timer interrupt flag. */

    eLPIT_WorkMode_32BitTriggerAccumulator = 2U,
    /* In this mode, the counter will load on the first trigger rising edge and then decrement down
       to zero on each trigger rising edge. It will then set the timer interrupt flag and assert the
       output pre-trigger. */

    eLPIT_WorkMode_32BitTriggerInputCapture = 3U
    /* In this mode, the counter will load with 0xFFFF_FFFF and then decrement down to zero. If a
       trigger rising edge is detected, it will store the inverse of the current counter value in
       the load value register, set the timer interrupt flag and assert the output pre-trigger. */
} LPIT_WorkMode_T;

typedef struct
{
    LPIT_TrgSel_T TriggerSelect; /* Selects the trigger to use for starting and/or reloading the LPIT timer. */
    LPIT_TrgSrc_T TriggerSource; /* Selects between internal or external trigger sources. */
    bool enReloadTimerOnTrigger; /* Timer will reload on selected trigger. */
    bool enStopTimerOnTimeout; /* Timer will stop after timeout. For one-time counting. */
    bool enStartTimerOnTrigger; /* Timer starts to decrement when rising edge on selected trigger is detected.
                                   Defaultly, the timer would start immediately when it is enabled. */
    LPIT_WorkMode_T WorkMode;
    bool enChainMode; /* Timer decrements on previous channel's timeout. */
} LPIT_ChannelConfig_T;

void LPIT_HAL_Reset(LPIT_Type *base);
void LPIT_HAL_Enable(LPIT_Type *base, bool enable);

bool LPIT_HAL_GetChannelTimeoutFlag(LPIT_Type *base, uint32_t channel);
void LPTI_HAL_ClearChannelTimeoutFlag(LPIT_Type *base, uint32_t channel);

void LPIT_HAL_StartChannel(LPIT_Type *base, uint32_t channel);
void LPIT_HAL_StopChannel(LPIT_Type *base, uint32_t channel);

void LPTI_HAL_SetChannelTimeoutValue(LPIT_Type *base, uint32_t channel, uint32_t value);
uint32_t LPIT_HAL_GetChannelCurrentValue(LPIT_Type *base, uint32_t channel);

void LPIT_HAL_GetChannelConfigForNormalTimer(LPIT_ChannelConfig_T *configPtr);
void LPIT_HAL_ConfigChannel(LPIT_Type *base, uint32_t channel, LPIT_ChannelConfig_T *configPtr);
void LPIT_HAL_EnableChannelInterrupt(LPIT_Type *base, uint32_t channel, bool enable);

#endif /* __HAL_LPIT_H__ */

