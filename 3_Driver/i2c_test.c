#include "soft_iic.h"

// 测试 I2C 函数
void I2C_Test(void)
{
    // 创建 I2C 实例
    static soft_iic_t test_iic;
    
    // 初始化 I2C
    soft_iic_init(&test_iic, GPIOA, GPIO_PIN_8, GPIOC, GPIO_PIN_9, 0x29, 5);
    
    // 测试 I2C 基本操作
    soft_iic_start(&test_iic);
    soft_iic_send_byte(&test_iic, 0x52);  // 写地址
    soft_iic_wait_ack(&test_iic);
    soft_iic_stop(&test_iic);
}
