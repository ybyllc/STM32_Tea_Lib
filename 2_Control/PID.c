/**
 ******************************************************************************
 * @file    pid.c
 * @brief   PID算法
 * 
 * @details 本文件实现了位置式和增量式两种PID控制算法：
 *          - PID_Calc():          增量式PID（默认，适合单片机）
 *          - PID_Position_Calc(): 位置式PID
 *          - PID_test():          最简单的学习用PID（内置static变量）
 ******************************************************************************
 */

#include "pid.h"

/**
 * @brief   PID初始化函数
 * @param   pid        - PID结构体指针
 * @param   kp         - 比例系数
 * @param   ki         - 积分系数
 * @param   kd         - 微分系数
 * @param   out_max    - 最大输出值
 * @param   out_min    - 最小输出值
 * @retval  无
 * @note    使用前必须先调用此函数初始化PID参数
 */
void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_max, float out_min)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    
    pid->target = 0.0f;
    pid->measure = 0.0f;
    
    pid->err = 0.0f;
    pid->err_last = 0.0f;
    pid->err_llast = 0.0f;
    pid->err_sum = 0.0f;
    
    pid->out = 0.0f;
    pid->out_last = 0.0f;
    pid->out_max = out_max;
    pid->out_min = out_min;
}

/**
 * @brief   PID重置函数
 * @param   pid        - PID结构体指针
 * @retval  无
 * @note    当需要重新开始控制时调用，清除历史误差
 */
void PID_Reset(PID_t *pid)
{
    pid->err = 0.0f;
    pid->err_last = 0.0f;
    pid->err_llast = 0.0f;
    pid->err_sum = 0.0f;
    pid->out = 0.0f;
    pid->out_last = 0.0f;
}

/**
 * @brief   增量式PID计算函数（默认使用）
 * @param   pid        - PID结构体指针
 * @param   measure    - 实际测量值（反馈值）
 * @param   target     - 目标值（设定值）
 * @retval  float      - PID输出值
 * @note    此函数需要定时调用（例如每隔10ms调用一次）
 *          适合单片机，无积分饱和问题
 * @formula 
 *          误差 = 目标值 - 测量值
 *          增量 = Kp*(本次误差-上次误差) + Ki*本次误差 + Kd*(本次误差-2*上次误差+上上次误差)
 */
float PID_Calc(PID_t *pid, float measure, float target)
{
    float delta_out;
    
    pid->measure = measure;
    pid->target = target;
    
    pid->err = pid->target - pid->measure;
    
    delta_out = pid->kp * (pid->err - pid->err_last)
              + pid->ki * pid->err
              + pid->kd * (pid->err - 2 * pid->err_last + pid->err_llast);
    
    pid->out_last += delta_out;
    
    if (pid->out_last > pid->out_max)
        pid->out_last = pid->out_max;
    else if (pid->out_last < pid->out_min)
        pid->out_last = pid->out_min;
    
    pid->err_llast = pid->err_last;
    pid->err_last = pid->err;
    
    pid->out = pid->out_last;
    
    return pid->out;
}

/**
 * @brief   位置式PID计算函数
 * @param   pid        - PID结构体指针
 * @param   measure    - 实际测量值（反馈值）
 * @param   target     - 目标值（设定值）
 * @retval  float      - PID输出值
 * @note    此函数需要定时调用（例如每隔10ms调用一次）
 * @formula 
 *          误差 = 目标值 - 测量值
 *          P项 = Kp * 误差
 *          I项 = Ki * 误差累计
 *          D项 = Kd * (本次误差 - 上次误差)
 *          输出 = P项 + I项 + D项
 */
float PID_Position_Calc(PID_t *pid, float measure, float target)
{
    float p_out, i_out, d_out;
    
    pid->measure = measure;
    pid->target = target;
    
    pid->err = pid->target - pid->measure;
    pid->err_sum += pid->err;
    
    p_out = pid->kp * pid->err;
    i_out = pid->ki * pid->err_sum;
    d_out = pid->kd * (pid->err - pid->err_last);
    
    pid->out = p_out + i_out + d_out;
    
    if (pid->out > pid->out_max)
        pid->out = pid->out_max;
    else if (pid->out < pid->out_min)
        pid->out = pid->out_min;
    
    pid->err_last = pid->err;
    
    return pid->out;
}