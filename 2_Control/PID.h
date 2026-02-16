/**
 ******************************************************************************
 * @file    pid.h
 * @brief   PID算法头文件
 * 
 * @example 完整使用示例（增量式PID - 推荐用于单片机）
 * 
 * #include "pid.h"
 * 
 * PID_t motor_pid;  // 定义PID结构体
 * 
 * void main(void)
 * {
 *     // 初始化PID：kp=2.0, ki=0.5, kd=0.1, 输出范围-1000~1000, 积分限幅0（不限幅）
 *     PID_Init(&motor_pid, 2.0f, 0.5f, 0.1f, 1000.0f, -1000.0f);
 *     
 *     while(1)
 *     {
 *         // 每10ms调用一次
 *         delay_ms(10);
 *         
 *         float measure = get_motor_speed();  // 获取电机实际转速
 *         float target = 1000.0f;              // 目标转速
 *         
 *         float output = PID_Calc(&motor_pid, measure, target);
 *         
 *         set_motor_pwm(output);  // 设置电机PWM
 *     }
 * }
 * 
 ******************************************************************************
 */

#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>

/**
 * @brief PID结构体
 * 
 * 存储PID的所有参数和状态变量
 */
typedef struct
{
    float kp;         /* 比例系数 */
    float ki;         /* 积分系数 */
    float kd;         /* 微分系数 */
    
    float target;     /* 目标值（设定值） */
    float measure;    /* 测量值（反馈值） */
    
    float err;        /* 当前误差 */
    float err_last;   /* 上一次误差 */
    float err_llast;  /* 上上次误差（增量式专用） */
    float err_sum;    /* 误差累计（积分项，位置式专用） */
    
    float out;        /* PID输出 */
    float out_last;   /* 上一次输出（增量式专用） */
    float out_max;    /* 最大输出限幅 */
    float out_min;    /* 最小输出限幅 */
    
} PID_t;

/**
 * @brief   PID初始化函数
 * @param   pid        - PID结构体指针
 * @param   kp         - 比例系数
 * @param   ki         - 积分系数
 * @param   kd         - 微分系数
 * @param   out_max    - 最大输出值
 * @param   out_min    - 最小输出值
 * @retval  无
 */
void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_max, float out_min);

/**
 * @brief   PID重置函数
 * @param   pid        - PID结构体指针
 * @retval  无
 */
void PID_Reset(PID_t *pid);

/**
 * @brief   增量式PID计算函数（默认使用）
 * @param   pid        - PID结构体指针
 * @param   measure    - 实际测量值（反馈值）
 * @param   target     - 目标值（设定值）
 * @retval  float      - PID输出值
 * @note    适合单片机，无积分饱和问题
 */
float PID_Calc(PID_t *pid, float measure, float target);

/**
 * @brief   位置式PID计算函数
 * @param   pid        - PID结构体指针
 * @param   measure    - 实际测量值（反馈值）
 * @param   target     - 目标值（设定值）
 * @retval  float      - PID输出值
 */
float PID_Position_Calc(PID_t *pid, float measure, float target);

#endif
