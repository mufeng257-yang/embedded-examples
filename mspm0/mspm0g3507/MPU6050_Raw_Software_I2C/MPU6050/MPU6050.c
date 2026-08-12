#include "MPU6050.h"
#include "bsp_i2c.h"
#define MPU6050_ADDR		0xD0		//MPU6050的I2C从机地址
#define PWR_MGMT_1      0x6B
#define	ACCEL_XOUT_H    0x3B 
#define GYRO_XOUT_H     0x43 
#define MPU6050_WHO_AM_I_REG  0x75
#define SMPRT_DIV      0x19
#define CONFIG         0x1A
#define GYRO_CONFIG    0x1B
#define ACCEL_CONFIG   0x1C
#define TEMP_OUT_H  	 0x41

uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t val)
{
    IIC_Start();
    IIC_Send_Byte(0xD0); // AD0=0，写地址0x68<<1=0xD0
    if( IIC_Wait_Ack() == 1 ) {IIC_Stop();return 1;}
    
    IIC_Send_Byte(reg);  // 发送要操作的寄存器地址
    if( IIC_Wait_Ack() == 1 ) {IIC_Stop();return 2;}
    
    IIC_Send_Byte(val);  // 写入配置值
    if( IIC_Wait_Ack() == 1 ) {IIC_Stop();return 3;}
    
    IIC_Stop();
    return 0; // 写入成功
}
uint8_t MPU6050_ReadReg(uint8_t reg, uint8_t num,uint8_t *buf)
{
		uint8_t i;
    IIC_Start();
    IIC_Send_Byte(0xD0);
    if( IIC_Wait_Ack() == 1 ){IIC_Stop();return 1;}
    
    IIC_Send_Byte(reg);
    if( IIC_Wait_Ack() == 1 ) {IIC_Stop();return 2;}
    
    IIC_Start(); // 重复起始
    IIC_Send_Byte(0xD1); // 读地址
    if( IIC_Wait_Ack() == 1 ) {IIC_Stop();return 3;}
    
		for(i=0;i<(num-1);i++){
                buf[i]=IIC_Read_Byte();
                IIC_Send_Ack(0);
        }
		buf[i]=IIC_Read_Byte();
		IIC_Send_Ack(1);// 最后一字节发NACK
    IIC_Stop();
    return 0;
}
int16_t MPU6050_ReadData(uint8_t reg){
	uint8_t data[2]={0};

	if(MPU6050_ReadReg(reg,2,data)!=0) MPU6050_ReadReg(reg,2,data);

	return (int16_t)((data[0]<<8)|data[1]);
}
///**
//  * 函    数：MPU6050初始化
//  * 参    数：无
//  * 返 回 值：无
//  */
uint8_t MPU6050_Init(void)
{
    uint8_t err;

    err = MPU6050_WriteReg(0x6B, 0x00);
    if (err) return err;

    delay_ms(10);

    err = MPU6050_WriteReg(0x19, 0x07);
    if (err) return err;

    err = MPU6050_WriteReg(0x1A, 0x03);
    if (err) return err;

    err = MPU6050_WriteReg(0x1B, 0x18);
    if (err) return err;

    err = MPU6050_WriteReg(0x1C, 0x00);
    if (err) return err;

    return 0;
}
void MPU6050_ReadAccel(int16_t *ax,int16_t *ay,int16_t *az){
	*ax=MPU6050_ReadData(ACCEL_XOUT_H);
	*ay=MPU6050_ReadData(ACCEL_XOUT_H+2);
	*az=MPU6050_ReadData(ACCEL_XOUT_H+4);
}
void MPU6050_ReadGyro(int16_t *gx,int16_t *gy,int16_t *gz){
	*gx=MPU6050_ReadData(GYRO_XOUT_H);
	*gy=MPU6050_ReadData(GYRO_XOUT_H+2);
	*gz=MPU6050_ReadData(GYRO_XOUT_H+4);
}
int16_t MPU6050_ReadTemperatureCenti(void)
{
    int16_t raw;
    int32_t temp;

    raw = MPU6050_ReadData(TEMP_OUT_H);

    /* 0.01°C */
    temp = ((int32_t)raw * 100) / 340 + 3653;

    return (int16_t)temp;
}



