#include "box.h"
//盒子初始化
void Box_Init(void)
{
    //舵机初始化
	//机械臂位置 0°收 180°放 
	Servo_Turn(1,100);	//1	     车身的舵机(180)   限制110-140°
	Servo_Turn(2,0);	//2      电磁铁的舵机(180) 限制20-135° 
	//盒子位置
	Servo_Turn(3,10);	//3      储物盒的舵机(360)
    //关闭门

}

//电磁铁拾取
void Box_pick(u8 n)
{
	//盒子转到指定方向
	Box_set(n);
	
	//放下
	Servo_Turn(2,180);//小臂
	delayms(50);
	//缓慢放下大臂
	for(int i=100;i<145;i++)
	{
		
		Servo_Turn(1,i);
		delayms(8);
	}
	//打开磁吸
	Mag_Set(1);
	delayms(1000);

		
	//Servo_Turn(1,95); //后面的盒子
	Servo_Turn(1,82);
	delayms(200);
	//Servo_Turn(2,0);
	Servo_Turn(2,20);
	
	delayms(1000);
	Mag_Set(0);
		
		
	delayms(500);
	Servo_Turn(1,100);
	Servo_Turn(2,0);
}


//盒子转到第N种图片的位置
void Box_set(u8 n)
{
    //舵机旋转位置
    switch(n)
    {
        case 1:Servo_Turn(3,10); break;//一分类
        case 2:Servo_Turn(3,180/5*1+10); break;
		case 3:Servo_Turn(3,180/5*2+10); break;
		case 4:Servo_Turn(3,180/5*3+10); break;
		case 5:Servo_Turn(3,180/5*4+10); break;
		case 6:Servo_Turn(3,180/5*4+10); break;//车上的图片

        default:break;
    }

}

//盒子的门打开
void Box_open(u8 n)
{
	//舵机旋转位置
    switch(n)
    {
        case 1:Servo_Turn(3,10); break;//一分类
        case 2:Servo_Turn(3,180/5*1+10+45); break;
		case 3:Servo_Turn(3,180/5*2+10+45); break;
		case 4:Servo_Turn(3,180/5*3+10+45); break;
		case 5:Servo_Turn(3,(180/5*4+10+45)-180); break;

        default:break;
    }
	delayms(1000);
    //关闭磁力
    switch(n)
    {
        case 1: gpio_high(lock1_pin); break;//一分类
        case 2: gpio_high(lock2_pin); break;
		case 3: gpio_high(lock3_pin); break;
		case 4: gpio_high(lock4_pin); break;
		//case 5: gpio_high(lock5_pin); break;

        default:break;
    }
	delayms(500);
}