#include "SR04.h"
#include "ti_msp_dl_config.h"
#include "stdio.h"
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}

volatile uint32_t msHcCount = 0; 

float distance = 0;
uint8_t SR04_Flag = 0; 

void SR04_Init(void)
{
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

void Open_Timer(void)
{

    DL_TimerG_setTimerCount(TIMER_0_INST, 0); 

    msHcCount = 0;

    DL_TimerG_startCounter(TIMER_0_INST);  
}

uint32_t Get_TIMER_Count(void)
{
    uint32_t time  = 0;
    time   = msHcCount * 1000;                       
    time  += DL_TimerG_getTimerCount(TIMER_0_INST);  
    DL_TimerG_setTimerCount(TIMER_0_INST, 0); 
    delay_ms(1);
    return time ;
}

void Close_Timer(void)
{
    DL_TimerG_stopCounter(TIMER_0_INST);  
}

void TIMER_0_INST_IRQHandler(void)
{
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMERA_IIDX_LOAD:
                msHcCount++;
            break;
        default:
            break;
    }
}

void GROUP1_IRQHandler(void)
{
    switch( DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) )
    {
        case SR04_INT_IIDX:
            if( SR04_ECHO() ) 
            {
                SR04_Flag = 1;
                distance = 0.0;
                Open_Timer();  
            }
            else 
            {
                NVIC_DisableIRQ(SR04_INT_IRQN); 
                Close_Timer();  
                SR04_Flag = 0;
                distance = (float)Get_TIMER_Count() / 58.0f;  
            }
        break;
    }
}

float SR04_GetLength(void)
{
    float distances[5] = {0}; 
    uint32_t TimeOut = 1000;
    uint8_t valid_count = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        msHcCount = 0;
        SR04_Flag = 0; 
        TimeOut = 90000; 
        NVIC_EnableIRQ(SR04_INT_IRQN);
        delay_ms(10);
        SR04_TRIG(0); 
        delay_1us(10);
        SR04_TRIG(1); 
        delay_1us(15); 
        SR04_TRIG(0); 
        while (SR04_Flag == 1 && TimeOut)
        {
            TimeOut--;
        }
        if (TimeOut == 0) 
        {
            printf("SR04 Time Out!");
            continue;
        }
        distances[valid_count++] = distance;
    }
    NVIC_DisableIRQ(SR04_INT_IRQN);
    if (valid_count < 3) 
    {
        printf("Not enough valid measurements!");
        return 0;
    }
    for (uint8_t i = 0; i < valid_count - 1; i++)
    {
        for (uint8_t j = i + 1; j < valid_count; j++)
        {
            if (distances[i] > distances[j])
            {
                float temp = distances[i];
                distances[i] = distances[j];
                distances[j] = temp;
            }
        }
    }
    float sum = 0;
    for (uint8_t i = 1; i < valid_count - 1; i++)
    {
        sum += distances[i];
    }
    return sum / (valid_count - 2);
}