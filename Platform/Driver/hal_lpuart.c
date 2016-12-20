/* hal_lpuart.c */
#include "hal_lpuart.h"

/* Reset all the hardware registers. */
void LPUART_HAL_Reset(LPUART_Type *base)
{
    base->BAUD = 0x0F000004;
    base->STAT = 0xC01FC000; /* Clear all the w1c flags. */
    base->CTRL = 0x0;
    base->MATCH = 0x0;
}

void LPUART_HAL_SetBaudrate(LPUART_Type *base, uint32_t busClkHz, uint32_t baudrate)
{
    uint32_t temp, oldCtrl;
    uint16_t sbr, sbrTemp;
    uint32_t osr, osrTemp, tempDiff, calculatedBaud, baudDiff;

    /* Store CTRL before disable Tx and Rx */
    oldCtrl = base->CTRL;

    /* Disable LPUART TX RX before setting. */
    base->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

    /* This LPUART instantiation uses a slightly different baud rate calculation
     * The idea is to use the best OSR (over-sampling rate) possible
     * Note, OSR is typically hard-set to 16 in other LPUART instantiations
     * loop to find the best OSR value possible, one that generates minimum baudDiff
     * iterate through the rest of the supported values of OSR */

    baudDiff = baudrate;
    osr = 0U;
    sbr = 0U;
    for (osrTemp = 4U; osrTemp <= 32U; osrTemp++)
    {
        /* calculate the temporary sbr value   */
        sbrTemp = (busClkHz / (baudrate * osrTemp));
        /*set sbrTemp to 1 if the sourceClockInHz can not satisfy the desired baud rate*/
        if (sbrTemp == 0U)
        {
            sbrTemp = 1U;
        }
        /* Calculate the baud rate based on the temporary OSR and SBR values */
        calculatedBaud = (busClkHz / (osrTemp * sbrTemp));

        tempDiff = calculatedBaud - baudrate;

        /* Select the better value between srb and (sbr + 1) */
        if (tempDiff > (baudrate - (busClkHz / (osrTemp * (sbrTemp + 1U)))))
        {
            tempDiff = baudrate - (busClkHz / (osrTemp * (sbrTemp + 1U)));
            sbrTemp++;
        }

        if (tempDiff <= baudDiff)
        {
            baudDiff = tempDiff;
            osr = osrTemp; /* update and store the best OSR value calculated */
            sbr = sbrTemp; /* update store the best SBR value calculated */
        }
    }

    /* Check to see if actual baud rate is within 3% of desired baud rate
     * based on the best calculate OSR value */
    if (baudDiff < ((baudrate / 100U) * 3U))
    {
        temp = base->BAUD;

        /* Acceptable baud rate, check if OSR is between 4x and 7x oversampling.
         * If so, then "BOTHEDGE" sampling must be turned on */
        if ((osr > 3U) && (osr < 8U))
        {
            temp |= LPUART_BAUD_BOTHEDGE_MASK;
        }

        /* program the osr value (bit value is one less than actual value) */
        temp &= ~LPUART_BAUD_OSR_MASK;
        temp |= LPUART_BAUD_OSR(osr - 1U);

        /* write the sbr value to the BAUD registers */
        temp &= ~LPUART_BAUD_SBR_MASK;
        base->BAUD = temp | LPUART_BAUD_SBR(sbr);
    }

    /* Restore CTRL. */
    base->CTRL = oldCtrl;
}

void LPUART_HAL_EnableInterrupts(LPUART_Type *base, uint32_t intMask, bool enable)
{
    if (enable)
    {
    	base->CTRL |= intMask;
    }
    else
    {
    	base->CTRL &= ~intMask;
    }
}

void LPUART_HAL_EnableTx(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= LPUART_CTRL_TE_MASK;
    }
    else
    {
        base->CTRL &= ~LPUART_CTRL_TE_MASK;
    }
}

void LPUART_HAL_EnableRx(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= LPUART_CTRL_RE_MASK;
    }
    else
    {
        base->CTRL &= ~LPUART_CTRL_RE_MASK;
    }
}

bool LPUART_HAL_GetTxInterruptEnabled(LPUART_Type *base)
{
    return (LPUART_CTRL_TE_MASK == (LPUART_CTRL_TE_MASK & base->CTRL));
}

uint32_t LPUART_HAL_GetStatus(LPUART_Type *base)
{
    return base->STAT;
}

void LPUART_HAL_SetTxData(LPUART_Type *base, uint8_t dat)
{
    base->DATA = dat;
}

uint8_t LPUART_HAL_GetRxData(LPUART_Type *base)
{
    return (uint8_t)(0xFF & base->DATA);
}

/* EOF. */
