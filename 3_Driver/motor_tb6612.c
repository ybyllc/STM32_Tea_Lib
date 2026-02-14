/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    motor_tb6612.c
  * @brief   TB6612电机驱动实现文件
  *          使用HAL库操作GPIO和PWM
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
  * 真值表
    IN1	IN2	PWM	STBY	输出状态
    H	H	H/L	H	制动
    L	H	H	H	反转
    L	H	L	H	制动
    H	L	H	H	正转
    H	L	L	H	制动
    L	L	H	H	停止
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "motor_tb6612.h"
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 启用GPIO时钟 */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 配置前电机方向控制引脚 */
  // FRONT_A_IN1 (C5)
  GPIO_InitStruct.Pin = FRONT_A_IN1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FRONT_A_IN1_PORT, &GPIO_InitStruct);

  // FRONT_A_IN2 (C7)
  GPIO_InitStruct.Pin = FRONT_A_IN2_PIN;
  HAL_GPIO_Init(FRONT_A_IN2_PORT, &GPIO_InitStruct);

  // FRONT_B_IN1 (C4)
  GPIO_InitStruct.Pin = FRONT_B_IN1_PIN;
  HAL_GPIO_Init(FRONT_B_IN1_PORT, &GPIO_InitStruct);

  // FRONT_B_IN2 (B8)
  GPIO_InitStruct.Pin = FRONT_B_IN2_PIN;
  HAL_GPIO_Init(FRONT_B_IN2_PORT, &GPIO_InitStruct);

  /* 配置后电机方向控制引脚 */
  // BACK_C_IN1 (A5)
  GPIO_InitStruct.Pin = BACK_C_IN1_PIN;
  HAL_GPIO_Init(BACK_C_IN1_PORT, &GPIO_InitStruct);

  // BACK_C_IN2 (A7)
  GPIO_InitStruct.Pin = BACK_C_IN2_PIN;
  HAL_GPIO_Init(BACK_C_IN2_PORT, &GPIO_InitStruct);

  // BACK_D_IN1 (A4)
  GPIO_InitStruct.Pin = BACK_D_IN1_PIN;
  HAL_GPIO_Init(BACK_D_IN1_PORT, &GPIO_InitStruct);

  // BACK_D_IN2 (B0)
  GPIO_InitStruct.Pin = BACK_D_IN2_PIN;
  HAL_GPIO_Init(BACK_D_IN2_PORT, &GPIO_InitStruct);

  /* 初始化方向引脚为默认状态（停止） */
  HAL_GPIO_WritePin(FRONT_A_IN1_PORT, FRONT_A_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_A_IN2_PORT, FRONT_A_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_B_IN1_PORT, FRONT_B_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_B_IN2_PORT, FRONT_B_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_C_IN1_PORT, BACK_C_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_C_IN2_PORT, BACK_C_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_D_IN1_PORT, BACK_D_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_D_IN2_PORT, BACK_D_IN2_PIN, GPIO_PIN_RESET);

  /* 启动PWM定时器 */
  // TIM4用于前电机PWM输出
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);  // FRONT_A_PWM1 (PB6)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);  // FRONT_B_PWM2 (PB9)

  // TIM3用于后电机PWM输出
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // BACK_C_PWM1 (PA6)
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
  */
