/**
 * @file encoder_knob.h
 * @brief 旋转编码器(EC11)驱动头文件
 */

#ifndef __ENCODER_KNOB_H
#define __ENCODER_KNOB_H

#include "common.h"

void Ec11_knob_Init(void);

/**
 * @brief 编码器定时器回调 - 在 TIM1_100us_Callback 中调用
 * @note 内部每10次(1ms)执行一次扫描
 */
void Ec11_knob_TIM_Callback(void);

// 以下函数由 Ec11_knob_TIM_Callback 内部调用，一般不需要直接调用
void Ec11_knob_Scan(void);
void Ec11_knob_Key_Scan(void);

int32_t Ec11_knob_Get_Count(void);
void Ec11_knob_Set_Count(int32_t count);
void Ec11_knob_Clear_Count(void);

uint8_t Ec11_knob_Key_GetEvent(void);
uint8_t Ec11_knob_Key_GetState(void);

#endif /* __ENCODER_KNOB_H */
