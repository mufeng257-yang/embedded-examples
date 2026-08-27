#include "serial.h"
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;         
volatile bool stringReady = false; 
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
