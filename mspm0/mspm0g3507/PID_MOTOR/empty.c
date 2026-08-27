#include "ti_msp_dl_config.h"
#include "encoder.h"
#include "interrupt.h"
#include "motor.h"
#include "pid.h"
#include "serial.h"
int A,B;
int main(void)
{
    PID_Init(&Aspeedhuan, 4, 0, 0.2f, 500, -500);
    PID_Init(&Bspeedhuan, 4, 0, 0.2f, 500, -500);
    SYSCFG_DL_init();
    TIMER_ENCODER_init();
    encoder_init();
		A=0;
		B=0;
		while(1){
		printf("%d,%d\n",coutA,coutB);
		}
    return 0;
}