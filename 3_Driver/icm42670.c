#include "icm42670.h"
#include "soft_iic.h"

// I2C 实例
static soft_iic_t icm_iic;

// 全局变量
int16_t icm42670_gyro_x, icm42670_gyro_y, icm42670_gyro_z;
int16_t icm42670_acc_x, icm42670_acc_y, icm42670_acc_z;
int16_t icm42670_temp;

float icm42670_gyro_x_dps, icm42670_gyro_y_dps, icm42670_gyro_z_dps;
float icm42670_acc_x_g, icm42670_acc_y_g, icm42670_acc_z_g;
float icm42670_temp_c;

// 兼容原来的陀螺仪接口变量
// 使用静态变量，避免与 wit_gyro_sdk.c 中的变量冲突
extern float fAcc[3], fGyro[3], fAngle[3], fYaw;

// 陀螺仪和加速度计的满量程设置（可根据需要修改）
#define GYRO_FS_SEL    ICM42670_GYRO_FS_250DPS
#define ACCEL_FS_SEL   ICM42670_ACCEL_FS_4G

// 采样率设置（可根据需要修改）
#define GYRO_ODR       ICM42670_ODR_1000
#define ACCEL_ODR      ICM42670_ODR_1000

/**
 * @brief  向 ICM-42670 写入一个字节
 * @param  reg : 寄存器地址
 * @param  data : 要写入的数据
 * @retval 无
 */
void ICM42670_Write_Reg(uint8_t reg, uint8_t data)
{
    soft_iic_write_reg(&icm_iic, reg, data);
}

/**
 * @brief  从 ICM-42670 读取一个字节
 * @param  reg : 寄存器地址
 * @retval 读取的数据
 */
u8 ICM42670_Read_Reg(uint8_t reg)
{
    return soft_iic_read_reg(&icm_iic, reg);
}

/**
 * @brief  从 ICM-42670 读取多个字节
 * @param  reg : 起始寄存器地址
 * @param  buf : 数据缓冲区
 * @param  len : 读取的字节数
 * @retval 无
 */
void ICM42670_Read_Regs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    soft_iic_read_bytes(&icm_iic, reg, buf, len);
}

/**
 * @brief  GPIO电平测试
 * @param  无
 * @retval 无
 */
