#include "stdint.h"	//定义变量类型的头文件


/**
  * @brief    电机（PWM接口）初始化
  *
  * @note 说明: 电机1： L5 正/反M5   电机2：A8 正/反A9  电机3：B9 正/反C9  电机4：A10 正/反J1
  */
void Motor_Init(void); //是否要把参数改成自定义PWM上限？

/**
  * @brief   设置电机功率（PWM占空比）
  *
  * @param 参数  num：电机编号，PWM：PWM值
  *
  * @note 说明  电机正反转通过PWM的正负来调整
  * 电机1： L5 正/反M5   电机2：A8 正/反A9  电机3：B9 正/反C9  电机4：A10 正/反J1
  */
void Motor_Set(u8 num, short PWM);
