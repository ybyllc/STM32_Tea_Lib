/**
 * @file remote_key.h
 * @brief 蜂鸟R1A M5N无线芯片遥控器驱动
 * @note 支持单点、长按、长按松开三种事件
 */

#ifndef REMOTE_KEY_H
#define REMOTE_KEY_H

#include <stdint.h>
#include "common.h"

// 按键枚举
typedef enum {
    REMOTE_KEY_D0 = 0,  // B5
    REMOTE_KEY_D1 = 1,  // B4
    REMOTE_KEY_D2 = 2,  // A12
    REMOTE_KEY_D3 = 3,  // A11
    REMOTE_KEY_MAX
} RemoteKeyType;

// 按键事件类型
typedef enum {
    REMOTE_KEY_EVENT_NONE = 0,         // 无事件
    REMOTE_KEY_EVENT_SINGLE_CLICK,     // 单点
    REMOTE_KEY_EVENT_LONG_PRESS,       // 长按
    REMOTE_KEY_EVENT_LONG_PRESS_RELEASE // 长按松开
} RemoteKeyEventType;

// 按键状态结构体
typedef struct {
    uint8_t current_state;     // 当前状态
    uint8_t last_state;        // 上一次状态
    uint32_t press_start_time; // 按下开始时间
    uint8_t long_press_detected; // 长按是否已检测
} RemoteKeyState;

/**
 * @brief 初始化遥控器按键
 */
void RemoteKey_Init(void);

/**
 * @brief 扫描遥控器按键状态
 * @note 在主循环中调用
 */
void RemoteKey_Scan(void);

/**
 * @brief 获取按键事件
 * @param key 按键类型
 * @return 按键事件类型
 */
RemoteKeyEventType RemoteKey_GetEvent(RemoteKeyType key);

/**
 * @brief 获取按键当前状态
 * @param key 按键类型
 * @return 1-按下, 0-释放
 */
uint8_t RemoteKey_GetState(RemoteKeyType key);

// 按键引脚映射结构体
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} RemoteKeyPinMap;

// 按键引脚映射
extern const RemoteKeyPinMap key_pins[REMOTE_KEY_MAX];

#endif /* REMOTE_KEY_H */