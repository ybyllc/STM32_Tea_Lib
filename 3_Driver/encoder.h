#ifndef _ENCODER_H
#define _ENCODER_H

#include "tim.h"

// 编码器编号定义
#define ENCODER_1       1
#define ENCODER_2       2

// 编码器定时器定义
#define ENCODER1_TIM    &htim5
#define ENCODER2_TIM    &htim2

// 编码器初始化
void Encoder_Init(void);

// 获取编码器计数
// encoder_num: 编码器编号 (ENCODER_1 或 ENCODER_2)
// 返回值: 编码器计数值
int32_t Encoder_GetCount(uint8_t encoder_num);

#endif /* _ENCODER_H */