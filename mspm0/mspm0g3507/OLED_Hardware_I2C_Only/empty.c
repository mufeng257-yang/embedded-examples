#include "ti_msp_dl_config.h"
#include "oled_hardware_i2c.h"
#include "stdio.h"
#include "clock.h"
uint8_t oled_buffer[32];
		
int main(void)
{
    SYSCFG_DL_init();
		SysTick_Init();
    OLED_Init();
    OLED_ShowString(0,0,(uint8_t *)"hello",16);
		int num=0.0;
    while (1) 
    {
        sprintf((char *)oled_buffer, "%d", num);
        OLED_ShowString(5*8,0,oled_buffer,16);
				num++;
    }
}
/*
 * Automatically ensure that the end of the image is 8-byte aligned.
 * The scatter file places the .flash_image_padding section at the end
 * of the Flash image using the +Last attribute.
 */
#if defined(__ARMCC_VERSION)
__attribute__((used, aligned(8), section(".flash_image_padding")))
static const uint64_t keil_flash_image_padding =
    UINT64_C(0xFFFFFFFFFFFFFFFF);
#endif