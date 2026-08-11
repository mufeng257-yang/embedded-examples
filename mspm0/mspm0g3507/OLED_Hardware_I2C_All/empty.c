#include "ti_msp_dl_config.h"
#include "OLED.h" 

int main(void)
{
		uint8_t t=0;
    SYSCFG_DL_init();
		OLED_init(); 
		OLED_operate_gram(PEN_CLEAR);
		OLED_show_string(0,6,(uint8_t*)"hello");
		OLED_show_HZ(0,0,0);  // Yang
    while (1) {
			t++;
			OLED_printf(0,12,"%d",t);
			delay_cycles(3200000);
			OLED_refresh_gram();
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