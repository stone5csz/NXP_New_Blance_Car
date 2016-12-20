/* hal_ftm.c */
#include "hal_ftm.h"

static void FTM_UnLockConfigCmd(FTM_Type *base)
{
    base->MODE |= FTM_MODE_WPDIS_MASK;
}

static void FTM_EnableAdvancedFeature(FTM_Type *base)
{
    base->MODE |= FTM_MODE_FTMEN_MASK;
}

/*
static void FTM_LockConfigCmd(FTM_Type *base)
{
    base->MODE &= ~FTM_MODE_WPDIS_MASK;
}
*/

void FTM_ConfigTimer(FTM_Type *base, FTM_TimerConfig_T *configPtr)
{
    uint32_t tmp32;

    FTM_EnableAdvancedFeature(base);
    FTM_UnLockConfigCmd(base);

    /* FTM_SC. */
    tmp32 = base->SC & ~(FTM_SC_CPWMS_MASK | FTM_SC_PS_MASK);
    if (eFTM_TimerCountingUpDown ==  configPtr->TimerCountingMode)
    {
        tmp32 |= FTM_SC_CPWMS_MASK;
    }
    tmp32 |= FTM_SC_PS(configPtr->SrcClkDivider);
    base->SC = tmp32;

    /* FTM_MODE. */
    base->MOD = FTM_MOD_MOD(configPtr->ModuloValue);

    /* FTM_CNTIN. */
    base->CNTIN = FTM_CNTIN_INIT(configPtr->InitValue);
}

void FTM_StartTimer(FTM_Type *base, FTM_ClkSrcMode_T mode)
{
    base->SC = (base->SC & ~FTM_SC_CLKS_MASK) | FTM_SC_CLKS(mode);
}

void FTM_EnableTimerInterrupt(FTM_Type *base)
{
    base->SC |= FTM_SC_TOIE_MASK;
}

void FTM_DisableTimerInterrupt(FTM_Type *base)
{
    base->SC &= ~FTM_SC_TOIE_MASK;
}

bool FTM_GetTimerFlag(FTM_Type *base)
{
    return 0U != (FTM_SC_TOF_MASK & base->SC);
}

void FTM_ClearTimerFlag(FTM_Type *base)
{
    base->SC &= ~FTM_SC_TOF_MASK; /* Write 0 to clear. */
}

void FTM_ConfigQuadDecoderMode(FTM_Type *base, FTM_QuadDecoderConfig_T *configPtr)
{
    uint32_t tmp32 = FTM_QDCTRL_QUADEN_MASK;

    if (configPtr == NULL)
    {
        base->QDCTRL = 0U;
        return;
    }
    if (configPtr->enPhAInputFilter)
    {
        tmp32 |= FTM_QDCTRL_PHAFLTREN_MASK;
    }
    if (configPtr->enPhBInputFilter)
    {
        tmp32 |= FTM_QDCTRL_PHBFLTREN_MASK;
    }
    if (configPtr->enPhAInvertPol)
    {
        tmp32 |= FTM_QDCTRL_PHAPOL_MASK;
    }
    if (configPtr->enPhBInvertPol)
    {
        tmp32 |= FTM_QDCTRL_PHBPOL_MASK;
    }
    if (eFTM_QuadEncodingCntDir == configPtr->QuadEncodingMode)
    {
        tmp32 |= FTM_QDCTRL_QUADMODE_MASK;
    }
    if (false == configPtr->enInvertDir)
    {
        tmp32 |= FTM_QDCTRL_QUADIR_MASK;
    }
    if (eFTM_QuadDecoderOverflowOnCountingUp == configPtr->QuadDecoderOverflowEvent)
    {
        tmp32 |= FTM_QDCTRL_TOFDIR_MASK;
    }
    base->QDCTRL = tmp32;
}

uint32_t FTM_GetQuadDecoderValue(FTM_Type *base)
{
    return base->CNT;
}

void FTM_ClearQuadDecoderValue(FTM_Type *base)
{
    base->CNT = 0U;
}

/* For channels. */
void FTM_EnableChannelInterrupt(FTM_Type *base, uint32_t channel)
{
    base->CONTROLS[channel].CnSC |= FTM_CnSC_CHIE_MASK;
}
void FTM_DisableChannelInterrupt(FTM_Type *base, uint32_t channel)
{
    base->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHIE_MASK;
}

void FTM_EnableChannelDMA(FTM_Type *base, uint32_t channel)
{
    base->CONTROLS[channel].CnSC |= FTM_CnSC_DMA_MASK;
}
void FTM_DisableChannelDMA(FTM_Type *base, uint32_t channel)
{
    base->CONTROLS[channel].CnSC &= ~FTM_CnSC_DMA_MASK;
}

bool FTM_GetChannelFlag(FTM_Type *base, uint32_t channel)
{
    return 0U != (FTM_CnSC_CHF_MASK & base->CONTROLS[channel].CnSC);
}
void FTM_ClearChannelFlag(FTM_Type *base, uint32_t channel)
{
    base->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

void FTM_SetChannelMode(FTM_Type *base, uint32_t channel, FTM_ChannelMode_T mode)
{
    switch(mode)
    {
        /* PWM. */
        case eFTM_ChannelMode_PWM_HighTruePulse:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~FTM_CnSC_ELSA_MASK
                                         | FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
            break;
        case eFTM_ChannelMode_PWM_LowTruePulse:
            base->CONTROLS[channel].CnSC |= FTM_CnSC_ELSA_MASK | FTM_CnSC_MSB_MASK;
            break;
        /* Input capture. */
        case eFTM_ChannelMode_InputCapture_OnRisingEdgeOnly:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK)
                                         | FTM_CnSC_ELSA_MASK;
            break;
        case eFTM_ChannelMode_InputCapture_OnFallingEdgeOnly:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK)
                                         | FTM_CnSC_ELSB_MASK;
            break;
        case eFTM_ChannelMode_InputCapture_OnEitherBothEdge:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSA_MASK | FTM_CnSC_MSB_MASK)
                                         | FTM_CnSC_ELSA_MASK| FTM_CnSC_ELSB_MASK;
            break;
        /* Output compare. */
        case eFTM_ChannelMode_OutputCompare_ToggleOutputOnMatch:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK)
                                         | FTM_CnSC_MSA_MASK | FTM_CnSC_ELSA_MASK;
            break;
        case eFTM_ChannelMode_OutputCompare_ClearOutputOnMatch:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK)
                                         | FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK;
            break;
        case eFTM_ChannelMode_OutputCompare_SetOutputOnMatch:
            base->CONTROLS[channel].CnSC = base->CONTROLS[channel].CnSC & ~FTM_CnSC_MSA_MASK
                                         | FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK;
            break;
        default:
            break;
    }
}

void FTM_EnableChannelOutput(FTM_Type *base, uint32_t channelMask, bool enable)
{
    if (enable)
    {
        base->SC |= ((channelMask & 0xFF) << FTM_SC_PWMEN0_SHIFT);
    }
    else
    {
        base->SC &= ~((channelMask & 0xFF) << FTM_SC_PWMEN0_SHIFT);
    }
}

void FTM_SetChannelOutputValue(FTM_Type *base, uint32_t channel, uint32_t value)
{
    base->CONTROLS[channel].CnV = value;
}
uint32_t FTM_GetChannelInputValue(FTM_Type *base, uint32_t channel)
{
    return base->CONTROLS[channel].CnV;
}

/* EOF. */
