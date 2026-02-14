#include "TOF_VL53L0X.h"
#include "soft_iic.h"
#include <stdio.h>
#include <string.h>

// I2C 实例
static soft_iic_t tof_iic;

// Xshut 引脚控制 - 使用 PC6
#define VL53L0X_Xshut(n)  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, (n)?GPIO_PIN_SET:GPIO_PIN_RESET)

// INT 引脚读取 - 使用 PC8
#define VL53L0X_INT()     HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

// 寄存器定义
#define REG_IDENTIFICATION_MODEL_ID     0xC0
#define REG_IDENTIFICATION_REVISION_ID  0xC2
#define REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define REG_SYSRANGE_START              0x00
#define REG_SYSTEM_SEQUENCE_CONFIG      0x01
#define REG_RESULT_INTERRUPT_STATUS     0x13
#define REG_RESULT_RANGE_STATUS         0x14
#define REG_I2C_SLAVE_DEVICE_ADDRESS    0x8A
#define REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV  0x89

// 从NVM读取校准数据 - 可选实现
#define NVM_REF_SPAD_OFFSET             0xC0  // SPAD 校准数据偏移
#define NVM_PART_TO_PART_RANGE_OFFSET   0xF8  // 部件间校准偏移

static void VL_WriteByte(u8 reg, u8 dat)
{
    soft_iic_write_reg(&tof_iic, reg, dat);
}

static u8 VL_ReadByte(u8 reg)
{
    return soft_iic_read_reg(&tof_iic, reg);
}

static u16 VL_ReadWord(u8 reg)
{
    return soft_iic_read_reg16(&tof_iic, reg);
}

static void VL_WriteWord(u8 reg, u16 dat)
{
    soft_iic_write_reg16(&tof_iic, reg, dat);
}

static void VL_WriteDWord(u8 reg, u32 dat)
{
    soft_iic_start(&tof_iic);
    soft_iic_send_byte(&tof_iic, (tof_iic.addr << 1) & 0xFE);
    if (soft_iic_wait_ack(&tof_iic)) { soft_iic_stop(&tof_iic); return; }
    soft_iic_send_byte(&tof_iic, reg);
    if (soft_iic_wait_ack(&tof_iic)) { soft_iic_stop(&tof_iic); return; }
    soft_iic_send_byte(&tof_iic, (dat >> 24) & 0xFF);
    soft_iic_wait_ack(&tof_iic);
    soft_iic_send_byte(&tof_iic, (dat >> 16) & 0xFF);
    soft_iic_wait_ack(&tof_iic);
    soft_iic_send_byte(&tof_iic, (dat >> 8) & 0xFF);
    soft_iic_wait_ack(&tof_iic);
    soft_iic_send_byte(&tof_iic, dat & 0xFF);
    soft_iic_wait_ack(&tof_iic);
    soft_iic_stop(&tof_iic);
}

/* 修改 I2C 地址 */
static void VL_SetAddress(u8 new_addr)
{
    VL_WriteByte(REG_I2C_SLAVE_DEVICE_ADDRESS, new_addr >> 1);
    delay_ms(10);
}

/* 硬件复位 */
static void VL_Reset(void)
{
    VL53L0X_Xshut(0);
    delay_ms(100);  // 保持 100ms
    VL53L0X_Xshut(1);
    delay_ms(150);  // 启动等待
}

/* 数据初始化 - 配置设备基本参数 */
static u8 VL_DataInit(void)
{
    u8 val;
    
    // 配置 2.8V 电压调节器 - 可选
    // 如果使用 2.8V 电源，可以注释掉
    // VL_WriteByte(REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, 
    //              VL_ReadByte(REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV) | 0x01);
    
    // 配置 I2C 速率
    VL_WriteByte(0x88, 0x00);
    
    // 清除内部状态
    VL_WriteByte(REG_SYSRANGE_START, 0x01);
    delay_ms(10);
    VL_WriteByte(REG_SYSRANGE_START, 0x00);
    delay_ms(10);
    
    // 复位信号
    VL_WriteByte(0x80, 0x01);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x00, 0x00);
    VL_WriteByte(0x91, VL_ReadByte(0x91));  // 读回值
    VL_WriteByte(0x00, 0x01);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x80, 0x00);
    
    return 0;
}

