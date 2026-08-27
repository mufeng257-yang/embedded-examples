#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "ti_msp_dl_config.h"
#include "stdio.h"
#define RX_BUFFER_SIZE 128 
extern volatile char rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxIndex;
extern volatile bool stringReady;
extern volatile unsigned char  rxChar;
int fputc(int ch, FILE *stream);
void UART_0_INST_IRQHandler(void);
#endif