void Motor_SetSpeed(uint8_t motor, int16_t speed)
{
  uint16_t pwm_value = 0;
  uint8_t direction = MOTOR_DIR_FORWARD;

  /* 限制速度范围 */
  if (speed > 1000) {
    speed = 1000;
  } else if (speed < -1000) {
    speed = -1000;
  }

  /* 计算PWM值和方向 */
  if (speed < 0) {
    pwm_value = -speed;
    direction = MOTOR_DIR_BACKWARD;
  } else {
    pwm_value = speed;
    direction = MOTOR_DIR_FORWARD;
  }

  /* 根据电机编号设置方向和PWM */
  switch (motor) {
    case MOTOR_FRONT_LEFT:  // 前电机左（A）
      if (pwm_value > 0) {
        // 有速度：正转或反转
        if (direction == MOTOR_DIR_FORWARD) {
          // 正转：IN1=H, IN2=L
          HAL_GPIO_WritePin(FRONT_A_IN1_PORT, FRONT_A_IN1_PIN, GPIO_PIN_SET);
          HAL_GPIO_WritePin(FRONT_A_IN2_PORT, FRONT_A_IN2_PIN, GPIO_PIN_RESET);
        } else {
          // 反转：IN1=L, IN2=H
          HAL_GPIO_WritePin(FRONT_A_IN1_PORT, FRONT_A_IN1_PIN, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(FRONT_A_IN2_PORT, FRONT_A_IN2_PIN, GPIO_PIN_SET);
        }
        // 设置PWM
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
      } else {
        // 速度为0：停止
        HAL_GPIO_WritePin(FRONT_A_IN1_PORT, FRONT_A_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(FRONT_A_IN2_PORT, FRONT_A_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
      }
      break;

    case MOTOR_FRONT_RIGHT:  // 前电机右（B）
      if (pwm_value > 0) {
        // 有速度：正转或反转
        if (direction == MOTOR_DIR_FORWARD) {
          // 正转：IN1=H, IN2=L
          HAL_GPIO_WritePin(FRONT_B_IN1_PORT, FRONT_B_IN1_PIN, GPIO_PIN_SET);
          HAL_GPIO_WritePin(FRONT_B_IN2_PORT, FRONT_B_IN2_PIN, GPIO_PIN_RESET);
        } else {
          // 反转：IN1=L, IN2=H
          HAL_GPIO_WritePin(FRONT_B_IN1_PORT, FRONT_B_IN1_PIN, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(FRONT_B_IN2_PORT, FRONT_B_IN2_PIN, GPIO_PIN_SET);
        }
        // 设置PWM
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pwm_value);
      } else {
        // 速度为0：停止
        HAL_GPIO_WritePin(FRONT_B_IN1_PORT, FRONT_B_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(FRONT_B_IN2_PORT, FRONT_B_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
      }
      break;

    case MOTOR_BACK_LEFT:  // 后电机左（C）- 交换为后右（D）
      if (pwm_value > 0) {
        // 有速度：正转或反转
        if (direction == MOTOR_DIR_FORWARD) {
          // 正转：IN1=H, IN2=L
          HAL_GPIO_WritePin(BACK_D_IN1_PORT, BACK_D_IN1_PIN, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BACK_D_IN2_PORT, BACK_D_IN2_PIN, GPIO_PIN_RESET);
        } else {
          // 反转：IN1=L, IN2=H
          HAL_GPIO_WritePin(BACK_D_IN1_PORT, BACK_D_IN1_PIN, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(BACK_D_IN2_PORT, BACK_D_IN2_PIN, GPIO_PIN_SET);
        }
        // 设置PWM
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm_value);
      } else {
        // 速度为0：停止
        HAL_GPIO_WritePin(BACK_D_IN1_PORT, BACK_D_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BACK_D_IN2_PORT, BACK_D_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
      }
      break;

    case MOTOR_BACK_RIGHT:  // 后电机右（D）- 交换为后左（C）
      if (pwm_value > 0) {
        // 有速度：正转或反转
        if (direction == MOTOR_DIR_FORWARD) {
          // 正转：IN1=H, IN2=L
          HAL_GPIO_WritePin(BACK_C_IN1_PORT, BACK_C_IN1_PIN, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BACK_C_IN2_PORT, BACK_C_IN2_PIN, GPIO_PIN_RESET);
        } else {
          // 反转：IN1=L, IN2=H
          HAL_GPIO_WritePin(BACK_C_IN1_PORT, BACK_C_IN1_PIN, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(BACK_C_IN2_PORT, BACK_C_IN2_PIN, GPIO_PIN_SET);
        }
        // 设置PWM
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);
      } else {
        // 速度为0：停止
        HAL_GPIO_WritePin(BACK_C_IN1_PORT, BACK_C_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BACK_C_IN2_PORT, BACK_C_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
      }
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
  // 设置所有方向引脚为低电平（停止状态）
  HAL_GPIO_WritePin(FRONT_A_IN1_PORT, FRONT_A_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_A_IN2_PORT, FRONT_A_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_B_IN1_PORT, FRONT_B_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(FRONT_B_IN2_PORT, FRONT_B_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_C_IN1_PORT, BACK_C_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_C_IN2_PORT, BACK_C_IN2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_D_IN1_PORT, BACK_D_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BACK_D_IN2_PORT, BACK_D_IN2_PIN, GPIO_PIN_RESET);

  // 设置所有PWM通道为0
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);  // FRONT_A_PWM1
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);  // FRONT_B_PWM2

  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // BACK_C_PWM1
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);  // BACK_D_PWM2
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
