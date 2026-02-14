/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWM_standard.c
  * @brief   标准PWM模式电机驱动实现
  *          支持标准航模PWM信号：50Hz，1-2ms脉宽
  *          只控制后电机（B1, B0, A7, A6）
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "PWM_standard.h"
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  标准PWM模式电机驱动初始化
  * @param  None
  * @retval None
  * @note   需要配置定时器为50Hz，计数周期20000
  *         假设TIM3已经配置为PWM模式，频率50Hz
  */
void PWM_Standard_Init(void)
{
  /* 启动PWM定时器 - 只控制后电机 */
  // TIM3用于后电机PWM输出
  // A6-A7: 后左电机 (CH1-CH2)
  // B0-B1: 后右电机 (CH3-CH4)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // BACK_LEFT_A (PA6)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);  // BACK_LEFT_B (PA7)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);  // BACK_RIGHT_A (PB0)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);  // BACK_RIGHT_B (PB1)

  /* 初始化为中位（停止） */
  PWM_Standard_StopAll();
}

/**
  * @brief  设置标准PWM模式电机速度
  * @param  motor: 电机编号 (PWM_MOTOR_BACK_LEFT, PWM_MOTOR_BACK_RIGHT)
  * @param  speed: 速度值 (-1000 到 1000)
  *                -1000 = 1ms (最小/后退)
  *                   0  = 1.5ms (中位/停止)
  *                +1000 = 2ms (最大/前进)
  * @retval None
  */
void PWM_Standard_SetSpeed(uint8_t motor, int16_t speed)
{
  uint16_t pwm_value = PWM_SERVO_MID;  // 默认中位

  /* 限制速度范围 */
  if (speed > 1000) {
    speed = 1000;
  } else if (speed < -1000) {
    speed = -1000;
  }

  /* 将速度值(-1000到1000)映射到PWM脉宽(1000到2000) */
  /* speed = -1000 -> pwm = 1000 (1ms, 最小)
     speed = 0     -> pwm = 1500 (1.5ms, 中位)
     speed = 1000  -> pwm = 2000 (2ms, 最大) */
  pwm_value = PWM_SERVO_MID + (speed * 500 / 1000);

  /* 根据电机编号设置PWM */
  switch (motor) {
    case PWM_MOTOR_BACK_LEFT:  // 后电机左 (A6-A7)
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);  // A6
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm_value);  // A7
      break;

    case PWM_MOTOR_BACK_RIGHT:  // 后电机右 (B0-B1)
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwm_value);  // B0
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm_value);  // B1
      break;

    default:
      // 无效电机编号，不做处理
      break;
  }
}

/**
  * @brief  停止所有电机（设置为中位1.5ms）
  * @param  None
  * @retval None
  */
void PWM_Standard_StopAll(void)
{
  // 设置所有PWM通道为中位（1.5ms）
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_SERVO_MID);  // BACK_LEFT_A (A6)
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, PWM_SERVO_MID);  // BACK_LEFT_B (A7)
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, PWM_SERVO_MID);  // BACK_RIGHT_A (B0)
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, PWM_SERVO_MID);  // BACK_RIGHT_B (B1)
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