void GPIO_Test(void)
{
    printf("Starting GPIO test...\r\n");
    
    // 测试SCL引脚 - 使用HAL库直接读取
    printf("Testing SCL...\r\n");
    HAL_GPIO_WritePin(ICM42670_SCL_PORT, ICM42670_SCL_PIN, GPIO_PIN_SET);
    delay_ms(100);
    printf("SCL = 1, %s_%d = %d\r\n", 
           (ICM42670_SCL_PORT == GPIOA) ? "PA" : 
           (ICM42670_SCL_PORT == GPIOB) ? "PB" : 
           (ICM42670_SCL_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SCL_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SCL_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SCL_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SCL_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SCL_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SCL_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SCL_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SCL_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SCL_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SCL_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SCL_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SCL_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SCL_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SCL_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SCL_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SCL_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SCL_PORT, ICM42670_SCL_PIN));
    
    HAL_GPIO_WritePin(ICM42670_SCL_PORT, ICM42670_SCL_PIN, GPIO_PIN_RESET);
    delay_ms(100);
    printf("SCL = 0, %s_%d = %d\r\n", 
           (ICM42670_SCL_PORT == GPIOA) ? "PA" : 
           (ICM42670_SCL_PORT == GPIOB) ? "PB" : 
           (ICM42670_SCL_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SCL_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SCL_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SCL_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SCL_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SCL_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SCL_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SCL_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SCL_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SCL_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SCL_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SCL_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SCL_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SCL_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SCL_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SCL_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SCL_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SCL_PORT, ICM42670_SCL_PIN));
    
    // 测试SDA引脚 - 使用HAL库直接读取
    printf("Testing SDA...\r\n");
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ICM42670_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ICM42670_SDA_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(ICM42670_SDA_PORT, ICM42670_SDA_PIN, GPIO_PIN_SET);
    delay_ms(100);
    printf("SDA = 1 (output), %s_%d = %d\r\n", 
           (ICM42670_SDA_PORT == GPIOA) ? "PA" : 
           (ICM42670_SDA_PORT == GPIOB) ? "PB" : 
           (ICM42670_SDA_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SDA_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SDA_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SDA_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SDA_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SDA_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SDA_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SDA_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SDA_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SDA_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SDA_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SDA_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SDA_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SDA_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SDA_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SDA_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SDA_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SDA_PORT, ICM42670_SDA_PIN));
    
    HAL_GPIO_WritePin(ICM42670_SDA_PORT, ICM42670_SDA_PIN, GPIO_PIN_RESET);
    delay_ms(100);
    printf("SDA = 0 (output), %s_%d = %d\r\n", 
           (ICM42670_SDA_PORT == GPIOA) ? "PA" : 
           (ICM42670_SDA_PORT == GPIOB) ? "PB" : 
           (ICM42670_SDA_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SDA_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SDA_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SDA_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SDA_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SDA_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SDA_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SDA_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SDA_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SDA_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SDA_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SDA_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SDA_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SDA_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SDA_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SDA_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SDA_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SDA_PORT, ICM42670_SDA_PIN));
    
    // 断开ICM42670，测试SDA引脚是否被外部拉低
    printf("\r\n!!! 请断开ICM42670模块，然后按任意键继续测试 !!!\r\n");
    //getchar();  // 等待用户按键
    
    HAL_GPIO_WritePin(ICM42670_SDA_PORT, ICM42670_SDA_PIN, GPIO_PIN_SET);
    delay_ms(100);
    printf("SDA = 1 (output, 断开模块), %s_%d = %d\r\n", 
           (ICM42670_SDA_PORT == GPIOA) ? "PA" : 
           (ICM42670_SDA_PORT == GPIOB) ? "PB" : 
           (ICM42670_SDA_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SDA_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SDA_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SDA_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SDA_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SDA_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SDA_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SDA_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SDA_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SDA_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SDA_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SDA_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SDA_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SDA_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SDA_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SDA_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SDA_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SDA_PORT, ICM42670_SDA_PIN));
    
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(ICM42670_SDA_PORT, &GPIO_InitStruct);
    delay_ms(100);
    printf("SDA = input (断开模块), %s_%d = %d\r\n", 
           (ICM42670_SDA_PORT == GPIOA) ? "PA" : 
           (ICM42670_SDA_PORT == GPIOB) ? "PB" : 
           (ICM42670_SDA_PORT == GPIOC) ? "PC" : "??",
           (ICM42670_SDA_PIN == GPIO_PIN_0) ? 0 :
           (ICM42670_SDA_PIN == GPIO_PIN_1) ? 1 :
           (ICM42670_SDA_PIN == GPIO_PIN_2) ? 2 :
           (ICM42670_SDA_PIN == GPIO_PIN_3) ? 3 :
           (ICM42670_SDA_PIN == GPIO_PIN_4) ? 4 :
           (ICM42670_SDA_PIN == GPIO_PIN_5) ? 5 :
           (ICM42670_SDA_PIN == GPIO_PIN_6) ? 6 :
           (ICM42670_SDA_PIN == GPIO_PIN_7) ? 7 :
           (ICM42670_SDA_PIN == GPIO_PIN_8) ? 8 :
           (ICM42670_SDA_PIN == GPIO_PIN_9) ? 9 :
           (ICM42670_SDA_PIN == GPIO_PIN_10) ? 10 :
           (ICM42670_SDA_PIN == GPIO_PIN_11) ? 11 :
           (ICM42670_SDA_PIN == GPIO_PIN_12) ? 12 :
           (ICM42670_SDA_PIN == GPIO_PIN_13) ? 13 :
           (ICM42670_SDA_PIN == GPIO_PIN_14) ? 14 :
           (ICM42670_SDA_PIN == GPIO_PIN_15) ? 15 : 99,
           HAL_GPIO_ReadPin(ICM42670_SDA_PORT, ICM42670_SDA_PIN));
    
    printf("GPIO test completed.\r\n");
}

