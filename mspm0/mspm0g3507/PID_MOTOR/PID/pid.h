#ifndef __PID_H
#define __PID_H
#include <stdint.h>
typedef struct {
    float Kp;          // 比例系数
    float Ki;          // 积分系数
    float Kd;          // 微分系数
    float integral;    // 积分累积
    float last_error;  // 上次误差（用于微分）
    float output_max;  // 输出限幅上限（正值）
    float output_min;  // 输出限幅下限（负值）
} PID_TypeDef;
extern PID_TypeDef jiaoduhuan;
extern PID_TypeDef Aspeedhuan;
extern PID_TypeDef Bspeedhuan;
// 初始化PID参数
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float max, float min);
float PID_Compute(PID_TypeDef *pid, float setpoint, float measurement);
void PID_Reset(PID_TypeDef *pid);

#endif
