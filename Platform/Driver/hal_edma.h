/* hal_edma.h */

#ifndef __HAL_EDMA_H__
#define __HAL_EDMA_H__

#include "hal_common.h"

/*
* Enumeration.
*/
typedef enum
{
    eEDMA_BusWidth_1Byte = 0U, /* 8-bit. */
    eEDMA_BusWidth_2Byte = 1U, /* 16-bit. */
    eEDMA_BusWidth_4Byte = 2U  /* 32-bit. */
} EDMA_BusWidth_T;

typedef enum
{
    eEDMA_AddrCycle_Disabled    = 0U,
    eEDMA_AddrCycleModulo_16B   = 1U,
    eEDMA_AddrCycleModulo_32B   = 2U,
    eEDMA_AddrCycleModulo_64B   = 3U,
    eEDMA_AddrCycleModulo_128B  = 4U,
    eEDMA_AddrCycleModulo_256B  = 5U,
    eEDMA_AddrCycleModulo_512B  = 6U,
    eEDMA_AddrCycleModulo_1KB   = 7U,
    eEDMA_AddrCycleModulo_2KB   = 8U,
    eEDMA_AddrCycleModulo_4KB   = 9U,
    eEDMA_AddrCycleModulo_8KB   = 10U,
    eEDMA_AddrCycleModulo_16KB  = 11U,
    eEDMA_AddrCycleModulo_32KB  = 12U,
    eEDMA_AddrCycleModulo_64KB  = 13U,
    eEDMA_AddrCycleModulo_128KB = 14U,
    eEDMA_AddrCycleModulo_256KB = 15U
} EDMA_AddrCycleMode_T;


/*
* Structure.
*/

typedef struct
{
    /* Basic settings. */
    uint32_t SrcAddr;
    uint32_t DestAddr;

    /* 一个Minor Loop连续搬运的字节数。 */
    uint32_t MinorLoopByteCount; /* Continuous bytes send by minor loop. */
    /* Bit width of each transfer. */
    /* 传输位宽主要是跟访问寄存器的总线宽度相关。对于有的寄存器只允许8-bit访问，
       此时，EDMA对该寄存器的写入或者读出就必须进行相应的配置，否则就会出现硬件
       上的BusFault。
    */
    EDMA_BusWidth_T SrcBusWidthMode;
    EDMA_BusWidth_T DestBusWidthMode;

    /* Address cycle. */
    /* 地址循环周期是楼下各种Offset的一种特定模式的用法。当地址递增到指定循环长
       度的时候，下一个地址会周转到开始的位置，从头再来。地址就在一个小周期里递
       增后递减的循环变化。地址循环的功能是可以关闭不用的。
    */
    EDMA_AddrCycleMode_T SrcAddrCycleMode;
    EDMA_AddrCycleMode_T DestAddrCycleMode;

    /* Minor loop. */
    /* Minor Loop是EDMA传输的最小不可拆分单元。每当检测到DMAMUX来的触发信号，
       EDMA就启动一次Minor Loop的传输。在AddrInc和Offset为0的情况下，每个Minor
       Loop传输结束后，下一次传输仍从上一次传输开始的位置重新传数。
       AddrInc的值表示每次总线传输之后的地址递增字节数，一个连续的Minor Loop可能
       会被分成多次总线传输，一次总线传输的字节数同总线位宽BusWidth的配置有关，
       一般将这个值同总线位宽对应的字节数对应起来，就可以让传输数据的地址连续。
       Offset的值表示一整个连续的Minor Loop传输完成之后，将传输地址进行偏移，为
       下一次传输重新定位开始地址。
       注意，AddrInc和Offset都是以字节为单位的，并且可以为负数。
    */
    /* Offset for next transfer's address of source and destination. */
    int32_t SrcAddrIncPerTransfer; /* Increasment for next transfer. */
    int32_t DestAddrIncPerTransfer;
    int32_t SrcAddrMinorLoopDoneOffset; /* Offset when minor loop is done. */
    int32_t DestAddrMinorLoopDoneOffset;

    /* Major loop. */
    /* 一个Major Loop可以包含若干个Minor Loop。引入Major Loop的意义在于，Major  
       Loop可以在若干个Minor Loop之后再跟一个特别的Offset，进行一次长距离的地址
       地址跳转。
       注意，Offset是以字节为单位的，并且可以为负数。
    */
    uint32_t MinorLoopCount; /* Count of minor loop in a major loop. */
    /* Address Offset on totally transfer done. */
    int32_t SrcAddrMajorLoopDoneOffset; 
    int32_t DestAddrMajorLoopDoneOffset;

} EDMA_TransferConfig_T;

/*
* API.
*/

/* DMAMUX. */
void DMAMUX_HAL_SetTriggerForDMA(uint32_t chnIdx, uint32_t trigger);

/* DMA Engine. */
void EDMA_HAL_ResetEngine(void);
void EDMA_HAL_EnableMinorLoopOffset(bool enable);

/* DMA TCD (Transfer-control Descriptor). */
void EDMA_HAL_ResetChannel(uint32_t chnIdx);
bool EDMA_HAL_ConfigTransfer(uint32_t chnIdx, EDMA_TransferConfig_T *configPtr);

bool EDMA_HAL_IsMajorLoopDone(uint32_t chnIdx);
void EDMA_HAL_ClearMajorLoopDoneFlag(uint32_t chnIdx);
void EDMA_HAL_ClearInterruptFlagOfMajorLoopDone(uint32_t chnIdx);
void EDMA_HAL_EnableInterruptOnMajorLoopDone(uint32_t chnIdx, bool enable);
void EDMA_HAL_EnableInterruptOnHalfMajorLoopDone(uint32_t chnIdx, bool enable);

/* Trigger. */
void EDMA_HAL_DoSoftTriggerCmd(uint32_t chnIdx);
void EDMA_HAL_EnableHwTrigger(uint32_t chnIdx, bool enable);
void EDMA_HAL_SetHwTriggerAutoDisabledOnMajorLoopDone(uint32_t chnIdx, bool enable);

#endif /* __HAL_EDMA_H__ */


