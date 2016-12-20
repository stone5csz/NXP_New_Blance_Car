/* hal_gpio.h */
/*--------------------------------------------------------------------------*
* Author: suyong_yq
* Email : suyong_yq@126.com
*--------------------------------------------------------------------------*/

#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

#include "hal_common.h"

/* Port operation. */
void GPIO_SetPortDir(GPIO_Type *devPtr, uint32_t pinMask, bool enDirOut);
void GPIO_SetPortLogic(GPIO_Type *devPtr, uint32_t pinMask, bool enLogicOne);
void GPIO_TogglePortLogic(GPIO_Type *devPtr, uint32_t pinMask);
uint32_t GPIO_GetPortLogic(GPIO_Type *devPtr, uint32_t pinMask);

/* Pin operation. */
void GPIO_SetPinDir(GPIO_Type *devPtr, uint32_t pinIdx, bool enDirOut);
void GPIO_SetPinLogic(GPIO_Type *devPtr, uint32_t pinIdx, bool enLogicOne);
void GPIO_TogglePinLogic(GPIO_Type *devPtr, uint32_t pinIdx);
bool GPIO_GetPinLogic(GPIO_Type *devPtr, uint32_t pinIdx);

#endif /* __FSL_GPIO_H__ */
