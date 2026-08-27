#include "ti_msp_dl_config.h"
#include "bsp_dht11.h"

int main(void)
{
    SYSCFG_DL_init();

    // 延时等待稳定
    delay_ms(1000);

    printf("DHT11 demo start....\r\n");

    while (1)
    {
        //读取模块数据
        DHT11_Read_Data();
        delay_ms(500);
    }
}
