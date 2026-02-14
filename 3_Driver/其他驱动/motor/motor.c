/**
  ******************************************************************************
  * @file		电机文件
  * @author		一杯原谅绿茶，胡书畅写的 qq：420752002
  * @version		V1.0.0
  * @date		2022年4月18日
  * @brief 功能：集成电机需要用到PWM例程的函数
  *         适合电机接口为1脚PWM输入，1脚控制正反转的驱动
  * 我在造轮子
  *****************************************************************************/
    
 /* Includes ------------------------------------------------------------------*/
#include "motor.h"
#include "stdint.h"	//定义变量类型的头文件
#include "LQ_PWM.h"	
#include "LQ_GPIO.h"

/*-----------------------------------------------------------------------------------------  
电机引脚
PWM1, kPWM_Module_3, kPWM_PwmA   L5     电机1接口
PWM1, kPWM_Module_3, kPWM_PwmB   M5     电机1接口（正/反）
PWM2, kPWM_Module_0, kPWM_PwmA   A8     电机2接口
PWM2, kPWM_Module_0, kPWM_PwmB   A9     电机2接口（正/反）
PWM2, kPWM_Module_1, kPWM_PwmA   B9     电机3接口
PWM2, kPWM_Module_1, kPWM_PwmB   C9     电机3接口（正/反）
PWM2, kPWM_Module_2, kPWM_PwmB   A10    电机4接口
PWM1, kPWM_Module_1, kPWM_PwmA   J1     电机4接口（正/反）
------------------------------------------------------------------------------------------*/

//把例程PWM那套搬过来
/**
  * @brief    电机（PWM接口）初始化
  *
  * @param 参数  
  *
  * @return 返回值   
  *
  * @note 说明 
  */
void Motor_Init(void) //是否要把参数改成自定义PWM上限？
{    
  /*初始化电机正/反转接口   */
    LQ_PinInit(M5, PIN_MODE_OUTPUT, 0); 
    //是个好函数，直接用普通的引脚号对应了板上的io口号
    LQ_PinInit(A9, PIN_MODE_OUTPUT, 0); 
    LQ_PinInit(C9, PIN_MODE_OUTPUT, 0); 
    LQ_PinInit(J1, PIN_MODE_OUTPUT, 0); 


  /*初始化电机PWM接口   */
    //电机1(第几组PWM，编号，A或B相，PWM上限)
    LQ_PWM_Init(PWM1, kPWM_Module_3, kPWM_PwmA, 17000);// L5
    LQ_PWM_Init(PWM2, kPWM_Module_0, kPWM_PwmA, 17000);  // A8
    LQ_PWM_Init(PWM2, kPWM_Module_1, kPWM_PwmA, 17000); // B9
    LQ_PWM_Init(PWM2, kPWM_Module_2, kPWM_PwmB, 17000);  // A10

    //显示屏显示“motor_init succeeded...”
    //TFTSPI_P8X16Str(3,0,"LQ Motor PWM",u16RED,u16BLUE);
    //串口发送“motor_init succeeded”
    
}


/**
  * @brief   设置电机功率（PWM占空比）
  *
  * @param 参数  num：电机编号（从1开始），PWM：PWM值
  *
  * @note 说明  电机正反转通过PWM的正负来调整
  * 电机1： L5 正/反M5   电机2：A8 正/反A9  电机3：B9 正/反C9  电机4：A10 正/反J1
  */
void Motor_Set(u8 num, short PWM)  
{
    assert(num < 5);  //限制num编号小于5
    
    switch(num)
    {
        case 1:
            if(PWM >= 0)  
            {
              LQ_PinWrite(M5, 1); //电机1正转;
              LQ_PWM_SetDuty(PWM1, kPWM_Module_3, kPWM_PwmA, PWM);
            }
            else
            {
              LQ_PinWrite(M5, 0);
              LQ_PWM_SetDuty(PWM1, kPWM_Module_3, kPWM_PwmA, -PWM);
            }
            break;
            
        case 2:
            if(PWM >= 0)  
            {
              LQ_PinWrite(A9, 1); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmA, PWM);
            }
            else
            {
              LQ_PinWrite(A9, 0); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmA, -PWM);
            }
            break;
            
        case 3:
            if(PWM >= 0)  
            {
              LQ_PinWrite(C9, 1); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmA, PWM);
            }
            else
            {
              LQ_PinWrite(C9, 0); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmA, -PWM);
            }
            break;
            
        case 4:
            if(PWM >= 0)  
            {
              LQ_PinWrite(J1, 1); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_2, kPWM_PwmB, PWM);
            }
            else
            {
              LQ_PinWrite(J1, 0); 
              LQ_PWM_SetDuty(PWM2, kPWM_Module_2, kPWM_PwmB, -PWM);
            }
            break;
    }
  
}


/* 麦轮四向移动的驱动程序*/
//左右量，前后量，旋转量
void move(float LR,float FB,float turn)
{
  u8 move_sum;
  float k;
  //移动量总和
  move_sum=abs(LR)+abs(FB)+abs(turn);
  if (move_sum > 100)
  {
    k = 100/move_sum;//当速度和过大时
    LR *= k;
    FB *= k;
    turn *= k;
  }
  Motor_Set(1, FB+LR* 1 +turn);//左前驱动
  Motor_Set(2, FB+LR*-1 +turn);//左后
  Motor_Set(3, FB+LR*-1 +turn*-1 );//右前
  Motor_Set(4, FB+LR* 1 +turn*-1 );//右后
}
