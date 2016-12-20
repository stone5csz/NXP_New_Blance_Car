/* rbuf.c */
#include "rbuf.h"

void RBUF_Init(RBUF_Handler_T *handler, uint8_t *bufPtr, uint32_t size)
{
    handler->BufferPtr = bufPtr;
    handler->BufferSize = size;
    handler->ReadIdx = 0U;
    handler->WriteIdx = 0U;
}

static uint32_t _RBUF_NextIdx(uint32_t idx, uint32_t maxCnt)
{
    return ( ((idx+1U) == maxCnt) ? 0U : (idx+1U) );
}

bool RBUF_IsEmpty(RBUF_Handler_T *handler)
{
    return (handler->ReadIdx == handler->WriteIdx);
}

bool RBUF_IsFull(RBUF_Handler_T *handler)
{
    return (  _RBUF_NextIdx(handler->WriteIdx, handler->BufferSize) == handler->ReadIdx);
}

void RBUF_PutDataIn(RBUF_Handler_T *handler, uint8_t dat)
{
    *((handler->BufferPtr)+(handler->WriteIdx)) = dat;
    handler->WriteIdx = _RBUF_NextIdx(handler->WriteIdx, handler->BufferSize);
}

uint8_t RBUF_GetDataOut(RBUF_Handler_T *handler)
{
    uint8_t dat = *((handler->BufferPtr)+(handler->ReadIdx));
    handler->ReadIdx = _RBUF_NextIdx(handler->ReadIdx, handler->BufferSize);
    return dat;
}

/* EOF. */

