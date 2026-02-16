//=============================================================================
// 文件名: finger_sensor_hlk.c
// 功能:   指纹传感器驱动
// 模块:   海凌科HLK-ZW800
// 官网:   https://h.hlktech.com/Mobile/download/FDetail/184.html
//=============================================================================

#include "finger_sensor_hlk.h"

//=============================================================================
// 静态变量
//=============================================================================
static UART_HandleTypeDef *finger_huart;  // UART句柄
static u8 tx_buf[FINGER_BUF_SIZE];        // 发送缓冲区
static u8 rx_buf[FINGER_BUF_SIZE];        // 接收缓冲区

//=============================================================================
// 内部辅助函数
//=============================================================================

/**
  * @brief  计算校验和
  * @param  data: 数据指针
  * @param  len:  数据长度
  * @retval 校验和
  */
static u16 Calc_Checksum(u8 *data, u16 len)
{
    u16 sum = 0;
    for (u16 i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

/**
  * @brief  发送数据包
  * @param  data: 数据指针
  * @param  len:  数据长度
  * @retval 无
  */
static void Send_Packet(u8 *data, u16 len)
{
    if (finger_huart) {
        HAL_UART_Transmit(finger_huart, data, len, 1000);
    }
}

/**
  * @brief  接收数据包
  * @param  timeout: 超时时间(ms)
  * @retval 1-成功, 0-失败
  */
static u8 Receive_Packet(u32 timeout)
{
    u32 start = HAL_GetTick();
    u16 i = 0;
    
    memset(rx_buf, 0, FINGER_BUF_SIZE);
    
    while (i < FINGER_BUF_SIZE) {
        if (HAL_UART_Receive(finger_huart, &rx_buf[i], 1, 200) == HAL_OK) {
            i++;
            if (i >= 9 && rx_buf[0] == 0xEF && rx_buf[1] == 0x01) {
                u16 pkt_len = (rx_buf[7] << 8) | rx_buf[8];
                if (i >= 9 + pkt_len) {
                    return 1;
                }
            }
        }
        if (HAL_GetTick() - start > timeout) {
            break;
        }
    }
    return 0;
}

/**
  * @brief  构建命令包
  * @param  cmd:       命令字
  * @param  param:     参数指针
  * @param  param_len: 参数长度
  * @retval 1-成功
  */
static u8 Build_Cmd(u8 cmd, u8 *param, u16 param_len)
{
    u16 sum;
    u16 pkt_len = param_len + 2;
    
    tx_buf[0] = 0xEF;
    tx_buf[1] = 0x01;
    tx_buf[2] = 0xFF;
    tx_buf[3] = 0xFF;
    tx_buf[4] = 0xFF;
    tx_buf[5] = 0xFF;
    tx_buf[6] = 0x01;
    tx_buf[7] = (pkt_len >> 8) & 0xFF;
    tx_buf[8] = pkt_len & 0xFF;
    tx_buf[9] = cmd;
    
    for (u16 i = 0; i < param_len; i++) {
        tx_buf[10 + i] = param[i];
    }
    
    sum = Calc_Checksum(&tx_buf[6], 3 + param_len);
    tx_buf[10 + param_len] = (sum >> 8) & 0xFF;
    tx_buf[11 + param_len] = sum & 0xFF;
    
    return 1;
}

/**
  * @brief  检查应答包
  * @param  无
  * @retval FINGER_OK-成功, FINGER_ERROR-失败
  */
static u8 Check_Ack(void)
{
    if (rx_buf[0] != 0xEF || rx_buf[1] != 0x01) {
        return FINGER_ERROR;
    }
    if (rx_buf[9] != 0x00) {
        return FINGER_ERROR;
    }
    return FINGER_OK;
}

//=============================================================================
// 基础功能函数实现
//=============================================================================

void Finger_Init(UART_HandleTypeDef *huart)
{
    finger_huart = huart;
}

u8 Finger_CheckPassword(void)
{
    u8 param[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // 模块默认密码
    
    Build_Cmd(0x13, param, 4);
    Send_Packet(tx_buf, 17);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_GetImage(void)
{
    Build_Cmd(0x01, NULL, 0);
    Send_Packet(tx_buf, 12);
    
    if (!Receive_Packet(3000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_Img2Tz(u8 slot)
{
    u8 param[1] = {slot};
    
    Build_Cmd(0x02, param, 1);
    Send_Packet(tx_buf, 13);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_RegModel(void)
{
    Build_Cmd(0x05, NULL, 0);
    Send_Packet(tx_buf, 12);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_Store(u16 page_id)
{
    u8 param[3] = {0x02, (page_id >> 8) & 0xFF, page_id & 0xFF};
    
    Build_Cmd(0x06, param, 3);
    Send_Packet(tx_buf, 15);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_Search(u16 *match_id, u16 *score)
{
    u8 param[5] = {0x01, 0x00, 0x00, 0x03, 0xE8};
    
    Build_Cmd(0x04, param, 5);
    Send_Packet(tx_buf, 17);
    
    if (!Receive_Packet(2000)) {
        return FINGER_ERROR;
    }
    
    if (Check_Ack() == FINGER_OK) {
        if (match_id) {
            *match_id = (rx_buf[10] << 8) | rx_buf[11];
        }
        if (score) {
            *score = (rx_buf[12] << 8) | rx_buf[13];
        }
        return FINGER_OK;
    }
    
    return FINGER_ERROR;
}

u8 Finger_Delete(u16 page_id, u16 count)
{
    u8 param[4] = {(page_id >> 8) & 0xFF, page_id & 0xFF, (count >> 8) & 0xFF, count & 0xFF};
    
    Build_Cmd(0x0C, param, 4);
    Send_Packet(tx_buf, 16);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

u8 Finger_Empty(void)
{
    Build_Cmd(0x0D, NULL, 0);
    Send_Packet(tx_buf, 12);
    
    if (!Receive_Packet(1000)) {
        return FINGER_ERROR;
    }
    
    return Check_Ack();
}

//=============================================================================
// 高级功能函数实现
//=============================================================================

u8 Finger_Enroll(u16 page_id)
{
    for (u8 i = 0; i < 3; i++) {
        if (Finger_GetImage() != FINGER_OK) {
            return FINGER_ERROR;
        }
        if (Finger_Img2Tz(i + 1) != FINGER_OK) {
            return FINGER_ERROR;
        }
    }
    
    if (Finger_RegModel() != FINGER_OK) {
        return FINGER_ERROR;
    }
    
    if (Finger_Store(page_id) != FINGER_OK) {
        return FINGER_ERROR;
    }
    
    return FINGER_OK;
}

u8 Finger_Verify(u16 *match_id)
{
    if (Finger_GetImage() != FINGER_OK) {
        return FINGER_ERROR;
    }
    
    if (Finger_Img2Tz(1) != FINGER_OK) {
        return FINGER_ERROR;
    }
    
    return Finger_Search(match_id, NULL);
}
