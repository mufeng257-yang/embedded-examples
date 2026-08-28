#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "string.h"
#define RX_BUFFER_SIZE 128 
#define  delay_ms(x)  delay_cycles(32000*x)
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}


void usart_send(uint8_t data)
{
    while (DL_UART_isTXFIFOFull(UART_1_INST) == true);
    DL_UART_Main_transmitData(UART_1_INST, data);
}
void USART_SEND(uint8_t *data, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++)
    {
        usart_send(data[i]);
    }
}
uint8_t BCC_Sum1(uint8_t *usart_data, unsigned char Count_Number)
{
    unsigned char crc_sum = 0, k;

    for (k = 0; k < Count_Number; k++)
    {
        crc_sum = crc_sum ^ usart_data[k];
    }

    return crc_sum;
}
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;         
volatile bool stringReady = false; 
int Encoder_cnt, Encoder_pr;
int Motor1_Speed = 50, Motor2_Speed = 50;
int motor1_Current_Speed, motor2_Current_Speed;
int Motor1_T_Position = 500, Motor2_T_Position = 0;
int Motor1_Current_Position, Motor2_Current_Position;
uint8_t usart_receive_data[10];
volatile bool flag_re=0;
uint8_t motor1_ID = 1, motor2_ID = 2;                         /* 电机 ID */
uint8_t motor1_Mode = 1, motor2_Mode = 1;                     /* 1 为多圈位置闭环模式 */

/* 以下协议数组保持参考例程写法：帧头 0x7A，帧尾 0x7B，中间 BCC 异或校验。 */
uint8_t motor1_Enable_data[5] = {0x7A, 0x01, 0x06, 0x7D, 0x7B};
uint8_t motor2_Enable_data[5] = {0x7A, 0x02, 0x06, 0x7E, 0x7B};

uint8_t motor1_Mode_data[7] = {0x7A, 0x01, 0x00, 0x00, 0x00, 0x7B, 0x7B};
uint8_t motor2_Mode_data[7] = {0x7A, 0x02, 0x00, 0x00, 0x00, 0x78, 0x7B};

uint8_t motor1_Speed_data_task[7] = {0x7A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x7B};
uint8_t motor2_Speed_data_task[7] = {0x7A, 0x02, 0x01, 0x00, 0x00, 0x00, 0x7B};

uint8_t motor1_Position_data_task[9] = {0x7A, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7B};
uint8_t motor2_Position_data_task[9] = {0x7A, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7B};

