#include "ti_msp_dl_config.h"
#include "board.h"
#include "tcs34725.h"
#include <stdio.h>
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
    TCS34725_Init();
    printf("\nTCS34725 Demo Start......\r\n");
    delay_ms(1000UL);
    while (1)
    {
        TCS34725_GetRawData(&rgb);
        RGBtoHSL(&rgb,&hsl);
        printf("R[%d]  G[%d]  B[%d]  C[%d]\r\n",rgb.r,rgb.g,rgb.b,rgb.c);
        printf("H=[%d] S=[%d] L=[%d]\r\n",hsl.h,hsl.s,hsl.l);
        printf("\n");
        delay_ms(500UL);
		}
}