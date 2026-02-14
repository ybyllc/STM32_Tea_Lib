#include "servo.h"
#include "car-data.h"

//初始化舵机
void Servo_Init()
{
	//舵机PWM 引脚，频率，默认占空比                                                          
//     pwm_init(Servo1_pwm, 330, 0);          //1	     车身的舵机(180)   限制110-140°
//     pwm_init(Servo2_pwm, 330, 0);          //2      电磁铁的舵机(180) 限制20-135°                                      
//     pwm_init(Servo3_pwm, 330, 0);          //3      储物盒的舵机(360)
// //  pwm_init(Servo4_pwm ,50, 250);           //4
//	
//	gpio_init(mag_pin  , GPO, 0, GPO_PUSH_PULL);	    //5路电磁铁模块初始化
//    gpio_init(lock1_pin, GPO, 0, GPO_PUSH_PULL);	
//	gpio_init(lock2_pin, GPO, 0, GPO_PUSH_PULL);	
//	gpio_init(lock3_pin, GPO, 0, GPO_PUSH_PULL);	
//	gpio_init(lock4_pin, GPO, 0, GPO_PUSH_PULL);	
	//gpio_init(lock5_pin, GPO, 0, GPO_PUSH_PULL);	
	
}

//舵机旋转角度  （编号，角度）
void Servo_Turn(u8 num,int angle)
{
	//(2000*(angle/180)+500)/(1000000/330) *10000 // 持续时间(500us-2500us) ÷ 每次PWM总时间*占空比
	//角度范围0°-180°  PWM信号范围500-2500
	u16 servo_duty = (int)( (float)(2000.0*((float)angle/180.0)+500)/(1000000.0/330.0)*10000.0);//计算舵机的占空比
	
	switch(num)
	{
//		case 1:pwm_set_duty(Servo1_pwm, servo_duty);break;
//		case 2:pwm_set_duty(Servo2_pwm, servo_duty);break;	
//		case 3:pwm_set_duty(Servo3_pwm, servo_duty);break;	
//		case 4:pwm_set_duty(Servo4_pwm, servo_duty);break;	
	
	}
	
}


//抓取电磁铁打开  输入1
void Mag_Set(u8 open)
{
	if(open==1) 
		gpio_high(mag_pin);
	else 
		gpio_low(mag_pin);
}