/* 静态初始化 - 加载校准数据 */
static u8 VL_StaticInit(void)
{
    u8 spad_count = 0;
    u8 spad_type_aperture = 0;
    u8 ref_spad_map[6] = {0};
    u8 first_spad = 0;
    u8 spads_enabled = 0;
    u8 i = 0;
    
    // 从 NVM 读取参考 SPAD 数据 - 0xC0开始
    // 这里简化处理，实际应该读取完整的 NVM
    // 并进行相应的校准计算
    
    // 读取设备信息
    u8 val1 = VL_ReadByte(0xC0);
    u8 val2 = VL_ReadByte(0xC1);
    printf("NVM SPAD Info: 0xC0=0x%02X, 0xC1=0x%02X\r\n", val1, val2);
    
    // 设置 SPAD 配置 - 基于ST参考代码
    // 大多数 VL53L0X 有 12 个 Aperture SPAD
    
    // 写入参考 SPAD 配置
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x4F, 0x00);  // 参考 SPAD 起始偏移
    VL_WriteByte(0x4E, 0x2C);  // SPAD 数量设置为44个
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0xB6, 0xB4);  // 参考 SPAD 使能
    
    // 写入 SPAD 映射 - 基于ST API
    u8 spad_map[6] = {0xCE, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF};
    for (i = 0; i < 6; i++) {
        VL_WriteByte(0xB0 + i, spad_map[i]);
    }
    
    // 加载 ST API 推荐的配置
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x00, 0x00);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x09, 0x04);
    VL_WriteByte(0x10, 0x00);
    VL_WriteByte(0x11, 0x00);
    VL_WriteByte(0x24, 0x01);
    VL_WriteByte(0x25, 0xFF);
    VL_WriteByte(0x75, 0x00);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x4E, 0x2C);
    VL_WriteByte(0x48, 0x00);
    VL_WriteByte(0x30, 0x20);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x30, 0x09);
    VL_WriteByte(0x54, 0x00);
    VL_WriteByte(0x31, 0x04);
    VL_WriteByte(0x32, 0x03);
    VL_WriteByte(0x40, 0x83);
    VL_WriteByte(0x46, 0x25);
    VL_WriteByte(0x60, 0x00);
    VL_WriteByte(0x27, 0x00);
    VL_WriteByte(0x50, 0x06);
    VL_WriteByte(0x51, 0x00);
    VL_WriteByte(0x52, 0x96);
    VL_WriteByte(0x56, 0x08);
    VL_WriteByte(0x57, 0x30);
    VL_WriteByte(0x61, 0x00);
    VL_WriteByte(0x62, 0x00);
    VL_WriteByte(0x64, 0x00);
    VL_WriteByte(0x65, 0x00);
    VL_WriteByte(0x66, 0xA0);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x22, 0x32);
    VL_WriteByte(0x47, 0x14);
    VL_WriteByte(0x49, 0xFF);
    VL_WriteByte(0x4A, 0x00);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x7A, 0x0A);
    VL_WriteByte(0x7B, 0x00);
    VL_WriteByte(0x78, 0x21);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x23, 0x34);
    VL_WriteByte(0x42, 0x00);
    VL_WriteByte(0x44, 0xFF);
    VL_WriteByte(0x45, 0x26);
    VL_WriteByte(0x46, 0x05);
    VL_WriteByte(0x40, 0x40);
    VL_WriteByte(0x0E, 0x06);
    VL_WriteByte(0x20, 0x1A);
    VL_WriteByte(0x43, 0x40);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x34, 0x03);
    VL_WriteByte(0x35, 0x44);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x31, 0x04);
    VL_WriteByte(0x4B, 0x09);
    VL_WriteByte(0x4C, 0x05);
    VL_WriteByte(0x4D, 0x04);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x44, 0x00);
    VL_WriteByte(0x45, 0x20);
    VL_WriteByte(0x47, 0x08);
    VL_WriteByte(0x48, 0x28);
    VL_WriteByte(0x67, 0x00);
    VL_WriteByte(0x70, 0x04);
    VL_WriteByte(0x71, 0x01);
    VL_WriteByte(0x72, 0xFE);
    VL_WriteByte(0x76, 0x00);
    VL_WriteByte(0x77, 0x00);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x0D, 0x01);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x80, 0x01);
    VL_WriteByte(0x01, 0xF8);
    VL_WriteByte(0xFF, 0x01);
    VL_WriteByte(0x8E, 0x01);
    VL_WriteByte(0x00, 0x01);
    VL_WriteByte(0xFF, 0x00);
    VL_WriteByte(0x80, 0x00);
    
    // 配置 GPIO
    VL_WriteByte(0x0A, 0x04);
    VL_WriteByte(0x84, 0x00);
    VL_WriteByte(0x0B, 0x01);
    
    return 0;
}

