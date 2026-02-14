/**
 * @file car_task.h
 * @brief 小车控制任务头文件
 * @note 声明小车控制任务的相关函数和变量
 */

#ifndef __CAR_TASK_H
#define __CAR_TASK_H

#include "hsc.h"

// 函数声明

/**
 * @brief 小车任务初始化
 * @retval 无
 */
void Car_Task_Init(void);

/**
 * @brief 小车任务开始
 * @retval 无
 */
void Car_Task_Start(void);

/**
 * @brief 小车任务停止
 * @retval 无
 */
void Car_Task_Stop(void);

/**
 * @brief 小车任务主函数
 * @retval 无
 */
void Car_Task_Main(void);

/**
 * @brief 获取小车任务运行状态
 * @retval 运行状态：1-运行中，0-停止
 */
uint8_t Car_Task_IsRunning(void);

/**
 * @brief 设置小车目标角度
 * @param angle 目标角度（度）
 * @retval 无
 */
void Car_Task_SetTargetAngle(float angle);

/**
 * @brief 设置小车最大速度
 * @param speed 最大速度（-300到300）
 * @retval 无
 */
void Car_Task_SetMaxSpeed(int16_t speed);

#endif /* __CAR_TASK_H */
