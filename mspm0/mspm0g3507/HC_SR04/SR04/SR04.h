#ifndef __BSP_SR04_H__
#define __BSP_SR04_H__

#include "ti_msp_dl_config.h"

#define SR04_TRIG(x)  ( x ? DL_GPIO_setPins(SR04_PORT,SR04_Trig_PIN) : DL_GPIO_clearPins(SR04_PORT,SR04_Trig_PIN) )
#define SR04_ECHO()   ( ( ( DL_GPIO_readPins(SR04_PORT,SR04_Echo_PIN) & SR04_Echo_PIN ) > 0 ) ? 1 : 0 )
#define delay_ms(x)		delay_cycles(32000*x)
#define delay_1us(x)		delay_cycles(32*x)
void SR04_Init(void);
float SR04_GetLength(void );

#endif