/* 设置测量时序预算 */
static u8 VL_SetMeasurementTimingBudgetMicroSeconds(u32 budget_us)
{
    // 简化实现 - 固定时序
    // 预范围 VCSEL 周期
    VL_WriteByte(REG_PRE_RANGE_CONFIG_VCSEL_PERIOD, 0x0A);
    // 最终范围 VCSEL 周期  
    VL_WriteByte(REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD, 0x08);
    
    // 设置超时
    VL_WriteByte(0x1A, 0x0B);  // 最终范围超时
    VL_WriteByte(0x1B, 0x00);
    
    return 0;
}

/* 执行参考校准 */
static u8 VL_PerformRefCalibration(void)
{
    u8 val;
    
    // VHV 校准
    VL_WriteByte(0x01, 0x01);  // SYSRANGE_START: VHV 校准
    delay_ms(100);
    do {
        val = VL_ReadByte(0x00);
    } while (val & 0x01);
    
    // Phase 校准
    VL_WriteByte(0x01, 0x02);  // SYSRANGE_START: Phase 校准
    delay_ms(100);
    do {
        val = VL_ReadByte(0x00);
    } while (val & 0x01);
    
    return 0;
}

/* 配置连续测量 */
static void VL_ConfigureContinuous(void)
{
    // 配置连续测量模式，100ms间隔
    VL_WriteWord(0x01, 0x0064);  // 100ms 间隔
}

/* 传感器初始化 */
u8 VL53L0X_Init(void)
{
    u8 id;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能 GPIOC 时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // 初始化 Xshut 引脚 (PC6) - 输出模式
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // 初始化 INT 引脚 (PC8) - 输入模式
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // 硬件复位
    VL_Reset();
    
    // 初始化软件 IIC
    soft_iic_init(&tof_iic, GPIOC, GPIO_PIN_9, GPIOA, GPIO_PIN_8, 0x29, 5);
    delay_ms(10);

    // 验证芯片 ID
    id = VL_ReadByte(REG_IDENTIFICATION_MODEL_ID);
    if(id != 0xEE) {
        printf("TOF_Init: Chip ID error: 0x%02X\r\n", id);
        return 1;
    }
    printf("TOF_Init: Chip ID OK: 0x%02X\r\n", id);

    // 数据初始化
    if (VL_DataInit() != 0) {
        printf("TOF_Init: Data init failed\r\n");
        return 2;
    }
    
    // 静态初始化
    if (VL_StaticInit() != 0) {
        printf("TOF_Init: Static init failed\r\n");
        return 3;
    }
    
    // 设置 33ms 测量时序预算
    if (VL_SetMeasurementTimingBudgetMicroSeconds(33000) != 0) {
        printf("TOF_Init: Timing budget failed\r\n");
        return 4;
    }
    
    // 参考校准
    VL_PerformRefCalibration();
    
    // 配置连续测量
    VL_ConfigureContinuous();
    
    // 序列配置
    VL_WriteByte(REG_SYSTEM_SEQUENCE_CONFIG, 0xE8);
    delay_ms(10);
    
    printf("TOF_Init: Initialization completed\r\n");
    return 0;
}

