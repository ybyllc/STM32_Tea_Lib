/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    motor_standard.h
  * @brief   标准电机驱动头文件
  *          支持前电机和后电机的控制
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_STANDARD_H__
#define __MOTOR_STANDARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* 电机编号定义 */
#define MOTOR_FRONT_LEFT     0  // 前电机左（A）
#define MOTOR_FRONT_RIGHT    1  // 前电机右（B）
#define MOTOR_BACK_LEFT      2  // 后电机左（C）
#define MOTOR_BACK_RIGHT     3  // 后电机右（D）

/* 前电机引脚定义 */
#define FRONT_A_PWM1_PIN     GPIO_PIN_6
#define FRONT_A_PWM1_PORT    GPIOB
#define FRONT_A_PWM2_PIN     GPIO_PIN_7
#define FRONT_A_PWM2_PORT    GPIOB
#define FRONT_B_PWM1_PIN     GPIO_PIN_8
#define FRONT_B_PWM1_PORT    GPIOB
#define FRONT_B_PWM2_PIN     GPIO_PIN_9
#define FRONT_B_PWM2_PORT    GPIOB

/* 后电机引脚定义 */
#define BACK_C_PWM1_PIN      GPIO_PIN_6
#define BACK_C_PWM1_PORT     GPIOA
#define BACK_C_PWM2_PIN      GPIO_PIN_7
#define BACK_C_PWM2_PORT     GPIOA
#define BACK_D_PWM1_PIN      GPIO_PIN_0
#define BACK_D_PWM1_PORT     GPIOB
#define BACK_D_PWM2_PIN      GPIO_PIN_1
#define BACK_D_PWM2_PORT     GPIOB

/* USER CODE END Private defines */

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  电机驱动初始化
  * @param  None
  * @retval None
  */
void Motor_Init(void);

/**
  * @brief  设置电机速度
  * @param  motor: 电机编号 (MOTOR_FRONT_LEFT, MOTOR_FRONT_RIGHT, MOTOR_BACK_LEFT, MOTOR_BACK_RIGHT)
  * @param  speed: 速度值 (-1000 到 1000)，负值表示反向
  * @retval None
  */
void Motor_SetSpeed(uint8_t motor, int16_t speed);

/**
  * @brief  停止所有电机
  * @param  None
  * @retval None
  */
void Motor_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_STANDARD_H__ */
