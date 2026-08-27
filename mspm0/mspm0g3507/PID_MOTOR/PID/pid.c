#include "pid.h"

/**
 * PID控制器初始化函数
 * @param pid PID结构体指针
 * @param kp  比例系数
 * @param ki  积分系数
 * @param kd  微分系数
 * @param max 输出上限
 * @param min 输出下限
 */
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float max, float min)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0.0f;        // 积分累加项清零
    pid->last_error = 0.0f;      // 上一次误差清零
    pid->output_max = max;       // 设置输出限幅
    pid->output_min = min;
}
PID_TypeDef jiaoduhuan;
PID_TypeDef Aspeedhuan;
PID_TypeDef Bspeedhuan;
/**
 * PID计算函数（位置式PID实现，但积分项采用了特殊累加方式）
 * @param pid         PID结构体指针
 * @param setpoint    目标值
 * @param measurement 当前测量值
 * @return            计算后的控制输出（已限幅）
 */
float PID_Compute(PID_TypeDef *pid, float setpoint, float measurement)
{
    // 计算当前误差
    float error = setpoint - measurement;

    // ---------- 比例项 ----------
    // P = Kp * error
    float proportional = pid->Kp * error;

    // ---------- 积分项（带动态限幅） ----------
    // 【注意】此处积分累加方式与传统不同：累加了 (error + last_error) 而非 error
    // 这相当于在离散域用梯形法则近似积分，理论上是更精确的积分近似方式
    if (pid->Ki != 0.0f) {
        pid->integral += error + pid->last_error;  // 梯形积分累加：∑(e(k)+e(k-1))
        
        // 积分分离/抗饱和处理：根据输出上限动态限制积分累加量
        // limit = output_max / Ki，即积分项最大贡献值不超过输出上限
        // 这是一种有效的抗积分饱和方法，但需注意当 Ki 很小时 limit 会非常大，削弱限制效果
        float limit = pid->output_max / pid->Ki;
        if (pid->integral > limit) pid->integral = limit;
        else if (pid->integral < -limit) pid->integral = -limit;
    } else {
        pid->integral = 0.0f;    // 如果积分系数为0，强制清零积分项
    }
    // 积分输出 = Ki * integral
    float integral_out = pid->Ki * pid->integral;

    // ---------- 微分项 ----------
    // 使用误差的差分近似微分：D = Kd * (error - last_error)
    // 这是“位置式”微分，仅基于误差变化，而非测量值微分（后者可避免微分冲击）
    float derivative = pid->Kd * (error - pid->last_error);
    pid->last_error = error;    // 更新上一次误差，供下次计算使用

    // ---------- 合成输出 ----------
    float output = proportional + integral_out + derivative;

    // ---------- 输出限幅 ----------
    if (output > pid->output_max) output = pid->output_max;
    else if (output < pid->output_min) output = pid->output_min;

    return output;
}

/**
 * PID控制器状态重置函数
 * 清除积分累加项和上一次误差，适用于控制模式切换或目标值大幅变化时
 * @param pid PID结构体指针
 */
void PID_Reset(PID_TypeDef *pid)
{
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
}
