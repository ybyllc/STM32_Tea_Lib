#include "rf_r1a_m5n.h"
/**
 * @brief  蜂鸟 灵R1A-M5N无线遥控器驱动
 * @note 官网链接：https://www.fengniaorf.com/pd.jsp?recommendFromPid=0&id=87&fromMid=416
 */
/************************* 内部静态变量 *************************/
static uint8_t r1a_rx_buf[R1A_RX_BUF_SIZE] = {0};  // DMA接收缓冲区
static uint8_t r1a_addr_code[3] = {0};              // 遥控器地址码
static uint8_t r1a_key_value = 0;                   // 按键值
static uint8_t r1a_new_data_flag = 0;               // 新数据标志
static uint16_t r1a_rx_len = 0;                      // 接收数据长度

/************************* 内部工具函数 *************************/
/**
 * @brief  ASCII字符转16进制（严格遵循规格书逻辑）
 * @param  d 输入的ASCII字符
 * @retval 转换后的16进制数，0xFF=无效字符
 */
static uint8_t ASC_To_Hex(uint8_t d)
{
    if(d >= '0' && d <= '9') return d & 0x0F;
    if(d >= 'A' && d <= 'F') return d - 'A' + 0x0A;
    if(d >= 'a' && d <= 'f') return d - 'a' + 0x0A;
    return 0xFF; // 无效数据
}

/**
 * @brief  R1A串口数据解析函数（严格遵循规格书帧格式）
 * @param  buf 接收的原始数据缓冲区
 * @param  len 数据长度
 * @retval 1=解析成功，0=解析失败
 */
static uint8_t R1A_Frame_Parse(uint8_t *buf, uint16_t len)
{
    uint8_t hex_buf[8] = {0};
    uint8_t calc_check = 0;
    uint8_t recv_check = 0;
    uint8_t i = 0;

    // 1. 长度校验：最短帧长 LC:XXXXXXXX\r\n = 3+8+2=13字节
    if(len < 13) return 0;

    // 2. 帧头校验：必须是 "LC:"
    if((buf[0] != R1A_FRAME_HEAD1) || (buf[1] != R1A_FRAME_HEAD2) || (buf[2] != R1A_FRAME_HEAD3))
    {
        return 0;
    }

    // 3. 8位数据位ASCII转HEX（DAB10893 → 8个字符）
    for(i = 0; i < 8; i++)
    {
        hex_buf[i] = ASC_To_Hex(buf[3 + i]);
        if(hex_buf[i] == 0xFF) return 0; // 无效字符，解析失败
    }

    // 4. 组合数据（严格遵循规格书示例逻辑）
    // 地址码：前6个字符 DA B1 08
    r1a_addr_code[0] = (hex_buf[0] << 4) + hex_buf[1]; // DA
    r1a_addr_code[1] = (hex_buf[2] << 4) + hex_buf[3]; // B1
    r1a_addr_code[2] = (hex_buf[4] << 4) + hex_buf[5]; // 08
    // 按键值：第6个字符（注意：规格书中第6位既是地址码的一部分，也是按键值）
    r1a_key_value = hex_buf[5];
    // 校验和：最后2个字符
    recv_check = (hex_buf[6] << 4) + hex_buf[7];

    // 5. 和校验：地址码3字节相加，取低8位
    calc_check = r1a_addr_code[0] + r1a_addr_code[1] + r1a_addr_code[2];
    if(calc_check != recv_check)
    {
        return 0;
    }

    // 6. 解析成功，置位标志
    r1a_new_data_flag = 1;
    return 1;
}

/************************* 对外接口函数 *************************/
/**
 * @brief  R1A驱动初始化（开启DMA+串口接收）
 * @param  无
 * @retval 无
 */
void R1A_Init(void)
{
    // 清空缓冲区
    memset(r1a_rx_buf, 0, R1A_RX_BUF_SIZE);
    r1a_addr_code[0] = 0;
    r1a_addr_code[1] = 0;
    r1a_addr_code[2] = 0;
    r1a_key_value = 0;
    r1a_new_data_flag = 0;

    // 开启串口DMA空闲中断接收
    HAL_UARTEx_ReceiveToIdle_DMA(&R1A_UART_HANDLE, r1a_rx_buf, R1A_RX_BUF_SIZE);
    // 关闭DMA半传输中断（可选，减少中断触发）
    __HAL_DMA_DISABLE_IT(&R1A_RX_DMA_HANDLE, DMA_IT_HT);
}

/**
 * @brief  串口空闲中断处理函数（必须在串口中断函数里调用）
 * @param  无
 * @retval 无
 */
void R1A_UART_IDLE_IRQHandler(void)
{
    // 判断空闲中断事件
    if(__HAL_UART_GET_FLAG(&R1A_UART_HANDLE, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&R1A_UART_HANDLE); // 清除空闲中断标志
        HAL_UART_DMAStop(&R1A_UART_HANDLE);           // 停止DMA

        // 获取本次接收的数据长度
        r1a_rx_len = R1A_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&R1A_RX_DMA_HANDLE);

        // 解析数据
        R1A_Frame_Parse(r1a_rx_buf, r1a_rx_len);

        // 清空缓冲区，重新开启DMA接收
        memset(r1a_rx_buf, 0, R1A_RX_BUF_SIZE);
        HAL_UARTEx_ReceiveToIdle_DMA(&R1A_UART_HANDLE, r1a_rx_buf, R1A_RX_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(&R1A_RX_DMA_HANDLE, DMA_IT_HT);
    }
}

/**
 * @brief  获取地址和按键值
 * @param  addr 输出参数，存储3字节地址码（addr[0], addr[1], addr[2]）
 * @param  key  输出参数，存储按键值
 * @retval 1=获取成功，0=无新数据/获取失败
 */
uint8_t R1A_Get_Addr_Key(uint8_t *addr, uint8_t *key)
{
    if(r1a_new_data_flag == 1)
    {
        addr[0] = r1a_addr_code[0];
        addr[1] = r1a_addr_code[1];
        addr[2] = r1a_addr_code[2];
        *key = r1a_key_value;
        r1a_new_data_flag = 0; // 读取后自动清除标志
        return 1;
    }
    return 0;
}

// 串口接收事件回调（备用，HAL库自带）
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == &R1A_UART_HANDLE)
    {
        // 备用解析逻辑，空闲中断已处理，此处可留空
    }
}
