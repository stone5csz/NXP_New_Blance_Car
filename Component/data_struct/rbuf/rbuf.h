/*!
* @file    ring_buffer.h
* @brief   This component implements a ring buffer component based on a array of static memory.
* @version 1.0
* @author  Andrew SU (suyong_yq@126.com)
* @date    2015-09
*/

#ifndef __RBUF_H__
#define __RBUF_H__

#include <stdint.h>
#include <stdbool.h>

/*!
* @brief Define structure for Ring Buffer's handler.
*
* The Ring Buffer is origanized as FIFO based on a array of static memory.
*/
typedef struct
{
    uint32_t BufferSize; /*!< Keep the available count of byte in buffer's array */
    uint8_t *BufferPtr;  /*!< Keep the first pointer to the buffer's array. This pointer is used to
                              handler the memory. */
    uint32_t ReadIdx;    /*!< Keep the index of item in static array for FIFO's read pointer. */
    uint32_t WriteIdx;   /*!< Keep the index of item in static array for FIFO's write pointer. */
} RBUF_Handler_T;

/*!
* @brief Initialize the ring buffer.
*
* This function fills the allocated static memory into ring buffer's handle structure, and fills the
* read and write pointers with initial value.
*
* @param [out] handler Pointer to a empty ring buffer handle to be filled. See to #RBUF_Handler_T.
* @param [in]  bufPtr  Pointer to allocated static memory.
* @param [in]  size    Count of memory size in byte.
*/
void RBUF_Init(RBUF_Handler_T *handler, uint8_t *bufPtr, uint32_t size);

/*!
* @brief Check if the buffer is empty.
*
* @param [in] handler Pointer to an available handler to be operated.
* @retval true  The buffer is empty.
* @retval false The buffer is not empty.
*/
bool RBUF_IsEmpty(RBUF_Handler_T *handler);

/*!
* @brief Check if the buffer is full.
*
* @param [in] handler Pointer to an available handler to be operated.
* @retval true  The buffer is full.
* @retval false The buffer is not full.
*/
bool RBUF_IsFull(RBUF_Handler_T *handler);

/*!
* @brief Put data into buffer.
*
* @param [in] handler Pointer to an available handler to be operated.
* @param [in] dat The data to be put into buffer.
*/
void RBUF_PutDataIn(RBUF_Handler_T *handler, uint8_t dat);

/*!
* @brief Get out data from the buffer.
*
* @param [in] handler Pointer to an available handler to be operated.
* @retval The data from the buffer.
*/
uint8_t RBUF_GetDataOut(RBUF_Handler_T *handler);

#endif /* __RING_BUFFER_H__ */
