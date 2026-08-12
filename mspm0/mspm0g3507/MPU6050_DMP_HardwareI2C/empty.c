#include "ti_msp_dl_config.h"
#include "clock.h"
#include "interrupt.h"
#include "mpu6050.h"
#include "stdio.h"

int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
//ON Use MicroLIB
int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();
    MPU6050_Init();
    Interrupt_Init();
		printf("MPU6050 Start\n");
    while (1) 
    {
			if(mpu6050_flag){
				printf("pitch=%0.2f,roll=%0.2f, yaw=%0.2f\n",pitch, roll, yaw);
				mpu6050_flag=0;
			}
			delay_cycles(3200000);
    }
}
