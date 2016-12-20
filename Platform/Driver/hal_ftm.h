/* HAL_ftm.h */
#ifndef __HAL_FTM_H__
#define __HAL_FTM_H__

#include "hal_common.h"


/* Interrupts. */
#define FTM_INTERRUPT_TIMER_OVERFLOW_MASK (1 << 0)

/* Flags. */
/* 清FTM_SC[TOF]标志位时，需要先读SC寄存器，即先要读一下FTM_SC[TOF]标志位，才能实现清除标志位的操作 */
#define FTM_FLAG_TIMER_OVERFLOW_MASK (1 << 0)

typedef enum
{
    eFTM_SrcClkDiv_1 = 0U,
    eFTM_SrcClkDiv_2 = 1U,
    eFTM_SrcClkDiv_4 = 2U,
    eFTM_SrcClkDiv_8 = 3U,
    eFTM_SrcClkDiv_16 = 4U,
    eFTM_SrcClkDiv_32 = 5U,
    eFTM_SrcClkDiv_64 = 6U,
    eFTM_SrcClkDiv_128 = 7U,
} FTM_SrcClkDiv_T;

typedef enum
{
    eFTM_ClkSrcDisabled = 0U,
    eFTM_ClkSrcAlt1 = 1U,
    eFTM_ClkSrcAlt2 = 2U,
    eFTM_ClkSrcAlt3 = 3U
} FTM_ClkSrcMode_T;

typedef enum
{
    eFTM_TimerCountingUp, /* 从零计到最大值，然后切回零 */
    eFTM_TimerCountingUpDown /* 从零计到最大值，然后从最大值计到最小值 */
} FTM_TimerCountingMode_T;

typedef struct
{
    /* For basic counter frequence. */
    FTM_SrcClkDiv_T SrcClkDivider; /* 对计数时钟进行分频的值 */
    uint32_t ModuloValue; /* 计数模值，即计数最大值 */
    uint32_t InitValue; /* 计数初值 */
    /* For work mode. */
    FTM_TimerCountingMode_T TimerCountingMode;
} FTM_TimerConfig_T;

typedef enum
{
	/* PWM. */
    eFTM_ChannelMode_PWM_HighTruePulse, /* Output high at first, clear output on match. */
    eFTM_ChannelMode_PWM_LowTruePulse,  /* Output low at first, set output on match. */
    /* Input capture. */
    eFTM_ChannelMode_InputCapture_OnRisingEdgeOnly,
    eFTM_ChannelMode_InputCapture_OnFallingEdgeOnly,
    eFTM_ChannelMode_InputCapture_OnEitherBothEdge,
    /* Output compare. */
    eFTM_ChannelMode_OutputCompare_ToggleOutputOnMatch,
    eFTM_ChannelMode_OutputCompare_ClearOutputOnMatch,
    eFTM_ChannelMode_OutputCompare_SetOutputOnMatch,
} FTM_ChannelMode_T;

typedef enum
{
    eFTM_QuadEncodingPhAPhB, /* Phase A and phase B encoding mode. */
    eFTM_QuadEncodingCntDir  /* Count and direction encoding mode. */
} FTM_QuadEncodingMode_T;

typedef enum
{
    eFTM_QuadDecoderOverflowOnCountingDown,
    eFTM_QuadDecoderOverflowOnCountingUp
} FTM_QuadDecoderOverflowEvent_T;

typedef struct
{
    bool enPhAInputFilter;
    bool enPhBInputFilter;
    bool enPhAInvertPol;
    bool enPhBInvertPol;
    FTM_QuadEncodingMode_T QuadEncodingMode;
    bool enInvertDir; /* Counting direction is decreasing or increasing. */
    FTM_QuadDecoderOverflowEvent_T QuadDecoderOverflowEvent; /* Indicates if the TOF bit was set on the top or the bottom of counting. */

} FTM_QuadDecoderConfig_T;

/* For timer. */
void FTM_ConfigTimer(FTM_Type *base, FTM_TimerConfig_T *configPtr);
void FTM_StartTimer(FTM_Type *base, FTM_ClkSrcMode_T mode);
void FTM_EnableTimerInterrupt(FTM_Type *base);
void FTM_DisableTimerInterrupt(FTM_Type *base);
bool FTM_GetTimerFlag(FTM_Type *base);
void FTM_ClearTimerFlag(FTM_Type *base);

/* For QuadDecoder. */
void FTM_ConfigQuadDecoderMode(FTM_Type *base, FTM_QuadDecoderConfig_T *configPtr);
uint32_t FTM_GetQuadDecoderValue(FTM_Type *base);
void FTM_ClearQuadDecoderValue(FTM_Type *base);

/* For channels. */
void FTM_EnableChannelInterrupt(FTM_Type *base, uint32_t channel);
void FTM_DisableChannelInterrupt(FTM_Type *base, uint32_t channel);
void FTM_EnableChannelDMA(FTM_Type *base, uint32_t channel);
void FTM_DisableChannelDMA(FTM_Type *base, uint32_t channel);
bool FTM_GetChannelFlag(FTM_Type *base, uint32_t channel);
void FTM_ClearChannelFlag(FTM_Type *base, uint32_t channel);
void FTM_SetChannelMode(FTM_Type *base, uint32_t channel, FTM_ChannelMode_T mode);
void FTM_EnableChannelOutput(FTM_Type *base, uint32_t channelMask, bool enable);
void FTM_SetChannelOutputValue(FTM_Type *base, uint32_t channel, uint32_t value);
uint32_t FTM_GetChannelInputValue(FTM_Type *base, uint32_t channel);

#endif /* __HAL_FTM_H__ */
