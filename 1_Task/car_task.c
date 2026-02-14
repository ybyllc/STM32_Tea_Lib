/**
 * @file car_task.c
 * @brief 小车控制任务实现
 * @note 实现电机控制和陀螺仪角度保持功能，让陀螺仪z轴保持在30度来笔直前进
 */

#include "car_task.h"
#include "ICM42670.h"
#include "motor_tb6612.h"
#include "wit_gyro_sdk.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"

// 陀螺仪类型枚举
typedef enum {
    GYRO_TYPE_NONE,        // 无陀螺仪
    GYRO_TYPE_WIT,         // 原来的陀螺仪
    GYRO_TYPE_ICM42670     // ICM42670 陀螺仪
} GyroType;

// 全局变量
static uint8_t car_task_running = 0;  // 任务运行状态
static float target_angle = 30.0f;    // 目标角度（度）
static int16_t max_speed = 300;       // 最大电机速度
static GyroType current_gyro_type = GYRO_TYPE_NONE; // 当前使用的陀螺仪类型

// PID控制参数
static float kp = 1.0f;  // 比例系数
static float ki = 0.0f;  // 积分系数
static float kd = 0.1f;  // 微分系数

// PID变量
static float error = 0.0f;      // 当前误差
static float last_error = 0.0f; // 上一次误差
static float integral = 0.0f;   // 积分项
static float derivative = 0.0f; // 微分项
static float pid_output = 0.0f; // PID输出

// 电压补偿相关变量
static ADC_HandleTypeDef hadc1;           // ADC句柄
static uint8_t adc_initialized = 0;       // ADC初始化标志
static float current_voltage = 0.0f;      // 当前电压值
static float voltage_compensation = 1.0f; // 电压补偿系数
#define REFERENCE_VOLTAGE 8.4f            // 参考电压（电池满电电压）
#define ADC_DIVIDER_RATIO 4.0f            // 分压比（ADC读取值是实际电压的1/4）

// 调试功能：模拟电压（0=使用实际ADC，5.0=模拟5V，8.4=模拟8.4V）
#define DEBUG_SIMULATE_VOLTAGE 8.4f

/**
 * @brief 初始化ADC用于电压检测
 * @retval 0-失败, 1-成功
 */
static uint8_t Car_Task_InitADC(void)
{
    if (adc_initialized) {
        return 1;
    }
    
    // 启用GPIOC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // 启用ADC1时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    // 配置PC4引脚为ADC输入
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = GPIO_PIN_4,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_NOPULL
    };
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // 配置ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        return 0;
    }
    
    // 配置ADC通道
    ADC_ChannelConfTypeDef sConfig = {
        .Channel = ADC_CHANNEL_14,
        .Rank = 1,
        .SamplingTime = ADC_SAMPLETIME_480CYCLES
    };
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return 0;
    }
    
    adc_initialized = 1;
    return 1;
}

/**
 * @brief 读取电池电压
 * @retval 电池电压值（V）
 */
static float Car_Task_ReadVoltage(void)
{
    // 调试模式：使用模拟电压
    if (DEBUG_SIMULATE_VOLTAGE > 0.0f) {
        return DEBUG_SIMULATE_VOLTAGE;
    }
    
    if (!adc_initialized) {
        return 0.0f;
    }
    
    // 启动ADC转换
    HAL_ADC_Start(&hadc1);
    
    // 等待转换完成
    HAL_ADC_PollForConversion(&hadc1, 100);
    
    // 读取ADC值
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    
    // 停止ADC
    HAL_ADC_Stop(&hadc1);
    
    // 计算实际电压：ADC值 * 3.3V / 4095 * 分压比
    float adc_voltage = (float)adc_value * 3.3f / 4095.0f;
    float actual_voltage = adc_voltage * ADC_DIVIDER_RATIO;
    
    return actual_voltage;
}

/**
 * @brief 计算电压补偿系数
 * @param voltage 当前电压
 * @retval 补偿系数
 */
static float Car_Task_CalcCompensation(float voltage)
{
    // 调试模式显示
    if (DEBUG_SIMULATE_VOLTAGE > 0.0f) {
        // 调试模式下也进行补偿计算
        if (voltage < 5.0f || voltage > 10.0f) {
            return 1.0f;
        }
        return REFERENCE_VOLTAGE / voltage;
    }
    
    // 正常模式：电压在 6.0V - 8.4V 范围内才补偿
    if (voltage < 6.0f || voltage > 8.4f) {
        // 电压超出正常范围，不补偿
        return 1.0f;
    }
    
    // 补偿系数 = 参考电压 / 当前电压
    // 电压越低，补偿系数越大，电机PWM相应增加
    return REFERENCE_VOLTAGE / voltage;
}

/**
 * @brief 小车任务初始化
 * @retval 无
 */
void Car_Task_Init(void)
{
    // 初始化电机
    Motor_Init();
    
    // 初始化ADC用于电压检测
    Car_Task_InitADC();
    
    // 初始化陀螺仪
    // 尝试初始化 ICM42670
    if (ICM42670_Init()) {
        current_gyro_type = GYRO_TYPE_ICM42670;
        printf("ICM42670 initialized successfully!\r\n");
    }
    else {
        // 如果 ICM42670 失败，尝试初始化串口陀螺仪（WIT）
        //extern void Gryo_init(void);
        Gryo_init();  // 串口陀螺仪初始化
        current_gyro_type = GYRO_TYPE_WIT;
        printf("WIT gyro initialized!\r\n");
    }
    
    // 初始化任务状态
    car_task_running = 0;
    
    // 初始化PID变量
    error = 0.0f;
    last_error = 0.0f;
    integral = 0.0f;
    derivative = 0.0f;
    pid_output = 0.0f;
}

