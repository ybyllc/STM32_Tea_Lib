#ifndef _MOTOR_M_H
#define _MOTOR_M_H
#include "car-data.h"

//PWM_DUTY_MAX   PWM最大值 10000

//==============引脚定义===========

//***PWM引脚
#define LF1_PWM(n)			(TIM3->CCR4=n)	//PB0左前轮
#define LF2_PWM(n)			(TIM3->CCR3=n)	//PB1左前后退

#define RF1_PWM(n)		(TIM3->CCR1=n)		//PA6 
#define RF2_PWM(n)		(TIM3->CCR2=n)	//PA7

#define LB1_PWM(n)		(TIM4->CCR2=n)	//PB7
#define LB2_PWM(n)		 (TIM4->CCR1=n)		//PB6

#define RB1_PWM(n)		 (TIM4->CCR3=n)	//PB8
#define RB2_PWM(n)		  (TIM4->CCR4=n)	//PB9

//================可用函数==========

//电机初始化
void Motor_Init(void);

//设置每个电机的功率 
void Motor_Set(short pwm1,short pwm2,short pwm3,short pwm4);

#endif
