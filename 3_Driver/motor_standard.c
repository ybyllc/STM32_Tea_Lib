/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    motor_standard.c
  * @brief   标准电机驱动实现文件
  *          使用PWM占空比控制速度
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "motor_standard.h"
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  电机驱动初始化
  * @param  None
  * @retval None
  */
void Motor_Init(void)
{
  /* 启动PWM定时器 */
  // TIM4用于前电机PWM输出
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);  // FRONT_A_PWM1 (PB6)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);  // FRONT_A_PWM2 (PB7)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);  // FRONT_B_PWM1 (PB8)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);  // FRONT_B_PWM2 (PB9)

  // TIM3用于后电机PWM输出
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // BACK_C_PWM1 (PA6)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);  // BACK_C_PWM2 (PA7)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);  // BACK_D_PWM1 (PB0)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);  // BACK_D_PWM2 (PB1)

  /* 初始化为0速度 */
  Motor_SetSpeed(MOTOR_FRONT_LEFT, 0);
  Motor_SetSpeed(MOTOR_FRONT_RIGHT, 0);
  Motor_SetSpeed(MOTOR_BACK_LEFT, 0);
  Motor_SetSpeed(MOTOR_BACK_RIGHT, 0);
}

/**
  * @brief  设置电机速度
  * @param  motor: 电机编号 (MOTOR_FRONT_LEFT, MOTOR_FRONT_RIGHT, MOTOR_BACK_LEFT, MOTOR_BACK_RIGHT)
  * @param  speed: 速度值 (-1000 到 1000)，负值表示反向
  * @retval None
  * @note   标准版驱动使用PWM占空比控制速度，假设外部电路已处理方向控制
  */
void Motor_SetSpeed(uint8_t motor, int16_t speed)
{
  uint16_t pwm_value = 0;

  /* 限制速度范围 */
  if (speed > 1000) {
    speed = 1000;
  } else if (speed < -1000) {
    speed = -1000;
  }

  /* 计算PWM值（取绝对值，因为方向由外部电路处理） */
  pwm_value = (speed < 0) ? -speed : speed;

  /* 根据电机编号设置PWM */
  switch (motor) {
    case MOTOR_FRONT_LEFT:  // 前电机左（A）
      // 设置PWM1为速度，PWM2为0（假设PWM2用于方向控制）
      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0);
      break;

    case MOTOR_FRONT_RIGHT:  // 前电机右（B）
      // 设置PWM2为速度，PWM1为0（假设PWM1用于方向控制）
      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pwm_value);
      break;

    case MOTOR_BACK_LEFT:  // 后电机左（C）- 交换为后右（D）
      // 设置PWM2为速度，PWM1为0（假设PWM1用于方向控制）
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm_value);
      break;

    case MOTOR_BACK_RIGHT:  // 后电机右（D）- 交换为后左（C）
      // 设置PWM1为速度，PWM2为0（假设PWM2用于方向控制）
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
      break;

    default:
      // 无效电机编号，不做处理
      break;
  }
}

/**
  * @brief  停止所有电机
  * @param  None
  * @retval None
  */
void Motor_StopAll(void)
{
  // 设置所有PWM通道为0
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);  // FRONT_A_PWM1
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0);  // FRONT_A_PWM2
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);  // FRONT_B_PWM1
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);  // FRONT_B_PWM2

  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // BACK_C_PWM1
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);  // BACK_C_PWM2
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);  // BACK_D_PWM1
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);  // BACK_D_PWM2
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
