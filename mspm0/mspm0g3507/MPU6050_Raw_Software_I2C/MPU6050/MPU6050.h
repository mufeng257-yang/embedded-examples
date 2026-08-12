#ifndef __MPU6050_H
#define __MPU6050_H
#include "ti_msp_dl_config.h"

uint8_t MPU6050_ReadReg(uint8_t reg, uint8_t num,uint8_t *buf);
uint8_t MPU6050_Init();
void MPU6050_ReadAccel(int16_t *ax,int16_t *ay,int16_t *az);
void MPU6050_ReadGyro(int16_t *gx,int16_t *gy,int16_t *gz);
int16_t MPU6050_ReadTemperatureCenti(void);
#endif
