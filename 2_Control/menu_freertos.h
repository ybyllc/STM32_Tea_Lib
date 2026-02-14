/**
 * @file menu_freertos.h
 * @brief FreeRTOS 菜单系统适配接口
 * @note 展示如何在多任务环境下使用菜单系统
 */

#ifndef MENU_FREERTOS_H
#define MENU_FREERTOS_H

#include "menu.h"

/**
 * @brief FreeRTOS 菜单系统初始化
 * @note 创建任务、队列和信号量
 */
void Menu_FreeRTOS_Init(void);

/**
 * @brief FreeRTOS 菜单系统启动
 * @note 启动 FreeRTOS 调度器
 * @attention 此函数应在 main() 中调用
 */
void Menu_FreeRTOS_Start(void);

/**
 * @brief FreeRTOS 菜单系统停止
 * @note 删除任务、队列和信号量
 */
void Menu_FreeRTOS_Stop(void);

#endif /* MENU_FREERTOS_H */
