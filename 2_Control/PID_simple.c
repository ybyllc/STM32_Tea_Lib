/**
 ******************************************************************************
 * @file    PID_simple.c
 * @brief   最简单的PID算法实现
 * @example 使用示例：
 *     float target = 50;      // 目标值，比如速度50
 *     float current;          // 当前测量值
 *     float out;              // 输出值
 *     
 *     // 在定时器中断中每10ms调用一次
 *     current = get_speed();              // 读取当前速度
 *     out = PID(target - current);        // 计算PID输出
 *     set_motor_pwm(out);                 // 设置电机PWM
 * 
 ******************************************************************************
 */

#include "PID_simple.h"

/**
 * @brief   PID计算函数
 * @param   err 误差值（目标值 - 当前值）
 */
float PID_1(float err)
{
    float kP = 2.0f;        // 比例系数
    float kI = 0.1f;        // 积分系数
    float kD = 0.5f;        // 微分系数
    
    static float I = 0.0f;      // 积分累计
    static float D = 0.0f;      // 微分值
    static float last_err = 0.0f;   // 上次误差
    static float out = 0.0f;        // 输出值
    
    I += err;                       // 积分累加
    D = err - last_err;             // 计算微分
    last_err = err;                 // 保存本次误差
    
    out = kP * err + kI * I + kD * D;   // PID计算
    
    return out;
}