/**
 * @brief 小车任务开始
 * @retval 无
 */
void Car_Task_Start(void)
{
    car_task_running = 1;
    printf("Car task started!\r\n");
}

/**
 * @brief 小车任务停止
 * @retval 无
 */
void Car_Task_Stop(void)
{
    car_task_running = 0;
    
    // 停止所有电机
    Motor_StopAll();
    printf("Car task stopped!\r\n");
}

/**
 * @brief 通用陀螺仪更新函数，根据当前使用的陀螺仪类型调用对应的更新函数
 * @retval 无
 */
static void Car_Gryo_Update(void)
{
    switch (current_gyro_type) {
        case GYRO_TYPE_ICM42670:
            // 使用 ICM42670 陀螺仪
            ICM42670_Gryo_Update();
            break;
        case GYRO_TYPE_WIT:
            // 使用原来的陀螺仪
            //extern void Gryo_Update(void);
            Gryo_Update();
            break;
        case GYRO_TYPE_NONE:
        default:
            // 无陀螺仪，不做任何操作
            break;
    }
}

/**
 * @brief 小车任务主函数
 * @retval 无
 */
void Car_Task_Main(void)
{
    if (!car_task_running)
    {
        return;
    }
    
    // 清除OLED屏幕
    OLED_Clear(0);
    
    // 更新陀螺仪数据
    Car_Gryo_Update();
    
    // 读取电池电压
    current_voltage = Car_Task_ReadVoltage();
    
    // 计算电压补偿系数
    voltage_compensation = Car_Task_CalcCompensation(current_voltage);
    
    // 获取当前陀螺仪z轴角度
    float current_angle = fAngle[2];
    
    // 计算误差
    error = target_angle - current_angle;
    
    // 计算积分项
    integral += error * 0.01f; // 假设采样时间为10ms
    
    // 计算微分项
    derivative = (error - last_error) / 0.01f; // 假设采样时间为10ms
    
    // 计算PID输出
    pid_output = kp * error + ki * integral + kd * derivative;
    
    // 更新上一次误差
    last_error = error;
    
    // 计算电机速度（基础速度）
    int16_t base_speed = 150;
    int16_t left_speed = base_speed ;//- (int16_t)pid_output;
    int16_t right_speed = base_speed ;//+ (int16_t)pid_output;
    
    // 应用电压补偿
    left_speed = (int16_t)(left_speed * voltage_compensation);
    right_speed = (int16_t)(right_speed * voltage_compensation);
    
    // 限制电机速度在合理范围内
    if (left_speed > max_speed) left_speed = max_speed;
    if (left_speed < -max_speed) left_speed = -max_speed;
    if (right_speed > max_speed) right_speed = max_speed;
    if (right_speed < -max_speed) right_speed = -max_speed;
    
    // 设置电机速度
    Motor_SetSpeed(MOTOR_FRONT_LEFT, left_speed);
    Motor_SetSpeed(MOTOR_FRONT_RIGHT, right_speed);
    Motor_SetSpeed(MOTOR_BACK_LEFT, left_speed);
    Motor_SetSpeed(MOTOR_BACK_RIGHT, right_speed);
    
    // 显示标题
    OLED_ShowString(0, 0, "Car Task", 16);
    
    // 显示陀螺仪z轴角度信息
    char str[20];
    sprintf(str, "Z:%.1f T:%.1f", current_angle, target_angle);
    OLED_ShowString(0, 2, str, 12);
    
    // 显示电压和补偿信息
    sprintf(str, "V:%.1f C:%.2f", current_voltage, voltage_compensation);
    OLED_ShowString(0, 3, str, 12);
    
    // 显示电机PWM速度信息
    sprintf(str, "L:%d R:%d", left_speed, right_speed);
    OLED_ShowString(0, 4, str, 12);
    
    // 刷新OLED屏幕
    OLED_Refresh();
    
    // 打印调试信息
    // printf("Angle: %.2f, Voltage: %.2f, Comp: %.2f, Left: %d, Right: %d\r\n", 
    //        current_angle, current_voltage, voltage_compensation, left_speed, right_speed);
}

/**
 * @brief 获取小车任务运行状态
 * @retval 运行状态：1-运行中，0-停止
 */
uint8_t Car_Task_IsRunning(void)
{
    return car_task_running;
}

/**
 * @brief 设置小车目标角度
 * @param angle 目标角度（度）
 * @retval 无
 */
void Car_Task_SetTargetAngle(float angle)
{
    target_angle = angle;
}

/**
 * @brief 设置小车最大速度
 * @param speed 最大速度（-300到300）
 * @retval 无
 */
void Car_Task_SetMaxSpeed(int16_t speed)
{
    if (speed > 300)
    {
        max_speed = 300;
    }
    else if (speed < -300)
    {
        max_speed = -300;
    }
    else
    {
        max_speed = speed;
    }
}
