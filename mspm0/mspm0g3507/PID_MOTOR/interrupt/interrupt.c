#include "ti_msp_dl_config.h"
#include "encoder.h"
#include "interrupt.h"
#include "pid.h"
#include "motor.h"
#include "serial.h"

uint8_t enable_group1_irq = 0;
extern int A;
extern int B;
extern PID_TypeDef Aspeedhuan;
extern PID_TypeDef Bspeedhuan;
int A_PWM=0;
int B_PWM=0;

void TIMER_ENCODER_INST_IRQHandler(){

if( DL_TimerA_getPendingInterrupt(TIMER_ENCODER_INST) == DL_TIMER_IIDX_ZERO )
	{
		encoder_update();
		float Aspeed_out=PID_Compute(&Aspeedhuan,(float)A,(float)coutA);
		float Bspeed_out=PID_Compute(&Bspeedhuan,(float)B,(float)coutB);
		A_PWM+=Aspeed_out;
		B_PWM+=Bspeed_out;
		if(A_PWM>1000) A_PWM=1000;
		if(A_PWM<-1000) A_PWM=-1000;
		if(B_PWM>1000) B_PWM=1000;
		if(B_PWM<-1000) B_PWM=-1000;
		A_speed(A_PWM);
		B_speed(B_PWM);
	}
}

void GROUP1_IRQHandler(void)
{
	uint32_t gpio_status;
	gpio_status = DL_GPIO_getEnabledInterruptStatus(ENCODER_PORT,ENCODER_E1A_PIN | ENCODER_E1B_PIN |ENCODER_E2A_PIN|ENCODER_E2B_PIN);
	
	if((gpio_status & ENCODER_E1A_PIN) == ENCODER_E1A_PIN)
	{

		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E1B_PIN))
		{
			raw_coutA++;
		}
		else
		{
			raw_coutA--;
		}
	}
	else if((gpio_status & ENCODER_E1B_PIN)==ENCODER_E1B_PIN)
	{

		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E1A_PIN))
		{
			raw_coutA--;
		}
		else
		{
			raw_coutA++;
		}
	}
	
	
	if((gpio_status & ENCODER_E2A_PIN) == ENCODER_E2A_PIN)
	{

		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E2B_PIN))
		{
			raw_coutB--;
		}
		else
		{
			raw_coutB++;
		}
	}
	else if((gpio_status & ENCODER_E2B_PIN)==ENCODER_E2B_PIN)
	{

		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E2A_PIN))
		{
			raw_coutB++;
		}
		else
		{
			raw_coutB--;
		}
	}

	DL_GPIO_clearInterruptStatus(ENCODER_PORT,ENCODER_E1A_PIN | ENCODER_E1B_PIN |ENCODER_E2A_PIN|ENCODER_E2B_PIN);
}