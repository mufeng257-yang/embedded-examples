#ifndef __IOI2C_H
#define __IOI2C_H
#include "ti_msp_dl_config.h"

/*-----------------------修改软件i2c的引脚，此处SCL->PA26 SDA->PA27------------------------*/

#define I2C_PORT																						 (OLED_PORT)

#define I2C_SCL_PINCM																		(OLED_SCL_IOMUX)

#define I2C_SDA_PINCM       														(OLED_SDA_IOMUX)

#define I2C_SCL_PIN    																	  (OLED_SCL_PIN)
#define I2C_SDA_PIN    																	  (OLED_SDA_PIN)

/*-----------------------修改软件i2c的引脚，此处SCL->PA26 SDA->PA27-------------------------*/

#define u32 unsigned int
#define u8 unsigned char

#define delay_us(x)  delay_cycles(32*x)
#define delay_ms(x)		delay_cycles(32000*x)

#define SDA_OUT()   SDA_SetOutput()
#define SDA_IN()    SDA_SetInput()

#define IIC_SCL_HIGH()   DL_GPIO_setPins(I2C_PORT, I2C_SCL_PIN)
#define IIC_SCL_LOW()    DL_GPIO_clearPins(I2C_PORT, I2C_SCL_PIN)
#define IIC_SDA_HIGH()   DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN)
#define IIC_SDA_LOW()    DL_GPIO_clearPins(I2C_PORT, I2C_SDA_PIN)
#define READ_SDA()       (DL_GPIO_readPins(I2C_PORT, I2C_SDA_PIN) & I2C_SDA_PIN)

void SDA_SetOutput(void);
void SDA_SetInput(void);

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

int32_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t *data, uint32_t length);
int32_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t* data, uint32_t length);

#endif
