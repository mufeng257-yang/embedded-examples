#include "ti_msp_dl_config.h"
#include "stdio.h"
//ON Use MicroLIB
#define RX_BUFFER_SIZE 128 
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;         
volatile bool stringReady = false; 
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
int x=0;
int y=0;
int main(void)
{
    SYSCFG_DL_init();
		NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
		printf("start....\r\n");
    while (1) {
			if (stringReady) 
			{
			sscanf((char*)rxBuffer, "%d,%d", &x, &y);
			stringReady = false;
			}
			printf("x=%d,y=%d\r\n",x,y);
			delay_cycles(3200000);
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