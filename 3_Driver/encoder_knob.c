/**
 * @file encoder_knob.c
 * @brief EC11 旋转编码器驱动 - 工业级可靠性版本
 * @note 两状态解码算法，支持双向检测
 *       在 TIM1_100us_Callback 中每10次(1ms)调用一次 Encoder_Scan()
 */

#include "encoder_knob.h"
#include "main.h"

// 编码器状态
static volatile int32_t encoder_count = 0;
static uint8_t last_a = 1;
static uint8_t last_b = 1;
static uint32_t last_debounce_time = 0;

#define DEBOUNCE_TIME 2   // 去抖动时间 (ms)

// 按键状态
static uint8_t key_last = 1;
static uint32_t key_tick = 0;
static uint8_t key_evt = 0;
static uint8_t long_press_triggered = 0;  // 长按触发标志，0:未触发，1:已触发

// 定时器计数
static uint8_t timer_cnt = 0;

void Ec11_knob_Init(void)
{
    encoder_count = 0;
    last_a = 1;
    last_b = 1;
    last_debounce_time = 0;
    key_last = 1;
    key_evt = 0;
    long_press_triggered = 0;  // 初始化长按触发标志
    timer_cnt = 0;
    
    // 初始化状态
    last_a = HAL_GPIO_ReadPin(OLED_TR_A_GPIO_Port, OLED_TR_A_Pin);
    last_b = HAL_GPIO_ReadPin(OLED_TR_B_GPIO_Port, OLED_TR_B_Pin);
}

/**
 * @brief 编码器定时器回调 - 每100us调用一次
 * @note 内部每10次(1ms)执行一次扫描
 */
void Ec11_knob_TIM_Callback(void)
{
    timer_cnt++;
    if (timer_cnt >= 10)  // 1ms 到
    {
        timer_cnt = 0;
        Ec11_knob_Scan();
        Ec11_knob_Key_Scan();
    }
}

/**
 * @brief 编码器扫描 - 1ms执行一次
 * @note 两状态解码算法（工业级可靠性）：
 *       - 顺时针：A下降沿时B为高电平
 *       - 逆时针：A下降沿时B为低电平
 *       优势：逻辑简单，响应快速，适合快速旋转
 */
void Ec11_knob_Scan(void)
{
    uint8_t a_now = HAL_GPIO_ReadPin(OLED_TR_A_GPIO_Port, OLED_TR_A_Pin);
    uint8_t b_now = HAL_GPIO_ReadPin(OLED_TR_B_GPIO_Port, OLED_TR_B_Pin);
    uint32_t now = HAL_GetTick();
    
    // 去抖动 - 只在状态变化时进行
    if (a_now != last_a || b_now != last_b)
    {
        // 检查去抖动时间
        if (now - last_debounce_time >= DEBOUNCE_TIME)
        {
            // 检测A相下降沿
            if (last_a == 1 && a_now == 0)
            {
                // 根据B相状态判断方向
                if (b_now == 1)
                {
                    // 顺时针
                    encoder_count++;
                }
                else
                {
                    // 逆时针
                    encoder_count--;
                }
            }
            
            // 更新状态
            last_a = a_now;
            last_b = b_now;
            last_debounce_time = now;
        }
    }
    else
    {
        // 状态未变化，重置去抖动时间
        last_debounce_time = now;
    }
}

/**
 * @brief 按键扫描 - 1ms执行一次
 */
void Ec11_knob_Key_Scan(void)
{
    uint8_t now = HAL_GPIO_ReadPin(OLED_PUSH_GPIO_Port, OLED_PUSH_Pin);
    uint32_t tick = HAL_GetTick();
    
    // 不要每次都重置key_evt，让Ec11_knob_Key_GetEvent()来处理重置
    
    if (now != key_last)
    {
        if (tick - key_tick > 20)  // 20ms消抖
        {
            key_last = now;
            if (now == 0)//按键按下
            {
                key_tick = tick;
                long_press_triggered = 0;  // 重置长按触发标志
                // 按下时不产生事件
            }
            else
            {
                // 按键释放，计算持续时间
                uint32_t dur = tick - key_tick;
                if (dur >= 20 && dur < 800)
                    key_evt = 1;  // 短按
                // 长按已经在按下期间处理
                long_press_triggered = 0;  // 重置长按触发标志
            }
        }
    }
    else if (now == 0 && key_last == 0)
    {
        // 按键持续按下，检查是否达到长按时间
        uint32_t dur = tick - key_tick;
        if (dur >= 800 && long_press_triggered == 0)
        {
            // 长按期间实时返回2，且只触发一次
            key_evt = 2;  // 长按
            long_press_triggered = 1;  // 标记已触发
        }
    }
}

int32_t Ec11_knob_Get_Count(void)
{
    return encoder_count;
}

void Ec11_knob_Set_Count(int32_t count)
{
    encoder_count = count;
}

void Ec11_knob_Clear_Count(void)
{
    encoder_count = 0;
}

//获取按键事件
// 0:无事件 1:短按 2:长按
uint8_t Ec11_knob_Key_GetEvent(void)
{
    uint8_t evt = key_evt;
    key_evt = 0;
    return evt;
}

uint8_t Ec11_knob_Key_GetState(void)
{
    return key_last;
}
