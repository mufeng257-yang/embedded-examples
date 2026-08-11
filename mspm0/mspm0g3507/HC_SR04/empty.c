#include "ti_msp_dl_config.h"
#include "SR04.h"
#include "stdio.h"
//ON Use MicroLIB
int mode=0;
int main(void)
{
    SYSCFG_DL_init();
		SR04_Init();

    while (1) {
			uint32_t Value = (int)SR04_GetLength();

      printf("Distance = %dCM\r\n", Value);

      delay_ms(500);
    }

}