/* 0x0E 0x01 请求当前位置反馈。 */
uint8_t motor1_Feedback_data[6] = {0x7A, 0x01, 0x0E, 0x01, 0x74, 0x7B};
uint8_t motor2_Feedback_data[6] = {0x7A, 0x02, 0x0E, 0x01, 0x77, 0x7B};
int main(void)
{
    SYSCFG_DL_init();
		DL_UART_Main_enableInterrupt(UART_1_INST, DL_UART_MAIN_INTERRUPT_RX);
		NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
		DL_UART_Main_enableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_RX);
		NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
		usart_send(0x00);
    delay_ms(1500);
	/* 1. 使能电机。 */
    motor1_Enable_data[1] = motor1_ID;
    motor1_Enable_data[3] = BCC_Sum1(motor1_Enable_data, 3);
    USART_SEND(motor1_Enable_data, sizeof(motor1_Enable_data));
    delay_ms(1);

    motor2_Enable_data[1] = motor2_ID;
    motor2_Enable_data[3] = BCC_Sum1(motor2_Enable_data, 3);
    USART_SEND(motor2_Enable_data, sizeof(motor2_Enable_data));
    delay_ms(1);

    /* 2. 选择位置闭环模式。 */
    motor1_Mode_data[1] = motor1_ID;
    motor1_Mode_data[3] = motor1_Mode >> 8;
    motor1_Mode_data[4] = motor1_Mode;
    motor1_Mode_data[5] = BCC_Sum1(motor1_Mode_data, 5);
    USART_SEND(motor1_Mode_data, sizeof(motor1_Mode_data));
    delay_ms(1);

    motor2_Mode_data[1] = motor2_ID;
    motor2_Mode_data[3] = motor2_Mode >> 8;
    motor2_Mode_data[4] = motor2_Mode;
    motor2_Mode_data[5] = BCC_Sum1(motor2_Mode_data, 5);
    USART_SEND(motor2_Mode_data, sizeof(motor2_Mode_data));
    delay_ms(1);

    /* 3. 设置位置模式运行速度。 */
    motor1_Speed_data_task[3] = (int)Motor1_Speed >> 8;
    motor1_Speed_data_task[4] = (int)Motor1_Speed;
    motor1_Speed_data_task[5] = BCC_Sum1(motor1_Speed_data_task, 5);
    motor2_Speed_data_task[3] = (int)Motor2_Speed >> 8;
    motor2_Speed_data_task[4] = (int)Motor2_Speed;
    motor2_Speed_data_task[5] = BCC_Sum1(motor2_Speed_data_task, 5);
    USART_SEND(motor1_Speed_data_task, sizeof(motor1_Speed_data_task));
    delay_ms(10);
    USART_SEND(motor2_Speed_data_task, sizeof(motor2_Speed_data_task));
    delay_ms(10);
    while (1) {
			if (stringReady) 
			{
			sscanf((char*)rxBuffer, "%d,%d", &Motor1_T_Position, &Motor2_T_Position);
			stringReady = false;
			}
			if(flag_re){
			printf("x=%d,y=%d\n",Motor1_Current_Position,Motor2_Current_Position);
			}
			
			motor1_Position_data_task[3] = (int)Motor1_T_Position >> 24;
			motor1_Position_data_task[4] = (int)Motor1_T_Position >> 16;
			motor1_Position_data_task[5] = (int)Motor1_T_Position >> 8;
			motor1_Position_data_task[6] = (int)Motor1_T_Position;
			motor1_Position_data_task[7] = BCC_Sum1(motor1_Position_data_task, 7);
			USART_SEND(motor1_Position_data_task, sizeof(motor1_Position_data_task));
			delay_ms(10);

			motor2_Position_data_task[3] = (int)Motor2_T_Position >> 24;
			motor2_Position_data_task[4] = (int)Motor2_T_Position >> 16;
			motor2_Position_data_task[5] = (int)Motor2_T_Position >> 8;
			motor2_Position_data_task[6] = (int)Motor2_T_Position;
			motor2_Position_data_task[7] = BCC_Sum1(motor2_Position_data_task, 7);
			USART_SEND(motor2_Position_data_task, sizeof(motor2_Position_data_task));
			delay_ms(10);
			USART_SEND(motor1_Feedback_data, sizeof(motor1_Feedback_data));
			delay_ms(10);
			USART_SEND(motor2_Feedback_data, sizeof(motor2_Feedback_data));
			delay_ms(10);
    }
}
void UART_0_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX:{
            char rxChar = DL_UART_Main_receiveData(UART_0_INST);
						if (rxChar == '\n' || rxChar == '\r') {
								if (rxIndex > 0) {
										rxBuffer[rxIndex] = '\0';
										stringReady = true;     
										rxIndex = 0;       
								}
						} 
						else {
								if (rxIndex < (RX_BUFFER_SIZE - 1)) {
										rxBuffer[rxIndex] = rxChar;
										rxIndex++;
								} else {
										rxBuffer[RX_BUFFER_SIZE - 1] = '\0';
										stringReady = true;
										rxIndex = 0;
								}
						}
				}
            break;
						
        default:
            break;
								
				}
}
void UART_1_INST_IRQHandler(void)
{
    uint8_t Usart1_Receive;
    static uint8_t Count = 0;

    if (DL_UART_Main_getPendingInterrupt(UART_1_INST) == DL_UART_MAIN_IIDX_RX)
    {
        Usart1_Receive = DL_UART_Main_receiveData(UART_1_INST);

        /* 等待帧头 0x7A，避免错误字节导致接收错位。 */
        if (Count == 0 && Usart1_Receive != 0x7A)
        {
            return;
        }

        usart_receive_data[Count++] = Usart1_Receive;

        if (Count >= 9)
        {
            Count = 0;

            /* 帧尾必须为 0x7B。 */
            if (usart_receive_data[8] != 0x7B)
            {
                return;
            }

            /* 第 8 字节为前 7 字节 BCC 校验。 */
            if (usart_receive_data[7] == BCC_Sum1(usart_receive_data, 7))
            {
                /* 地址 0x01，反馈类型 0x01：1 号电机实时位置。 */
                if (usart_receive_data[1] == 0x01)
                {
                    if (usart_receive_data[2] == 0x01)
                    {
                        Motor1_Current_Position = (usart_receive_data[3] << 24) +
                                                  (usart_receive_data[4] << 16) +
                                                  (usart_receive_data[5] << 8) +
                                                   usart_receive_data[6];
                    }
                    else if (usart_receive_data[2] == 0x00)
                    {
                        motor1_Current_Speed = (usart_receive_data[3] << 24) +
                                               (usart_receive_data[4] << 16) +
                                               (usart_receive_data[5] << 8) +
                                                usart_receive_data[6];
                    }
                }

                /* 地址 0x02，反馈类型 0x01：2 号电机实时位置。 */
                if (usart_receive_data[1] == 0x02)
                {
                    if (usart_receive_data[2] == 0x01)
                    {
                        Motor2_Current_Position = (usart_receive_data[3] << 24) +
                                                  (usart_receive_data[4] << 16) +
                                                  (usart_receive_data[5] << 8) +
                                                   usart_receive_data[6];
                    }
                    else if (usart_receive_data[2] == 0x00)
                    {
                        motor2_Current_Speed = (usart_receive_data[3] << 24) +
                                               (usart_receive_data[4] << 16) +
                                               (usart_receive_data[5] << 8) +
                                                usart_receive_data[6];
                    }
                }
								flag_re=1;
            }
        }
    }
}