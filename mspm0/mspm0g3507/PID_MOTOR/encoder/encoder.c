#include "encoder.h"
int volatile coutA=0;
int volatile coutB=0;
int volatile raw_coutA=0;
int volatile raw_coutB=0;
//≥ı ºªØ
void encoder_init(void)
{
	NVIC_ClearPendingIRQ(GPIOB_INT_IRQn);
	NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void encoder_update(void)
{
	coutA = raw_coutA;
	coutB = raw_coutB;
	raw_coutA= 0;
	raw_coutB= 0;
}
void TIMER_ENCODER_init(void)
{

	NVIC_ClearPendingIRQ(TIMER_ENCODER_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_ENCODER_INST_INT_IRQN);
}
int encoder_flag=0;
