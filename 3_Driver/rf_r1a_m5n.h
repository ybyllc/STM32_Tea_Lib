#ifndef __RF_R1A_M5N_H
#define __RF_R1A_M5N_H

/**
 * @file rf_r1a_m5n.h
 * @brief 蜂鸟 灵R1A-M5N无线遥控器驱动
 * @note  官网链接：https://www.fengniaorf.com/pd.jsp?recommendFromPid=0&id=87&fromMid=416
 * 
 * ==================== 硬件接线 ====================
 * M5N模块是开关量输出版本（GPIO接口）：
 *   M5N模块 VCC  -> 3.3V
 *   M5N模块 GND  -> GND
 *   M5N模块 D0   -> MCU的UART_RX引脚（9600bps）
 * 
 * 通信：仅使用D0串口接收（9600bps）
 * 数据格式：LC:123456 9C\r\n，12345为遥控器地址，6为按键值，9C为字节和校验
 * 对码：M5N无需对码，接收所有遥控器
 * 
 * ==================== CubeMX配置步骤 ====================
 * 1. 配置串口（如USART1）：
 *    - Mode: Asynchronous
 *    - Baud Rate: 9600 Bits/s
 *    - NVIC: 使能USART1 global interrupt
 * 
 * 2. 配置DMA（接收）：
 *    - USART1_RX: DMA1 Stream0（或自动分配）
 *    - Mode: Circular（可选，Normal也可）
 *    - NVIC: 使能DMA1 stream0 global interrupt
 * 
 * 3. 生成代码后，修改本文件的【用户配置区】
 * 
 * ==================== 工程集成步骤 ====================
 * 
 * 【步骤1】在main.c中包含头文件
 *    #include "rf_r1a_m5n.h"
 * 
 * 【步骤2】在main()函数中初始化（在MX_USART1_UART_Init()之后调用）
 *    int main(void)
 *    {
 *        HAL_Init();
 *        SystemClock_Config();
 *        MX_GPIO_Init();
 *        MX_DMA_Init();
 *        MX_USART1_UART_Init();  // CubeMX生成的串口初始化
 *        
 *        R1A_Init();  // 初始化R1A驱动（必须在串口初始化之后）
 *        
 *        // 你的其他初始化代码...
 *    }
 * 
 * 【步骤3】在串口中断函数中添加R1A处理（stm32f4xx_it.c）
 *    void USART1_IRQHandler(void)
 *    {
 *        HAL_UART_IRQHandler(&huart1);
 *        R1A_UART_IDLE_IRQHandler();  // 添加这一行
 *    }
 * 
 * 【步骤4】在主循环中读取数据
    // 全局变量
    static uint8_t bind_addr[3] = {0xFF, 0xFF, 0xFF};// 对码的3字节地址码
    static uint8_t is_bound = 0; // 是否已绑定

    while (1)
    {
        uint8_t addr[3], key;
        static uint32_t press_time = 0;
        
        if(R1A_Get_Addr_Key(addr, &key))
        {
            // 对码：新地址长按3秒
            if(memcmp(addr, bind_addr, 3) != 0 && !is_bound)
            {
                if(press_time == 0) press_time = HAL_GetTick();
                if(HAL_GetTick() - press_time > 3000)
                {
                    memcpy(bind_addr, addr, 3);
                    is_bound = 1;
                    press_time = 0;
                    // LED闪一下提示成功
                }
            }
            // 执行：已绑定地址的按键
            else if(memcmp(addr, bind_addr, 3) == 0)
            {
                press_time = 0;
                switch(key) {
                    case 0x00: //按键1 break;
                    case 0x01: //按键2 break;
                    case 0x02: //按键3 break;
                    default: break;
                }
            }
        }
        else
        {
            press_time = 0;
        }
        
        HAL_Delay(10);
    }
 */

#include "common.h"  
#include "string.h"
#include "stdio.h"

/************************* 用户配置区 *************************/
// 适配你的串口配置（CubeMX生成的串口句柄）
#define R1A_UART_HANDLE    huart1        // 串口句柄，如huart1/huart2/huart3
#define R1A_RX_DMA_HANDLE  hdma_usart1_rx // 串口RX对应的DMA句柄

// 缓冲区配置（通常无需修改）
#define R1A_RX_BUF_SIZE    64       // 接收缓冲区大小
#define R1A_FRAME_MAX_LEN  16       // 单帧最大长度
/**************************************************************/

// 帧格式固定参数（规格书定义，禁止修改）
#define R1A_FRAME_HEAD1    'L'
#define R1A_FRAME_HEAD2    'C'
#define R1A_FRAME_HEAD3    ':'
#define R1A_FRAME_TAIL1    '\r'
#define R1A_FRAME_TAIL2    '\n'
#define R1A_BAUDRATE       9600     // 固定波特率9600

// 对外接口函数声明
void R1A_Init(void);                                  // 驱动初始化（在main函数初始化时调用）
void R1A_UART_IDLE_IRQHandler(void);                 // 串口空闲中断处理（在串口中断函数里调用）
uint8_t R1A_Get_Addr_Key(uint8_t *addr, uint8_t *key); // 获取地址和按键值（成功返回1，失败返回0）

#endif
