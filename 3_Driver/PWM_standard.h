/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWM_standard.h
  * @brief   标准PWM模式电机驱动头文件
  *          支持标准航模PWM信号：50Hz，1-2ms脉宽
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __PWM_STANDARD_H__
#define __PWM_STANDARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "main.h"

/* 电机编号定义 */
#define PWM_MOTOR_BACK_LEFT      0  // 后电机左 (A6-A7)
#define PWM_MOTOR_BACK_RIGHT     1  // 后电机右 (B0-B1)

/* PWM参数定义（假设系统时钟配置为1MHz计数频率） */
/* 50Hz = 20ms周期，计数器周期 = 20000 */
#define PWM_SERVO_PERIOD           20000  // 20ms对应的计数值
#define PWM_SERVO_MIN              1000   // 1ms对应的计数值
#define PWM_SERVO_MAX              2000   // 2ms对应的计数值
#define PWM_SERVO_MID              1500   // 1.5ms中位对应的计数值

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  标准PWM模式电机驱动初始化
  * @param  None
  * @retval None
  * @note   需要配置定时器为50Hz，计数周期20000
  */
void PWM_Standard_Init(void);

/**
  * @brief  设置标准PWM模式电机速度
  * @param  motor: 电机编号 (PWM_MOTOR_BACK_LEFT, PWM_MOTOR_BACK_RIGHT)
  * @param  speed: 速度值 (-1000 到 1000)
  *                -1000 = 1ms (最小/后退)
  *                   0  = 1.5ms (中位/停止)
  *                +1000 = 2ms (最大/前进)
  * @retval None
  */
void PWM_Standard_SetSpeed(uint8_t motor, int16_t speed);

/**
  * @brief  停止所有电机（设置为中位1.5ms）
  * @param  None
  * @retval None
  */
void PWM_Standard_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_STANDARD_H__ */
