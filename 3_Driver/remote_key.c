/**
 * @file remote_key.c
 * @brief 蜂鸟R1A M5N无线芯片遥控器驱动实现
 * @note 支持单点、长按、长按松开三种事件
 */

#include "remote_key.h"

// 按键引脚映射
const RemoteKeyPinMap key_pins[REMOTE_KEY_MAX] = {
    {D0_433_GPIO_Port, D0_433_Pin},  // REMOTE_KEY_D0: B5
    {D1_433_GPIO_Port, D1_433_Pin},  // REMOTE_KEY_D1: B4
    {D2_433_GPIO_Port, D2_433_Pin},  // REMOTE_KEY_D2: A12
    {D3_433_GPIO_Port, D3_433_Pin}   // REMOTE_KEY_D3: A11
};

// 长按检测时间阈值（ms）
#define REMOTE_KEY_LONG_PRESS_THRESHOLD 1000

// 按键状态数组
static RemoteKeyState key_states[REMOTE_KEY_MAX];
// 按键事件数组
static RemoteKeyEventType key_events[REMOTE_KEY_MAX];

/**
 * @brief 初始化遥控器按键
 */
void RemoteKey_Init(void)
{
    // 初始化按键状态
    for (uint8_t i = 0; i < REMOTE_KEY_MAX; i++) {
        key_states[i].current_state = 0;
        key_states[i].last_state = 0;
        key_states[i].press_start_time = 0;
        key_states[i].long_press_detected = 0;
        key_events[i] = REMOTE_KEY_EVENT_NONE;
    }
}

/**
 * @brief 读取按键状态
 * @param key 按键类型
 * @return 1-按下, 0-释放
 */
static uint8_t RemoteKey_ReadState(RemoteKeyType key)
{
    if (key >= REMOTE_KEY_MAX) {
        return 0;
    }
    
    // 读取引脚状态（低电平表示按下）
    return (HAL_GPIO_ReadPin(key_pins[key].port, key_pins[key].pin) == GPIO_PIN_RESET) ? 1 : 0;
}

/**
 * @brief 扫描遥控器按键状态
 * @note 在主循环中调用
 */
void RemoteKey_Scan(void)
{
    uint32_t current_time = HAL_GetTick();
    
    for (uint8_t i = 0; i < REMOTE_KEY_MAX; i++) {
        // 读取当前状态
        key_states[i].current_state = RemoteKey_ReadState((RemoteKeyType)i);
        
        // 重置事件
        key_events[i] = REMOTE_KEY_EVENT_NONE;
        
        // 检测状态变化
        if (key_states[i].current_state != key_states[i].last_state) {
            if (key_states[i].current_state == 1) {
                // 按键按下
                key_states[i].press_start_time = current_time;
                key_states[i].long_press_detected = 0;
            } else {
                // 按键松开
                if (key_states[i].long_press_detected) {
                    // 长按后松开
                    key_events[i] = REMOTE_KEY_EVENT_LONG_PRESS_RELEASE;
                } else {
                    // 单点
                    key_events[i] = REMOTE_KEY_EVENT_SINGLE_CLICK;
                }
            }
        } else {
            // 状态未变化，检测长按
            if (key_states[i].current_state == 1) {
                // 按键仍按下
                if (!key_states[i].long_press_detected) {
                    if ((current_time - key_states[i].press_start_time) >= REMOTE_KEY_LONG_PRESS_THRESHOLD) {
                        // 检测到长按
                        key_events[i] = REMOTE_KEY_EVENT_LONG_PRESS;
                        key_states[i].long_press_detected = 1;
                    }
                }
            }
        }
        
        // 更新上一次状态
        key_states[i].last_state = key_states[i].current_state;
    }
}

/**
 * @brief 获取按键事件
 * @param key 按键类型
 * @return 按键事件类型
 */
RemoteKeyEventType RemoteKey_GetEvent(RemoteKeyType key)
{
    if (key >= REMOTE_KEY_MAX) {
        return REMOTE_KEY_EVENT_NONE;
    }
    
    return key_events[key];
}

/**
 * @brief 获取按键当前状态
 * @param key 按键类型
 * @return 1-按下, 0-释放
 */
uint8_t RemoteKey_GetState(RemoteKeyType key)
{
    if (key >= REMOTE_KEY_MAX) {
        return 0;
    }
    
    return key_states[key].current_state;
}
