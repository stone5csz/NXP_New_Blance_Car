/* bsp_eidetect.c */
#include "app_inc.h"
#include "bsp_eidetect.h"
#include "fsl_device_registers.h"

/*! @brief Transform raw signed calibration result to unified type int32_t. */
#define ADC12_TRANSFORM_CALIBRATION_RESULT(resultValue, bitWidth) \
    ((resultValue >= (1 << (bitWidth - 1))) ? (resultValue - (1 << bitWidth)) : resultValue);

void EIDetect_InitDevice(void)
{
		int32_t	OFS, CLP9, CLPX;
		uint32_t CLPS, CLP0, CLP1, CLP2, CLP3, Typ1, Typ2, Typ3;
		uint32_t result, tmp32;
    /* Calibrate ADC0. */	
		//BSP_EID_LEFT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_LEFT_ADC_CHANNLE_NUM;
		BSP_EID_LEFT_ADC_MODULE_NUM->CFG1 = 0x00000008;		// 10bit resolution
		BSP_EID_LEFT_ADC_MODULE_NUM->CFG2 = 0x0000000D;		// sample clock 13
		BSP_EID_LEFT_ADC_MODULE_NUM->SC2 =	0x00000000;		// software trigger
		BSP_EID_LEFT_ADC_MODULE_NUM->SC3 =	0x00000087;		// Enable hardware average. 8 samples averaged.
		do {
			tmp32 = BSP_EID_LEFT_ADC_MODULE_NUM->SC1[0];	
		}	while((tmp32 & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK);	
		OFS = (int32_t)((BSP_EID_LEFT_ADC_MODULE_NUM->OFS & ADC_OFS_OFS_MASK) >> ADC_OFS_OFS_SHIFT);
    CLP9 = (int32_t)((BSP_EID_LEFT_ADC_MODULE_NUM->CLP9 & ADC_CLP9_CLP9_MASK) >> ADC_CLP9_CLP9_SHIFT);
    CLPX = (int32_t)((BSP_EID_LEFT_ADC_MODULE_NUM->CLPX & ADC_CLPX_CLPX_MASK) >> ADC_CLPX_CLPX_SHIFT);
    CLPS = ((BSP_EID_LEFT_ADC_MODULE_NUM->CLPS & ADC_CLPS_CLPS_MASK) >> ADC_CLPS_CLPS_SHIFT);
    CLP0 = ((BSP_EID_LEFT_ADC_MODULE_NUM->CLP0 & ADC_CLP0_CLP0_MASK) >> ADC_CLP0_CLP0_SHIFT);
    CLP1 = ((BSP_EID_LEFT_ADC_MODULE_NUM->CLP1 & ADC_CLP1_CLP1_MASK) >> ADC_CLP1_CLP1_SHIFT);
    CLP2 = ((BSP_EID_LEFT_ADC_MODULE_NUM->CLP2 & ADC_CLP2_CLP2_MASK) >> ADC_CLP2_CLP2_SHIFT);
    CLP3 = ((BSP_EID_LEFT_ADC_MODULE_NUM->CLP3 & ADC_CLP3_CLP3_MASK) >> ADC_CLP3_CLP3_SHIFT);
    Typ1 = (CLP0 + CLP0);
    Typ2 = (CLP1 + CLP1 - 26U);
    Typ3 = (CLP2 + CLP2);	
		
    /* Transform raw calibration result to unified type int32_t when the conversion result value is signed number. */
    OFS = ADC12_TRANSFORM_CALIBRATION_RESULT(OFS, 16);
    CLP9 = ADC12_TRANSFORM_CALIBRATION_RESULT(CLP9, 7);
    CLPX = ADC12_TRANSFORM_CALIBRATION_RESULT(CLPX, 7);		
		
    /* Check the calibration result value with its limit range. */
    if ((OFS < -48) || (OFS > 22) || (CLP9 < -12) || (CLP9 > 20) || (CLPX < -16) || (CLPX > 16) || (CLPS < 30U) ||
        (CLPS > 120U) || (CLP0 < (CLPS - 14U)) || (CLP0 > (CLPS + 14U)) || (CLP1 < (Typ1 - 16U)) ||
        (CLP1 > (Typ1 + 16U)) || (CLP2 < (Typ2 - 20U)) || (CLP2 > (Typ2 + 20U)) || (CLP3 < (Typ3 - 36U)) ||
        (CLP3 > (Typ3 + 36U)))
    {
        while(1);
    }
		
		/* read result for clearing coco */
		result = BSP_EID_LEFT_ADC_MODULE_NUM->R[0];
	
    /* Configure ADC0. */	
		//BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_RIGHT_ADC_CHANNLE_NUM;
		BSP_EID_LEFT_ADC_MODULE_NUM->CFG1 = 0x00000008;		// 10bit resolution
		BSP_EID_LEFT_ADC_MODULE_NUM->CFG2 = 0x0000000D;		// sample clock 13
		BSP_EID_LEFT_ADC_MODULE_NUM->SC2 =	0x00000000;		// software trigger
		BSP_EID_LEFT_ADC_MODULE_NUM->SC3 =	0x00000005;		// Enable hardware average. 8 samples averaged.
		
    /* Calibrate ADC2. */	
		//BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_RIGHT_ADC_CHANNLE_NUM;
		BSP_EID_RIGHT_ADC_MODULE_NUM->CFG1 = 0x00000008;		// 10bit resolution
		BSP_EID_RIGHT_ADC_MODULE_NUM->CFG2 = 0x0000000D;		// sample clock 13
		BSP_EID_RIGHT_ADC_MODULE_NUM->SC2 =	0x00000000;		// software trigger
		BSP_EID_RIGHT_ADC_MODULE_NUM->SC3 =	0x00000087;		// Enable hardware average. 8 samples averaged.
		do {
			tmp32 = BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0];	
		}	while((tmp32 & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK);	
		OFS = (int32_t)((BSP_EID_RIGHT_ADC_MODULE_NUM->OFS & ADC_OFS_OFS_MASK) >> ADC_OFS_OFS_SHIFT);
    CLP9 = (int32_t)((BSP_EID_RIGHT_ADC_MODULE_NUM->CLP9 & ADC_CLP9_CLP9_MASK) >> ADC_CLP9_CLP9_SHIFT);
    CLPX = (int32_t)((BSP_EID_RIGHT_ADC_MODULE_NUM->CLPX & ADC_CLPX_CLPX_MASK) >> ADC_CLPX_CLPX_SHIFT);
    CLPS = ((BSP_EID_RIGHT_ADC_MODULE_NUM->CLPS & ADC_CLPS_CLPS_MASK) >> ADC_CLPS_CLPS_SHIFT);
    CLP0 = ((BSP_EID_RIGHT_ADC_MODULE_NUM->CLP0 & ADC_CLP0_CLP0_MASK) >> ADC_CLP0_CLP0_SHIFT);
    CLP1 = ((BSP_EID_RIGHT_ADC_MODULE_NUM->CLP1 & ADC_CLP1_CLP1_MASK) >> ADC_CLP1_CLP1_SHIFT);
    CLP2 = ((BSP_EID_RIGHT_ADC_MODULE_NUM->CLP2 & ADC_CLP2_CLP2_MASK) >> ADC_CLP2_CLP2_SHIFT);
    CLP3 = ((BSP_EID_RIGHT_ADC_MODULE_NUM->CLP3 & ADC_CLP3_CLP3_MASK) >> ADC_CLP3_CLP3_SHIFT);
    Typ1 = (CLP0 + CLP0);
    Typ2 = (CLP1 + CLP1 - 26U);
    Typ3 = (CLP2 + CLP2);	
		
    /* Transform raw calibration result to unified type int32_t when the conversion result value is signed number. */
    OFS = ADC12_TRANSFORM_CALIBRATION_RESULT(OFS, 16);
    CLP9 = ADC12_TRANSFORM_CALIBRATION_RESULT(CLP9, 7);
    CLPX = ADC12_TRANSFORM_CALIBRATION_RESULT(CLPX, 7);		
		
    /* Check the calibration result value with its limit range. */
    if ((OFS < -48) || (OFS > 22) || (CLP9 < -12) || (CLP9 > 20) || (CLPX < -16) || (CLPX > 16) || (CLPS < 30U) ||
        (CLPS > 120U) || (CLP0 < (CLPS - 14U)) || (CLP0 > (CLPS + 14U)) || (CLP1 < (Typ1 - 16U)) ||
        (CLP1 > (Typ1 + 16U)) || (CLP2 < (Typ2 - 20U)) || (CLP2 > (Typ2 + 20U)) || (CLP3 < (Typ3 - 36U)) ||
        (CLP3 > (Typ3 + 36U)))
    {
        while(1);
    }

		/* read result for clearing coco */
		result = BSP_EID_RIGHT_ADC_MODULE_NUM->R[0];
		
    /* Configure ADC2. */	
		//BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_RIGHT_ADC_CHANNLE_NUM;
		BSP_EID_RIGHT_ADC_MODULE_NUM->CFG1 = 0x00000008;		// 10bit resolution
		BSP_EID_RIGHT_ADC_MODULE_NUM->CFG2 = 0x0000000D;		// sample clock 13
		BSP_EID_RIGHT_ADC_MODULE_NUM->SC2 =	0x00000000;		// software trigger
		BSP_EID_RIGHT_ADC_MODULE_NUM->SC3 =	0x00000005;		// Enable hardware average. 8 samples averaged.
}

void StartEIDetect(uint8_t eid_chnl)
{	
		if(eid_chnl == EID_CHANNEL_LEFT) {
			if((BSP_EID_LEFT_ADC_MODULE_NUM->SC1[0] & ADC_SC1_COCO_MASK) == 0U) {
				BSP_EID_LEFT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_LEFT_ADC_CHANNLE_NUM;
			}
		}

		if(eid_chnl == EID_CHANNEL_RIGHT) {
			if((BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0] & ADC_SC1_COCO_MASK) == 0U) {
				BSP_EID_RIGHT_ADC_MODULE_NUM->SC1[0] = ADC_SC1_AIEN_MASK | BSP_EID_RIGHT_ADC_CHANNLE_NUM;
			}
		}
}


/* EOF. */
