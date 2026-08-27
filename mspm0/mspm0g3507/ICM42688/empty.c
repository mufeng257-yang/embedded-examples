#include "board.h"
#include <stdio.h>
#include "icm42688.h"
#include "IMU.h"

// VCC--------5V或者3.3V都可以
//IIC 模式接线
// PA0------------------------SDA
// PA1------------------------SCL
//SPI 模式接线
// PB5------------------------CS
// PB7------------------------MISO
// PB8------------------------MOSI
// PB9------------------------SCLK

/*
1.需配置一个定时器，100us周期。
2.使用软件i2c，PA0->SDA PA1->SCL
3.在IMU.c中有一个变量nowtime，在empty.cextern uint32_t nowtime，在100us的定时器中累加这个值
4.定义float ypr[3];
5.imu初始化IMU_init，定时器初始化，清除标志位、开启中断
6.使用这个函数IMU_getYawPitchRoll(ypr);即可获取三个姿态角
*/

void TimeA1_Init(void);
extern uint32_t nowtime;
icm42688RealData_t acc, gyro;

float ypr[3];          // 上传yaw pitch roll的值

int main(void)
{
    unsigned char buff[10] = {0};
	uint32_t printDivider = 0;
    board_init();
    printf("OK");
    delay_ms(100);//等待部署
		IMU_init();
    TimeA1_Init();
    delay_ms(20);
    while (1) 
    { 
			IMU_getYawPitchRoll(ypr);
			if (++printDivider >= 10U)
			{
				printDivider = 0;
				printf("YPR: %.2f %.2f %.2f\r\n",ypr[0], ypr[1], ypr[2]);
			}
			delay_ms(2);
    }
}

void TimeA1_Init(void)
{
	NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
}


void TIMER_1_INST_IRQHandler(void)
{
	switch(DL_TimerA_getPendingInterrupt(TIMER_1_INST))
	{
		case DL_TIMER_IIDX_ZERO:
			nowtime++;
			DL_TimerG_clearInterruptStatus(TIMER_1_INST, DL_TIMER_IIDX_ZERO);
		break;
		default:
			
		break;
	}
}
