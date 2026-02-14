#ifndef _GRYO_H
#define _GRYO_H

#include "car-data.h"


extern u8 Gryo_start;//陀螺仪启动状态

extern float car_ang;  //车的角度（初始90度，向前）

extern float car_ax; //车的加速度计
extern float car_ay;
extern float car_az;

//陀螺仪初始化 （带零飘）
void Gryo_Init(void);
void Gryo_z0_cal(void);//独立的计算零飘

//不带滤波的陀螺仪数据
void Get_Ang(void);

#endif