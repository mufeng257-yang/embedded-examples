#include "IOI2C.h"

void Delay(u32 count)
{
    delay_cycles(32*count);
}

void SDA_SetOutput(void)
{
   DL_GPIO_initDigitalOutput(I2C_SDA_PINCM);    //配功能
   DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_14);      //拉高
   DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_14);  //使能输出
}

void SDA_SetInput(void)
{		
    DL_GPIO_initDigitalInputFeatures(OLED_SDA_IOMUX,
		DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

void IIC_Init(void)
{
    SDA_SetOutput();

    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
}

void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();

    Delay(5); 
    IIC_SDA_LOW(); 

    Delay(5);
    IIC_SCL_LOW(); 
}

void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL_LOW(); 
    IIC_SDA_LOW(); 

    Delay(5);
    IIC_SCL_HIGH();

    IIC_SDA_HIGH();

    Delay(5);
}

u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;

		SDA_OUT();
	
    IIC_SDA_HIGH();
	
		Delay(4);
	
		SDA_IN();

    Delay(5);

    while (READ_SDA())
    {
        ucErrTime++;
        if (ucErrTime > 50)
        {
            IIC_Stop();
            return 1;
        }
        Delay(5);
    }

    IIC_SCL_HIGH();
    Delay(5);
    IIC_SCL_LOW();
    return 0;
}

void IIC_Ack(void)
{
    IIC_SCL_LOW();
    SDA_OUT(); 
    IIC_SDA_LOW(); 

    Delay(5);
    IIC_SCL_HIGH();
    Delay(5);
    IIC_SCL_LOW(); 
}

void IIC_NAck(void)
{
    IIC_SCL_LOW();
    SDA_OUT();
    IIC_SDA_HIGH();

    Delay(5);
    IIC_SCL_HIGH();
    Delay(5);
    IIC_SCL_LOW();
}

void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT(); 
    IIC_SCL_LOW();

    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80)>>7)
            IIC_SDA_HIGH(); 
        else
            IIC_SDA_LOW(); 
				txd<<=1; 
        Delay(2);
        IIC_SCL_HIGH();
        Delay(5);
        IIC_SCL_LOW(); 
        Delay(3);
    }
}

u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;

    SDA_IN();

    for (i = 0; i < 8; i++)
    {
        IIC_SCL_LOW();

        Delay(5);
        IIC_SCL_HIGH();
        receive <<= 1;
        if (READ_SDA())
            receive++;

        Delay(5);
    }

    if (ack)
        IIC_Ack();
    else
        IIC_NAck();

    return receive;
}

int32_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t *data, uint32_t length)
{
    uint32_t count = 0;

    IIC_Start();
    IIC_Send_Byte(dev);
    if (IIC_Wait_Ack() == 1) return 0; 

    IIC_Send_Byte(reg); 
    if (IIC_Wait_Ack() == 1) return 0; 

    IIC_Start();  
    IIC_Send_Byte(dev + 1); 
    if (IIC_Wait_Ack() == 1) return 0; 

    for (count = 0; count < length; count++)
    {
        if (count != length - 1)             
            data[count] = IIC_Read_Byte(1);
        else
            data[count] = IIC_Read_Byte(0);  
    }

    IIC_Stop();                               
    return 1;
}

int32_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t* data, uint32_t length)
{
    uint32_t count = 0;

    IIC_Start();
    IIC_Send_Byte(dev);
    if (IIC_Wait_Ack() == 1) return 0;

    IIC_Send_Byte(reg);
    if (IIC_Wait_Ack() == 1) return 0;

    for (count = 0; count < length; count++)
    {
        IIC_Send_Byte(data[count]);
        if (IIC_Wait_Ack() == 1) return 0;
    }

    IIC_Stop();
    return 1;
}