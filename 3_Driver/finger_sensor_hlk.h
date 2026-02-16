//=============================================================================
// 文件名: finger_sensor_hlk.h
// 功能:   海凌科HLK-ZW800指纹传感器驱动
// 作者:   
// 日期:   2026-02-16
// 通信:   UART 57600 8N1
//=============================================================================

#ifndef __FINGER_SENSOR_HLK_H
#define __FINGER_SENSOR_HLK_H

#include "common.h"

//=============================================================================
// 新手使用流程示例
//=============================================================================
/*

1. 硬件连接
   - V_TOUCH -> 3.3V
   - TOUCHOUT -> MCU唤醒引脚
   - VCC  -> 3.3V
   - TX   -> MCU的RX引脚
   - RX   -> MCU的TX引脚
   - GND  -> GND

2. 初始化（只需调用一次）
   UART_HandleTypeDef huart2;
   Finger_Init(&huart2);
   
3. 验证模块密码（每次上电后需要，0xFFFFFFFF）
   if (Finger_CheckPassword() == FINGER_OK) {
       // 密码正确，可以开始使用
   }

4. 录入指纹（示例：录入到ID=1）
   u16 id = 1;
   if (Finger_Enroll(id) == FINGER_OK) {
       // 录入成功！
       // 过程：按照提示按压指纹3次
   }

5. 验证指纹
   u16 match_id;
   if (Finger_Verify(&match_id) == FINGER_OK) {
       // 验证成功！
       // match_id 就是匹配到的指纹ID
   }

6. 删除指纹
   Finger_Delete(1, 1);   // 删除ID=1的1个指纹
   Finger_Empty();         // 清空所有指纹

-------------------------------------------------------------------------------
*/

//=============================================================================
// 配置参数
//=============================================================================
#define FINGER_BAUDRATE  57600   // 波特率
#define FINGER_BUF_SIZE  128     // 缓冲区大小

//=============================================================================
// 返回值定义
//=============================================================================
#define FINGER_OK    0   // 操作成功
#define FINGER_ERROR 1   // 操作失败

//=============================================================================
// 基础功能函数
//=============================================================================

/**
  * @brief  初始化指纹传感器
  * @param  huart: UART句柄指针
  * @retval 无
  */
void Finger_Init(UART_HandleTypeDef *huart);

/**
  * @brief  验证传感器密码（默认密码: 0xFFFFFFFF）
  * @param  无
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_CheckPassword(void);

/**
  * @brief  获取指纹图像
  * @param  无
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_GetImage(void);

/**
  * @brief  将图像转换为特征
  * @param  slot: 缓冲区编号 (1或2)
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_Img2Tz(u8 slot);

/**
  * @brief  合并两个特征生成模板
  * @param  无
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_RegModel(void);

/**
  * @brief  保存指纹模板到指定位置
  * @param  page_id: 指纹ID (0-1000)
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_Store(u16 page_id);

/**
  * @brief  在指纹库中搜索匹配
  * @param  match_id: 输出参数，匹配到的指纹ID
  * @param  score:    输出参数，匹配分数
  * @retval FINGER_OK-匹配成功, FINGER_ERROR-匹配失败
  */
u8 Finger_Search(u16 *match_id, u16 *score);

/**
  * @brief  删除指定ID的指纹
  * @param  page_id: 起始指纹ID
  * @param  count:   删除数量
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_Delete(u16 page_id, u16 count);

/**
  * @brief  清空所有指纹
  * @param  无
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
u8 Finger_Empty(void);

//=============================================================================
// 高级功能函数（新手推荐使用）
//=============================================================================

/**
  * @brief  一键录入指纹（自动完成3次采集）
  * @param  page_id: 要保存的指纹ID
  * @retval FINGER_OK-录入成功, FINGER_ERROR-录入失败
  * @note   使用前请先调用Finger_Init()和Finger_CheckPassword()
  */
u8 Finger_Enroll(u16 page_id);

/**
  * @brief  一键验证指纹
  * @param  match_id: 输出参数，匹配到的指纹ID
  * @retval FINGER_OK-验证成功, FINGER_ERROR-验证失败
  * @note   使用前请先调用Finger_Init()和Finger_CheckPassword()
  */
u8 Finger_Verify(u16 *match_id);


#endif