/* 单次测距 - 同步模式 */
u16 VL53L0X_ReadDistance(void)
{
    u16 dist = 0xFFFF;
    u16 timeout = 1000;  // 10秒超时(1000 * 10ms)
    u8 status;
    
    // 清除内部状态
    VL_WriteByte(REG_SYSRANGE_START, 0x01);
    delay_ms(10);
    VL_WriteByte(REG_SYSRANGE_START, 0x00);
    delay_ms(10);
    
    // 清除中断
    VL_WriteByte(0x0B, 0x01);
    delay_ms(5);
    
    // 启动单次测距(bit 0 = 1)
    VL_WriteByte(REG_SYSRANGE_START, 0x01);
    
    // 等待测量完成 - 检查中断
    do {
        delay_ms(10);
        status = VL_ReadByte(REG_RESULT_INTERRUPT_STATUS);
        if ((status & 0x07) != 0) break;  // 有测量结果
    } while (--timeout);
    
    if (timeout == 0) {
        printf("VL53L0X: Measurement timeout\r\n");
        VL_WriteByte(0x0B, 0x01);
        return 0xFFFF;
    }
    
    // 读取距离(RESULT_RANGE_STATUS + 10处)
    dist = VL_ReadWord(REG_RESULT_RANGE_STATUS + 10);
    
    // 清除中断
    VL_WriteByte(0x0B, 0x01);
    
    // 检查有效性
    if (dist == 0 || dist > 8190) {
        printf("VL53L0X: Invalid distance %d mm (status=0x%02X)\r\n", dist, status);
        return 0xFFFF;
    }
    
    printf("VL53L0X: Distance = %d mm\r\n", dist);
    return dist;
}

/* 检查 TOF 是否就绪 */
u8 VL53L0X_IsReady(void)
{
    u8 id = VL_ReadByte(REG_IDENTIFICATION_MODEL_ID);
    return (id == 0xEE) ? 1 : 0;
}

/* 读取 INT 引脚状态 */
u8 VL53L0X_ReadINT(void)
{
    return VL53L0X_INT();
}

/**
 * @brief TOF 快速测试函数
 */
u16 TOF_QuickTest(void)
{
    return VL53L0X_ReadDistance();
}

/**
 * @brief TOF 诊断测试函数
 */