/**
 * @brief  I2C设备扫描测试
 * @param  无
 * @retval 无
 */
void I2C_Scan(void)
{
    u8 i;
    u8 ack;
    
    printf("Starting I2C scan...\r\n");
    
    for (i = 0; i < 128; i++)
    {
        soft_iic_start(&icm_iic);
        soft_iic_send_byte(&icm_iic, (i << 1) & 0xFE);  // 写地址
        ack = soft_iic_wait_ack(&icm_iic);
        soft_iic_stop(&icm_iic);
        
        if (ack == 0)  // 检测到应答
        {
            printf("I2C device found at address 0x%02X (7-bit: 0x%02X)\r\n", (i << 1), i);
        }
        
        delay_ms(1);  // 延时1ms
    }
    
    printf("I2C scan completed.\r\n");
}

/**
 * @brief  检查 ICM-42670 是否存在（带详细调试）
 * @param  无
 * @retval 1: 成功 0: 失败
 */
u8 ICM42670_Check(void)
{
    u8 dat;
    u8 ack;
    
    printf("ICM42670_Check: Starting I2C communication test...\r\n");
    printf("Device address: 0x%02X (7-bit: 0x%02X)\r\n", ICM42670_I2C_ADDR << 1, ICM42670_I2C_ADDR);
    
    // 步骤1：发送起始信号 + 写地址
    printf("Step 1: Start + Write address...\r\n");
    soft_iic_start(&icm_iic);
    soft_iic_send_byte(&icm_iic, (ICM42670_I2C_ADDR << 1) | 0);  // 写操作
    ack = soft_iic_wait_ack(&icm_iic);
    printf("  Write address ACK: %d (0=OK, 1=FAIL)\r\n", ack);
    
    if (ack) {
        soft_iic_stop(&icm_iic);
        printf("  ERROR: Device not responding to write address!\r\n");
        return 0;
    }
    
    // 步骤2：发送寄存器地址
    printf("Step 2: Send register address 0x%02X...\r\n", ICM42670_WHO_AM_I);
    soft_iic_send_byte(&icm_iic, ICM42670_WHO_AM_I);
    ack = soft_iic_wait_ack(&icm_iic);
    printf("  Register address ACK: %d (0=OK, 1=FAIL)\r\n", ack);
    
    if (ack) {
        soft_iic_stop(&icm_iic);
        printf("  ERROR: Device not responding to register address!\r\n");
        return 0;
    }
    
    // 步骤3：重复起始信号 + 读地址
    printf("Step 3: Repeated Start + Read address...\r\n");
    soft_iic_start(&icm_iic);
    soft_iic_send_byte(&icm_iic, (ICM42670_I2C_ADDR << 1) | 1);  // 读操作
    ack = soft_iic_wait_ack(&icm_iic);
    printf("  Read address ACK: %d (0=OK, 1=FAIL)\r\n", ack);
    
    if (ack) {
        soft_iic_stop(&icm_iic);
        printf("  ERROR: Device not responding to read address!\r\n");
        return 0;
    }
    
    // 步骤4：读取数据
    printf("Step 4: Read data...\r\n");
    dat = soft_iic_read_byte(&icm_iic, 0);  // 读取1字节，发送NACK
    soft_iic_stop(&icm_iic);
    
    printf("ICM42670 WHO_AM_I = 0x%02X (expected 0x41)\r\n", dat);
    
    if (dat == 0x41) {
        printf("ICM42670 found!\r\n");
        return 1;
    } else {
        printf("ERROR: Wrong device ID!\r\n");
        return 0;
    }
}

