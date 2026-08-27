#include "ti_msp_dl_config.h"
#include "stdio.h"
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
#define  delay_ms(x)  delay_cycles(CPUCLK_FREQ/1000*x)
 //采样次数
#define SAMPLES         30
uint32_t ADC_GET(void)
{
    unsigned int gAdcResult = 0;

    //使能ADC转换
    DL_ADC12_enableConversions(ADC0_INST);
    //软件触发ADC开始转换
    DL_ADC12_startConversion(ADC0_INST);

    //如果当前状态 不是 空闲状态
    while (DL_ADC12_getStatus(ADC0_INST) != DL_ADC12_STATUS_CONVERSION_IDLE );

    //清除触发转换状态
    DL_ADC12_stopConversion(ADC0_INST);
    //失能ADC转换
    DL_ADC12_disableConversions(ADC0_INST);

    //获取数据
    gAdcResult = DL_ADC12_getMemResult(ADC0_INST, ADC0_ADCMEM_CH0);

    return gAdcResult;
}
unsigned int Get_Adc_Value(void)
{
    uint32_t Data = 0;

    for(int i = 0; i < SAMPLES; i++)
    {
        Data += ADC_GET();

        delay_ms(5);
    }

    Data = Data / SAMPLES;

    return Data;
}

unsigned int Get_Percentage_Value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;

    adc_new = Get_Adc_Value();
    Percentage_value = ((float)adc_new/(float)adc_max) * 100.f;
    return Percentage_value;
}
int main(void)
{
    SYSCFG_DL_init();

    printf("\nGrayscale Sensor Start...\r\n");

    while (1)
    {
        printf("\n");
        printf("Value      = [ %d ]\r\n", Get_Adc_Value() );
        printf("percentage = [ %d%% ]\r\n", Get_Percentage_Value() );
        delay_ms(300);
    }
}