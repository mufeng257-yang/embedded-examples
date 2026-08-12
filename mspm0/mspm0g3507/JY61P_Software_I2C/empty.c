#include "ti_msp_dl_config.h"
#include "bsp_gyro.h"

int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
//Stack_Size      EQU     0x00000800
int main(void)
{
    SYSCFG_DL_init();

    jy61pInit();

    printf("\r\nJY61P 3D Attitude Measurement Sensor Start...\r\n");

    while (1)
    {
        Gyro_Struct *JY61P_Data = get_angle();

        printf("\n");
        printf("RollX  = [ %0.2f ] ",JY61P_Data->x);
        printf("PitchY = [ %0.2f ] ",JY61P_Data->y);
        printf("YawZ   = [ %0.2f ]\r\n",JY61P_Data->z);
        delay_ms(100);
    }
}