/**
 * @brief  初始化 ICM-42670
 * @param  无
 * @retval u8 初始化结果：1成功，0失败
 */
u8 ICM42670_Init(void)
{
    uint16_t timeout = 0;
    
    // 初始化软件 I2C（包含SA0和CS引脚初始化）
    soft_iic_init(&icm_iic, ICM42670_SCL_PORT, ICM42670_SCL_PIN, 
                 ICM42670_SDA_PORT, ICM42670_SDA_PIN, ICM42670_I2C_ADDR, 5);
    
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();   //使能GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
    //SA0初始化设置
    GPIO_Initure.Pin=ICM42670_SA0_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(ICM42670_SA0_PORT,&GPIO_Initure);
    HAL_GPIO_WritePin(ICM42670_SA0_PORT, ICM42670_SA0_PIN, GPIO_PIN_SET);  //设置SA0为高电平，选择设备地址0x69
    
    //CS初始化设置
    GPIO_Initure.Pin=ICM42670_CS_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(ICM42670_CS_PORT,&GPIO_Initure);
    HAL_GPIO_WritePin(ICM42670_CS_PORT, ICM42670_CS_PIN, GPIO_PIN_SET);  //设置CS为高电平，禁用SPI模式


    // 延时等待ICM42670启动
    delay_ms(50);

    I2C_Scan();
    // 检查设备是否存在，添加超时检测
    while (!ICM42670_Check())
    {
        timeout++;
        if (timeout > 10) // 超时检测，最多尝试10次
        {
            printf("ICM42670 can not find！\r\n");
            return 0; // 初始化失败
        }
        delay_ms(10); // 每次尝试后延时10ms
    }
    
    // 复位设备
    ICM42670_Write_Reg(ICM42670_PWR_MGMT_0, 0x80);
    delay_us(1000);
    
    // 电源管理：唤醒设备，选择内部时钟
    ICM42670_Write_Reg(ICM42670_PWR_MGMT_0, 0x01);
    delay_us(100);
    
    // 配置陀螺仪：设置满量程和采样率
    ICM42670_Write_Reg(ICM42670_GYRO_CONFIG_0, (GYRO_FS_SEL << 5) | (GYRO_ODR << 2));
    delay_us(100);
    
    // 配置加速度计：设置满量程和采样率
    ICM42670_Write_Reg(ICM42670_ACCEL_CONFIG_0, (ACCEL_FS_SEL << 5) | (ACCEL_ODR << 2));
    delay_us(100);
    
    // 配置低功耗模式：禁用低功耗
    ICM42670_Write_Reg(ICM42670_LP_CONFIG, 0x00);
    delay_us(100);
    
    return 1; // 初始化成功
}

/**
 * @brief  读取 ICM-42670 陀螺仪数据
 * @param  无
 * @retval 无
 */
void ICM42670_Get_Gyro(void)
{
    uint8_t buf[6];
    
    // 读取陀螺仪数据寄存器
    ICM42670_Read_Regs(ICM42670_GYRO_DATA_X1, buf, 6);
    
    // 组合数据
    icm42670_gyro_x = ((int16_t)buf[0] << 8) | buf[1];
    icm42670_gyro_y = ((int16_t)buf[2] << 8) | buf[3];
    icm42670_gyro_z = ((int16_t)buf[4] << 8) | buf[5];
}

/**
 * @brief  读取 ICM-42670 加速度计数据
 * @param  无
 * @retval 无
 */
void ICM42670_Get_Accel(void)
{
    uint8_t buf[6];
    
    // 读取加速度计数据寄存器
    ICM42670_Read_Regs(ICM42670_ACCEL_DATA_X1, buf, 6);
    
    // 组合数据
    icm42670_acc_x = ((int16_t)buf[0] << 8) | buf[1];
    icm42670_acc_y = ((int16_t)buf[2] << 8) | buf[3];
    icm42670_acc_z = ((int16_t)buf[4] << 8) | buf[5];
}

