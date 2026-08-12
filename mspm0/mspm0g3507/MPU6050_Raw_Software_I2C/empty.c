#include "ti_msp_dl_config.h"
#include "bsp_i2c.h"
#include "MPU6050.h"
int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_0_INST) == true );

        DL_UART_Main_transmitData(UART_0_INST, ch);

        return ch;
}
static uint32_t last_tick = 0;
int16_t ax,ay,az,gx,gy,gz;
int16_t temperature;
int16_t temp_integer;
int16_t temp_decimal;
int main(void)
{
    uint8_t err;
    uint8_t retry;

    SYSCFG_DL_init();
    delay_ms(200);

    for (retry = 0; retry < 5; retry++) {
        err = MPU6050_Init();

        if (err == 0) {
            break;
        }

        delay_ms(20);
    }

    printf("MPU init err=%u\r\n", err);

    if (err != 0) {
        while (1) {
            delay_ms(1000);
        }
    }

    while (1) {
        MPU6050_ReadAccel(&ax, &ay, &az);
        MPU6050_ReadGyro(&gx, &gy, &gz);
        printf("ax=%d,ay=%d,az=%d,gx=%d,gy=%d,gz=%d\r\n",ax, ay, az, gx, gy, gz);     
			
				temperature = MPU6050_ReadTemperatureCenti();

				temp_integer = temperature / 100;
				temp_decimal = temperature % 100;

				if (temp_decimal < 0) {
						temp_decimal = -temp_decimal;
				}

				printf("temperature=%d.%02d C\r\n",
							 temp_integer,
							 temp_decimal);
        delay_ms(100);
    }
}