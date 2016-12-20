/* hal_gpio.c */
#include "hal_gpio.h"

/*
 * Port operation.
 */
void GPIO_SetPortDir(GPIO_Type *devPtr, uint32_t pinMask, bool enDirOut)
{
    if (enDirOut)
    {
        devPtr->PDDR |= pinMask;
    }
    else
    {
        devPtr->PDDR &= ~pinMask;
    }
}

void GPIO_SetPortLogic(GPIO_Type *devPtr, uint32_t pinMask, bool enLogicOne)
{
    if (enLogicOne)
    {
        devPtr->PSOR = pinMask;
    }
    else
    {
        devPtr->PCOR = pinMask;
    }
}

void GPIO_TogglePortLogic(GPIO_Type *devPtr, uint32_t pinMask)
{
    devPtr->PTOR = pinMask;
}

uint32_t GPIO_GetPortLogic(GPIO_Type *devPtr, uint32_t pinMask)
{
    return (pinMask & devPtr->PDIR);
}

/*
 * Pin operation.
 */
void GPIO_SetPinDir(GPIO_Type *devPtr, uint32_t pinIdx, bool enDirOut)
{
    GPIO_SetPortDir(devPtr, 1U<<pinIdx, enDirOut);
}

void GPIO_SetPinLogic(GPIO_Type *devPtr, uint32_t pinIdx, bool enLogicOne)
{
    GPIO_SetPortLogic(devPtr, 1<<pinIdx, enLogicOne);
}

void GPIO_TogglePinLogic(GPIO_Type *devPtr, uint32_t pinIdx)
{
    GPIO_TogglePortLogic(devPtr, 1U<<pinIdx);
}

bool GPIO_GetPinLogic(GPIO_Type *devPtr, uint32_t pinIdx)
{
    return (0U != GPIO_GetPortLogic(devPtr, 1U<<pinIdx));
}

/* EOF. */