/**
 * @brief  读取 ICM-42670 温度数据
 * @param  无
 * @retval 无
 */
void ICM42670_Get_Temp(void)
{
    uint8_t buf[2];
    
    // 读取温度数据寄存器
    ICM42670_Read_Regs(ICM42670_TEMP_DATA1, buf, 2);
    
    // 组合数据
    icm42670_temp = ((int16_t)buf[0] << 8) | buf[1];
}

/**
 * @brief  读取 ICM-42670 所有数据（陀螺仪、加速度计、温度）
 * @param  无
 * @retval 无
 */
void ICM42670_Get_All(void)
{
    uint8_t buf[14];
    
    // 从陀螺仪 X 轴数据开始读取所有数据
    ICM42670_Read_Regs(ICM42670_GYRO_DATA_X1, buf, 14);
    
    // 组合数据
    icm42670_gyro_x = ((int16_t)buf[0] << 8) | buf[1];
    icm42670_gyro_y = ((int16_t)buf[2] << 8) | buf[3];
    icm42670_gyro_z = ((int16_t)buf[4] << 8) | buf[5];
    
    // 温度数据在陀螺仪和加速度计数据之间
    icm42670_temp = ((int16_t)buf[6] << 8) | buf[7];
    
    icm42670_acc_x = ((int16_t)buf[8] << 8) | buf[9];
    icm42670_acc_y = ((int16_t)buf[10] << 8) | buf[11];
    icm42670_acc_z = ((int16_t)buf[12] << 8) | buf[13];
}

/**
 * @brief  将 ICM-42670 原始数据转换为物理单位
 * @param  无
 * @retval 无
 */
void ICM42670_Data_Unit_Convert(void)
{
    // 转换陀螺仪数据为 °/s
    switch (GYRO_FS_SEL)
    {
        case ICM42670_GYRO_FS_2000DPS:
            icm42670_gyro_x_dps = (float)icm42670_gyro_x / 16.4f;
            icm42670_gyro_y_dps = (float)icm42670_gyro_y / 16.4f;
            icm42670_gyro_z_dps = (float)icm42670_gyro_z / 16.4f;
            break;
        case ICM42670_GYRO_FS_1000DPS:
            icm42670_gyro_x_dps = (float)icm42670_gyro_x / 32.8f;
            icm42670_gyro_y_dps = (float)icm42670_gyro_y / 32.8f;
            icm42670_gyro_z_dps = (float)icm42670_gyro_z / 32.8f;
            break;
        case ICM42670_GYRO_FS_500DPS:
            icm42670_gyro_x_dps = (float)icm42670_gyro_x / 65.5f;
            icm42670_gyro_y_dps = (float)icm42670_gyro_y / 65.5f;
            icm42670_gyro_z_dps = (float)icm42670_gyro_z / 65.5f;
            break;
        case ICM42670_GYRO_FS_250DPS:
            icm42670_gyro_x_dps = (float)icm42670_gyro_x / 131.0f;
            icm42670_gyro_y_dps = (float)icm42670_gyro_y / 131.0f;
            icm42670_gyro_z_dps = (float)icm42670_gyro_z / 131.0f;
            break;
        case ICM42670_GYRO_FS_125DPS:
            icm42670_gyro_x_dps = (float)icm42670_gyro_x / 262.0f;
            icm42670_gyro_y_dps = (float)icm42670_gyro_y / 262.0f;
            icm42670_gyro_z_dps = (float)icm42670_gyro_z / 262.0f;
            break;
        default:
            break;
    }
    
    // 转换加速度计数据为 g
    switch (ACCEL_FS_SEL)
    {
        case ICM42670_ACCEL_FS_16G:
            icm42670_acc_x_g = (float)icm42670_acc_x / 2048.0f;
            icm42670_acc_y_g = (float)icm42670_acc_y / 2048.0f;
            icm42670_acc_z_g = (float)icm42670_acc_z / 2048.0f;
            break;
        case ICM42670_ACCEL_FS_8G:
            icm42670_acc_x_g = (float)icm42670_acc_x / 4096.0f;
            icm42670_acc_y_g = (float)icm42670_acc_y / 4096.0f;
            icm42670_acc_z_g = (float)icm42670_acc_z / 4096.0f;
            break;
        case ICM42670_ACCEL_FS_4G:
            icm42670_acc_x_g = (float)icm42670_acc_x / 8192.0f;
            icm42670_acc_y_g = (float)icm42670_acc_y / 8192.0f;
            icm42670_acc_z_g = (float)icm42670_acc_z / 8192.0f;
            break;
        case ICM42670_ACCEL_FS_2G:
            icm42670_acc_x_g = (float)icm42670_acc_x / 16384.0f;
            icm42670_acc_y_g = (float)icm42670_acc_y / 16384.0f;
            icm42670_acc_z_g = (float)icm42670_acc_z / 16384.0f;
            break;
        default:
            break;
    }
    
    // 转换温度数据为摄氏度
    // 温度计算公式：T(°C) = (TEMP_DATA / 132.48) + 25
    icm42670_temp_c = (float)icm42670_temp / 132.48f + 25.0f;
}

