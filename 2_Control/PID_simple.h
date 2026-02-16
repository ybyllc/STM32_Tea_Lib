/**
 ******************************************************************************
 * @file    PID_simple.h
 * @brief   最简单的PID算法头文件
 ******************************************************************************
 */

#ifndef __PID_SIMPLE_H__
#define __PID_SIMPLE_H__

/**
 * @brief   最简单的PID计算函数，直接调用
 */
float PID_1(float err);

// 要重复使用，可以多定义几个
// float PID_2(float err);
// float PID_3(float err);
// float PID_4(float err);

#endif
