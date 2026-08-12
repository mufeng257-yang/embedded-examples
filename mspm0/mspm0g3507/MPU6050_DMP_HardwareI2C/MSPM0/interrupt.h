#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include "ti_msp_dl_config.h"
extern uint8_t enable_group1_irq;
extern bool mpu6050_flag;
void Interrupt_Init(void);

#endif  /* #ifndef _INTERRUPT_H_ */