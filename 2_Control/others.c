#include "others.h"
//备用操作

void Classify(u8 s)    //4分类
{
		switch(s)
	{
		case 1:Servo_Turn(3,0);break;     //10  15  贴合地面 单位°
		        
		case 2:Servo_Turn(3,90);break;	  //50  190

		case 3:Servo_Turn(3,180);break;	 
		
		case 4:Servo_Turn(3,270);break;	 
		
		default:break;
	}

}


void Classify5(u8 s)    //5分类
{
		switch(s%4+1)
	{
		case 1:Servo_Turn(3,0);break;     //10  15  贴合地面 单位°
		        
		case 2:Servo_Turn(3,90);break;	  //50  190

		case 3:Servo_Turn(3,180);break;	 
		
		case 4:Servo_Turn(3,270);break;	 
		
		default:break;
	}

}


void Classify6(u8 s)    //六分类
{
		switch(s%4+1)
	{
		case 1:Servo_Turn(3,0);break;     //10  15  贴合地面 单位°
		        
		case 2:Servo_Turn(3,90);break;	  //50  190

		case 3:Servo_Turn(3,180);break;	 
		
		case 4:Servo_Turn(3,270);break;	 
		default:break;
	}


}