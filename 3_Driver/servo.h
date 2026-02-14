#ifndef _SERVO_H
#define _SERVO_H
#include "car-data.h"

//===================引脚配置================

//设置舵机的通道			（会对应引脚）
#define Servo1_pwm PWM4_MODULE3_CHA_C31
#define Servo2_pwm PWM4_MODULE2_CHA_C30
#define Servo3_pwm PWM2_MODULE2_CHB_C11
#define Servo4_pwm PWM2_MODULE0_CHA_C6

//设置电磁铁的引脚
#define mag_pin			(B16)

//设置电磁铁的引脚
#define lock1_pin			(B17)
#define lock2_pin			(B14)
#define lock3_pin			(B15)
#define lock4_pin			(B21)
//#define lock5_pin			(B16)   //还没装

//==================可用函数==============

//舵机初始化
void Servo_Init(void);

//舵机旋转角度
void Servo_Turn(u8 num,int angle);
	
//电磁铁打开 打开:1 关闭:0
void Mag_Set(u8 open);

#endif