/**
 * @brief  兼容原来的陀螺仪接口，更新陀螺仪数据
 * @param  无
 * @retval 无
 */
void ICM42670_Gryo_Update(void)
{
    // 读取所有数据（陀螺仪、加速度计、温度）
    ICM42670_Get_All();
    
    // 转换数据为物理单位
    ICM42670_Data_Unit_Convert();
    
    // 将数据赋值给兼容变量，使其与原来的函数行为一致
    // 注意：这里需要根据实际硬件安装方向调整轴的映射
    // 以下映射假设 X 轴指向前方，Y 轴指向右方，Z 轴指向上方
    fAcc[0] = icm42670_acc_x_g;    // 加速度 X 轴
    fAcc[1] = icm42670_acc_y_g;    // 加速度 Y 轴
    fAcc[2] = icm42670_acc_z_g;    // 加速度 Z 轴
    
    fGyro[0] = icm42670_gyro_x_dps; // 陀螺仪 X 轴（滚转）
    fGyro[1] = icm42670_gyro_y_dps; // 陀螺仪 Y 轴（俯仰）
    fGyro[2] = icm42670_gyro_z_dps; // 陀螺仪 Z 轴（偏航）
    
    // 计算角度（这里使用简单的积分，实际应用中可能需要更复杂的姿态解算）
    // 注意：这里需要根据实际硬件安装方向调整角度的计算
    static float angle_x = 0, angle_y = 0, angle_z = 0;
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();
    float dt = (current_time - last_time) / 1000.0f; // 时间间隔（秒）
    last_time = current_time;
    
    // 简单的角度积分计算（实际应用中可能需要更复杂的姿态解算）
    angle_x += fGyro[0] * dt;
    angle_y += fGyro[1] * dt;
    angle_z += fGyro[2] * dt;
    
    // 将角度限制在 ±180 度范围内
    if (angle_x > 180) angle_x -= 360;
    else if (angle_x < -180) angle_x += 360;
    
    if (angle_y > 180) angle_y -= 360;
    else if (angle_y < -180) angle_y += 360;
    
    if (angle_z > 180) angle_z -= 360;
    else if (angle_z < -180) angle_z += 360;
    
    fAngle[0] = angle_x; // 滚转角（Roll）
    fAngle[1] = angle_y; // 俯仰角（Pitch）
    fAngle[2] = angle_z; // 偏航角（Yaw）
    
    fYaw = angle_z; // 偏航角（与 fAngle[2] 相同）
}
