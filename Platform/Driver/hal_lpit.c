/* hal_lpit.c */
#include "hal_lpit.h"

void LPIT_HAL_Reset(LPIT_Type *base)
{
	base->MCR |= LPIT_MCR_SW_RST_MASK;
	base->MCR &= ~LPIT_MCR_SW_RST_MASK;
}

void LPIT_HAL_Enable(LPIT_Type *base, bool enable)
{
	if (enable)
	{
		base->MCR = LPIT_MCR_DBG_EN_MASK
		          | LPIT_MCR_DOZE_EN_MASK
		          | LPIT_MCR_M_CEN_MASK;
	}
	else
	{
		base->MCR = 0U;
	}
	
}

bool LPIT_HAL_GetChannelTimeoutFlag(LPIT_Type *base, uint32_t channel)
{
    return (0U != ((1U << channel) & base->MSR));
}
void LPTI_HAL_ClearChannelTimeoutFlag(LPIT_Type *base, uint32_t channel)
{
    base->MSR = (1U << channel);
}

void LPIT_HAL_StartChannel(LPIT_Type *base, uint32_t channel)
{
    base->SETTEN = (1U << channel); /* SET_T_EN_n */
}
void LPIT_HAL_StopChannel(LPIT_Type *base, uint32_t channel)
{
    base->CLRTEN = (1U << channel);
}

void LPTI_HAL_SetChannelTimeoutValue(LPIT_Type *base, uint32_t channel, uint32_t value)
{
	base->CHANNEL[channel].TVAL = value;
}

uint32_t LPIT_HAL_GetChannelCurrentValue(LPIT_Type *base, uint32_t channel)
{
	return base->CHANNEL[channel].CVAL;
}

void LPIT_HAL_GetChannelConfigForNormalTimer(LPIT_ChannelConfig_T *configPtr)
{
	configPtr->TriggerSelect = eLPIT_TrgSel_Alt0;
    configPtr->TriggerSource = eLPIT_TrgSrc_External;
    configPtr->enReloadTimerOnTrigger = false;
    configPtr->enStopTimerOnTimeout = false;
    configPtr->enStartTimerOnTrigger = false;
    configPtr->WorkMode = eLPIT_WorkMode_32bitPeriodicCounter;
    configPtr->enChainMode = false;
}

void LPIT_HAL_ConfigChannel(LPIT_Type *base, uint32_t channel, LPIT_ChannelConfig_T *configPtr)
{
	uint32_t tmp32 = 0U;

	tmp32 |= LPIT_TCTRL_TRG_SEL(configPtr->TriggerSelect);
	if (eLPIT_TrgSrc_Internal == configPtr->TriggerSource)
	{
		tmp32 |= LPIT_TCTRL_TRG_SRC_MASK;
	}
	if (configPtr->enReloadTimerOnTrigger)
	{
		tmp32 |= LPIT_TCTRL_TROT_MASK;
	}
	if (configPtr->enStopTimerOnTimeout)
	{
		tmp32 |= LPIT_TCTRL_TSOI_MASK;
	}
	if (configPtr->enStartTimerOnTrigger)
	{
		tmp32 |= LPIT_TCTRL_TSOT_MASK;
	}
	tmp32 |= LPIT_TCTRL_MODE(configPtr->WorkMode);
	if (configPtr->enChainMode)
	{
		tmp32 |= LPIT_TCTRL_CHAIN_MASK;
	}
	base->CHANNEL[channel].TCTRL = tmp32;
}

void LPIT_HAL_EnableChannelInterrupt(LPIT_Type *base, uint32_t channel, bool enable)
{
	if (enable)
	{
		base->MIER |= (1U << channel);
	}
	else
	{
		base->MIER &= ~(1U << channel);
	}
}

/* EOF. */
