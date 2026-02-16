#ifndef __PS2_H
#define __PS2_H
#include "main.h"
#include "common.h"

/*********************************************************
Copyright (C), 2015-2025, YFRobot.
www.yfrobot.com
File：PS2驱动程序
Author：pinggai    Version:1.0     Data:2015/05/16
Description: PS2驱动程序
**********************************************************/ 

// PS2引脚定义
#define PS2_DI_PORT     GPIOC
#define PS2_DI_PIN      GPIO_PIN_0    // 输入 DAT
#define PS2_DO_PORT     GPIOC
#define PS2_DO_PIN      GPIO_PIN_1    // 输出 CMD
#define PS2_CS_PORT     GPIOC
#define PS2_CS_PIN      GPIO_PIN_2    // 输出 CS
#define PS2_CLK_PORT    GPIOC
#define PS2_CLK_PIN     GPIO_PIN_3    // 输出 CLK

#define PS2_Delayus(n) Delay_us(n)

// 使用HAL库函数操作GPIO
#define DI   HAL_GPIO_ReadPin(PS2_DI_PORT, PS2_DI_PIN)           //输入 DAT

#define DO_H HAL_GPIO_WritePin(PS2_DO_PORT, PS2_DO_PIN, GPIO_PIN_SET)        //命令位高
#define DO_L HAL_GPIO_WritePin(PS2_DO_PORT, PS2_DO_PIN, GPIO_PIN_RESET)        //命令位低

#define CS_H HAL_GPIO_WritePin(PS2_CS_PORT, PS2_CS_PIN, GPIO_PIN_SET)       //CS拉高
#define CS_L HAL_GPIO_WritePin(PS2_CS_PORT, PS2_CS_PIN, GPIO_PIN_RESET)       //CS拉低

#define CLK_H HAL_GPIO_WritePin(PS2_CLK_PORT, PS2_CLK_PIN, GPIO_PIN_SET)      //时钟拉高
#define CLK_L HAL_GPIO_WritePin(PS2_CLK_PORT, PS2_CLK_PIN, GPIO_PIN_RESET)      //时钟拉低


//按键序号表
#define PSB_SELECT      1
#define PSB_L3          2   //左摇杆
#define PSB_R3          3   //右摇杆
#define PSB_START       4
#define PSB_PAD_UP      5   //↑
#define PSB_PAD_RIGHT   6   //→
#define PSB_PAD_DOWN    7   //↓
#define PSB_PAD_LEFT    8   //←
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14  //B
#define PSB_BLUE        15  //A
#define PSB_PINK        16  //X
#define PSB_TRIANGLE    13  //Y
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      26

//#define WHAMMY_BAR		8

//These are stick values
#define PSS_RX 5                //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8



extern u8 Data[9];
extern u16 MASK[16];
extern u16 Handkey;

void PS2_Init(void);
u8 PS2_RedLight(void);//判断是否为红灯模式
void PS2_ReadData(void);
void PS2_Cmd(u8 CMD);		  //
u8 PS2_DataKey(void);		  //键值读取
u8 PS2_AnologData(u8 button); //得到一个摇杆的模拟量
void PS2_ClearData(void);	  //清除数据缓冲区

void PS2_SetInit(void);
void PS2_Vibration(u8 motor1,u8 motor2);

#endif