u8 TOF_DiagnosticTest(void)
{
    u8 result = 0;
    u8 id = 0;
    u16 distance = 0;
    u8 i;
    
    printf("\r\n=== TOF VL53L0X 诊断测试 ===\r\n");
    
    // 1. 硬件复位测试
    printf("1. 硬件复位测试...\r\n");
    VL_Reset();
    printf("   [OK] 硬件复位完成\r\n");
    
    // 2. I2C 通信测试
    printf("2. I2C 通信测试...\r\n");
    id = VL_ReadByte(REG_IDENTIFICATION_MODEL_ID);
    printf("   芯片 ID: 0x%02X ", id);
    if (id == 0xEE) {
        printf("[OK] 正确\r\n");
    } else {
        printf("[ERR] 错误 (应为 0xEE)\r\n");
        result = 1;
    }
    
    // 3. 版本信息
    printf("3. 版本信息: Rev=0x%02X\r\n", VL_ReadByte(REG_IDENTIFICATION_REVISION_ID));
    
    // 4. 寄存器读写测试
    printf("4. 寄存器读写测试...\r\n");
    VL_WriteByte(0x75, 0x55);
    delay_ms(5);
    u8 read_val = VL_ReadByte(0x75);
    printf("   写入: 0x55, 读取: 0x%02X %s\r\n", read_val, 
           (read_val == 0x55) ? "[OK]" : "[ERR]");
    
    // 5. 配置状态检查
    printf("5. 配置状态检查:\r\n");
    printf("   序列配置: 0x%02X\r\n", VL_ReadByte(REG_SYSTEM_SEQUENCE_CONFIG));
    printf("   预范围VCSEL: 0x%02X\r\n", VL_ReadByte(REG_PRE_RANGE_CONFIG_VCSEL_PERIOD));
    printf("   最终范围VCSEL: 0x%02X\r\n", VL_ReadByte(REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD));
    
    // 6. 单次测距测试
    printf("6. 单次测距测试...\r\n");
    
    // 清除中断
    VL_WriteByte(0x0B, 0x01);
    delay_ms(10);
    
    // 启动测量
    printf("   启动测量...\r\n");
    VL_WriteByte(REG_SYSRANGE_START, 0x01);
    
    // 等待测量完成
    u16 timeout = 500;
    u8 int_status = 0;
    u8 range_status = 0;
    
    while (timeout--) {
        delay_ms(10);
        int_status = VL_ReadByte(REG_RESULT_INTERRUPT_STATUS);
        range_status = VL_ReadByte(REG_RESULT_RANGE_STATUS);
        
        if ((int_status & 0x07) != 0) {
            printf("   [OK] 测量完成 (int=0x%02X, range=0x%02X)\r\n", int_status, range_status);
            break;
        }
    }
    
    if (timeout == 0) {
        printf("   [ERR] 测量超时\r\n");
        result = 1;
    } else {
        // 读取距离
        distance = VL_ReadWord(REG_RESULT_RANGE_STATUS + 10);
        printf("   测量距离: %d mm\r\n", distance);
        
        // 分析结果
        printf("   测量结果分析: ");
        u8 result_type = int_status & 0x07;
        switch(result_type) {
            case 1: printf("Range valid\r\n"); break;
            case 2: printf("Sigma fail\r\n"); break;
            case 3: printf("Signal fail\r\n"); break;
            case 4: printf("Min range fail\r\n"); break;
            case 5: printf("Phase fail\r\n"); break;
            case 6: printf("Hardware fail\r\n"); break;
            default: printf("No result (0x%02X)\r\n", int_status); break;
        }
        
        if (int_status & 0x40) printf("   [WARN] Wrap around detected\r\n");
        if (int_status & 0x20) printf("   [WARN] Signal fail detected\r\n");
        
        if (distance > 0 && distance < 8190) {
            printf("   [OK] 距离有效: %d mm\r\n", distance);
        } else {
            printf("   [ERR] 距离无效\r\n");
            result = 1;
        }
    }
    
    // 清除中断
    VL_WriteByte(0x0B, 0x01);
    
    // 7. 连续测距测试
    printf("7. 连续测距测试 (3次)...\r\n");
    u8 success_count = 0;
    for (i = 0; i < 3; i++) {
        distance = VL53L0X_ReadDistance();
        if (distance != 0xFFFF) {
            printf("   [%d] %d mm [OK]\r\n", i+1, distance);
            success_count++;
        } else {
            printf("   [%d] 失败\r\n", i+1);
        }
        delay_ms(200);
    }
    
    printf("   成功率: %d/3\r\n", success_count);
    if (success_count == 0) result = 1;
    
    printf("\r\n=== 测试 %s ===\r\n", result ? "失败" : "成功");
    return result;
}

/**
 * @brief 读取 TOF 详细状态信息
 */
void TOF_ReadStatusInfo(void)
{
    printf("\r\n=== TOF 状态信息 ===\r\n");
    printf("INT引脚: %s\r\n", VL53L0X_ReadINT() ? "高" : "低");
    printf("中断状态: 0x%02X\r\n", VL_ReadByte(REG_RESULT_INTERRUPT_STATUS));
    printf("测距状态: 0x%02X\r\n", VL_ReadByte(REG_RESULT_RANGE_STATUS));
    printf("系统范围: 0x%02X\r\n", VL_ReadByte(REG_SYSRANGE_START));
    printf("序列配置: 0x%02X\r\n", VL_ReadByte(REG_SYSTEM_SEQUENCE_CONFIG));
    printf("芯片ID: 0x%02X\r\n", VL_ReadByte(REG_IDENTIFICATION_MODEL_ID));
}
