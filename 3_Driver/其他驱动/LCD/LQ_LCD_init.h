#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stdint.h"
#include "LQ_GPIO_Cfg.h" //龙邱gpio口的定义


#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 135
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 135
#endif

#define LCD_SCLK_Clr() LQ_PinWrite(D12, 0U) //SCL=SCLK
#define LCD_SCLK_Set() LQ_PinWrite(D12, 1U)

#define LCD_MOSI_Clr() LQ_PinWrite(E12, 0U) //SDA=MOSI
#define LCD_MOSI_Set() LQ_PinWrite(E12, 1U)

#define LCD_CS_Clr()   LQ_PinWrite(D11, 0U)//CS
#define LCD_CS_Set()   LQ_PinWrite(D11, 1U)

#define LCD_RES_Clr()  LQ_PinWrite(E11, 0U) //RST B13
#define LCD_RES_Set()  LQ_PinWrite(E11, 1U)

#define LCD_DC_Clr()   LQ_PinWrite(B13, 0U) //DC E11
#define LCD_DC_Set()   LQ_PinWrite(B13, 1U)

#define LCD_BLK_Clr()  LQ_PinWrite(G11, 0U)//BLK
#define LCD_BLK_Set()  LQ_PinWrite(G11, 1U)

void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




