#ifndef _ICM42670_H
#define _ICM42670_H

#include "stdint.h"
#include "hsc.h"
#include "soft_iic.h"

// ICM-42670 I2C 设备地址（7位地址，不包含R/W位）
// SA0 接地时为 0x68，接高时为 0x69
// 当前PC8(SA0)默认为高电平
#define ICM42670_I2C_ADDR                 0x69

// ICM-42670 引脚定义
#define ICM42670_SCL_PORT                 GPIOA
#define ICM42670_SCL_PIN                  GPIO_PIN_8
#define ICM42670_SDA_PORT                 GPIOC
#define ICM42670_SDA_PIN                  GPIO_PIN_9
#define ICM42670_SA0_PORT                 GPIOC
#define ICM42670_SA0_PIN                  GPIO_PIN_8
#define ICM42670_CS_PORT                  GPIOC
#define ICM42670_CS_PIN                   GPIO_PIN_6

// ICM-42670 寄存器地址
#define ICM42670_WHO_AM_I                 0x0F    // 设备 ID 寄存器
#define ICM42670_DEVICE_CONFIG            0x11    // 设备配置寄存器
#define ICM42670_GYRO_CONFIG_0            0x14    // 陀螺仪配置 0
#define ICM42670_ACCEL_CONFIG_0           0x15    // 加速度计配置 0
#define ICM42670_LP_CONFIG                0x16    // 低功耗配置
#define ICM42670_TEMP_DATA1               0x1D    // 温度数据高字节
#define ICM42670_TEMP_DATA0               0x1E    // 温度数据低字节
#define ICM42670_GYRO_DATA_X1             0x21    // 陀螺仪 X 轴数据高字节
#define ICM42670_GYRO_DATA_X0             0x22    // 陀螺仪 X 轴数据低字节
#define ICM42670_GYRO_DATA_Y1             0x23    // 陀螺仪 Y 轴数据高字节
#define ICM42670_GYRO_DATA_Y0             0x24    // 陀螺仪 Y 轴数据低字节
#define ICM42670_GYRO_DATA_Z1             0x25    // 陀螺仪 Z 轴数据高字节
#define ICM42670_GYRO_DATA_Z0             0x26    // 陀螺仪 Z 轴数据低字节
#define ICM42670_ACCEL_DATA_X1            0x29    // 加速度计 X 轴数据高字节
#define ICM42670_ACCEL_DATA_X0            0x2A    // 加速度计 X 轴数据低字节
#define ICM42670_ACCEL_DATA_Y1            0x2B    // 加速度计 Y 轴数据高字节
#define ICM42670_ACCEL_DATA_Y0            0x2C    // 加速度计 Y 轴数据低字节
#define ICM42670_ACCEL_DATA_Z1            0x2D    // 加速度计 Z 轴数据高字节
#define ICM42670_ACCEL_DATA_Z0            0x2E    // 加速度计 Z 轴数据低字节
#define ICM42670_PWR_MGMT_0               0x4E    // 电源管理 0

// 陀螺仪满量程选项
#define ICM42670_GYRO_FS_2000DPS          0x00    // ±2000 dps
#define ICM42670_GYRO_FS_1000DPS          0x01    // ±1000 dps
#define ICM42670_GYRO_FS_500DPS           0x02    // ±500 dps
#define ICM42670_GYRO_FS_250DPS           0x03    // ±250 dps
#define ICM42670_GYRO_FS_125DPS           0x04    // ±125 dps

// 加速度计满量程选项
#define ICM42670_ACCEL_FS_16G             0x00    // ±16 g
#define ICM42670_ACCEL_FS_8G              0x01    // ±8 g
#define ICM42670_ACCEL_FS_4G              0x02    // ±4 g
#define ICM42670_ACCEL_FS_2G              0x03    // ±2 g

// 采样率选项 (Hz)
#define ICM42670_ODR_1000                 0x00    // 1000 Hz
#define ICM42670_ODR_500                  0x01    // 500 Hz
#define ICM42670_ODR_250                  0x02    // 250 Hz
#define ICM42670_ODR_125                  0x03    // 125 Hz
#define ICM42670_ODR_62_5                 0x04    // 62.5 Hz
#define ICM42670_ODR_31_25                0x05    // 31.25 Hz
#define ICM42670_ODR_15_625               0x06    // 15.625 Hz
#define ICM42670_ODR_7_8125               0x07    // 7.8125 Hz

// 全局变量
extern int16_t icm42670_gyro_x, icm42670_gyro_y, icm42670_gyro_z;
extern int16_t icm42670_acc_x, icm42670_acc_y, icm42670_acc_z;
extern int16_t icm42670_temp;

extern float icm42670_gyro_x_dps, icm42670_gyro_y_dps, icm42670_gyro_z_dps;
extern float icm42670_acc_x_g, icm42670_acc_y_g, icm42670_acc_z_g;
extern float icm42670_temp_c;

// 函数声明
u8 ICM42670_Init(void);
u8 ICM42670_Check(void);
void GPIO_Test(void);  // GPIO电平测试
void I2C_Scan(void);  // I2C设备扫描测试
void ICM42670_Get_Gyro(void);
void ICM42670_Get_Accel(void);
void ICM42670_Get_Temp(void);
void ICM42670_Get_All(void);
void ICM42670_Data_Unit_Convert(void);

// 兼容原来的陀螺仪接口
extern float fAcc[3], fGyro[3], fAngle[3], fYaw;
void ICM42670_Gryo_Update(void);

#endif
