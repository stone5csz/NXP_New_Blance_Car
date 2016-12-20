/* bsp_EIDETECT.h */
#ifndef __BSP_EIDETECT_H__
#define __BSP_EIDETECT_H__

#include <stdbool.h>
#include <stdint.h>

#include "bsp_eidetect_config.h"


void EIDetect_InitDevice(void);
void StartEIDetect(uint8_t eid_chnl);



#endif /* __BSP_EIDETECT_H__ */

