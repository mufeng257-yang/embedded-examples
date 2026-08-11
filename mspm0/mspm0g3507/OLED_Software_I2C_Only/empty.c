#include "ti_msp_dl_config.h"

#include <stdio.h>
#include "IOI2C.h"
#include "OLED.h" 

int cnt = 0;
char cnt_buf[20];

int main(void)
{
    SYSCFG_DL_init();

		OLED_Init();
		OLED_Clear();
    while (1) {
			sprintf(cnt_buf,"cnt=%d",cnt);
			OLED_ShowString(0,1,(uint8_t*)cnt_buf,16);
			cnt++;
    }
}