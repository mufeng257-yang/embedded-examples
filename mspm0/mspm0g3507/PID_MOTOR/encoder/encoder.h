#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "ti_msp_dl_config.h"

extern int volatile coutA;
extern int volatile coutB;
extern int volatile raw_coutA;
extern int volatile raw_coutB;
extern int encoder_flag;
void encoder_init(void);
void encoder_update(void);
void TIMER_ENCODER_init(void);
#endif