# 菜单系统使用说明

## 1. 系统概述

本菜单系统是为 STM32F401 开发板设计的，支持通过 EC11 旋转编码器进行菜单导航和信息页面显示。系统提供了主菜单、陀螺仪信息页面和 EC11 测试页面，用户可以通过旋转编码器和按键进行操作。

## 2. 文件结构

```
App/hsc_Lib/2_Control/
├── menu.c          # 菜单核心实现
├── menu.h          # 菜单接口定义
├── menu_freertos.c # FreeRTOS 适配例程
├── menu_freertos.h # FreeRTOS 适配接口
└── MENU_README.md  # 本说明文件
```

## 3. 功能说明

### 3.1 主菜单

- 显示 "Main Menu" 标题
- 包含两个菜单项：
  - "Gyro Info" - 陀螺仪信息页面
  - "EC11 Test" - EC11 测试页面
- 显示操作提示："Short: Select" 和 "Long: Back"

### 3.2 陀螺仪信息页面

- 显示 "Gyro Info" 标题
- 显示加速度数据（Acc X, Acc Y, Acc Z）
- 显示角速度数据（Gyro X, Gyro Y, Gyro Z）
- 显示操作提示："Long press to back"

### 3.3 EC11 测试页面

- 显示 "EC11 Test" 标题
- 显示编码器计数值
- 显示按键状态（R-释放，P-按下）
- 显示操作提示："Turn me!" 和 "Long press to back"

## 4. 操作方法

### 4.1 编码器操作

- **顺时针旋转**：在主菜单中选择下一个菜单项
- **逆时针旋转**：在主菜单中选择上一个菜单项

### 4.2 按键操作

- **短按**：在主菜单中选择当前菜单项，进入相应的信息页面
- **长按**：从信息页面返回主菜单

## 5. 系统集成

### 5.1 基本集成

1. 在 `app_main.c` 中包含菜单头文件：
   ```c
   #include "menu.h"
   ```

2. 在 `App_Init()` 函数中初始化菜单系统：
   ```c
   Menu_Init();
   ```

3. 在 `App_Loop()` 函数中运行菜单系统：
   ```c
   while (1) {
       Menu_Run();
   }
   ```

### 5.2 FreeRTOS 适配

1. 在 `app_main.c` 中包含 FreeRTOS 菜单头文件：
   ```c
   #include "menu_freertos.h"
   ```

2. 在 `App_Init()` 函数中初始化 FreeRTOS 菜单系统：
   ```c
   Menu_FreeRTOS_Init();
   ```

3. 在 `main()` 函数中启动 FreeRTOS 调度器：
   ```c
   Menu_FreeRTOS_Start();
   ```

## 6. 代码说明

### 6.1 菜单数据结构

- `MenuPageType`：菜单页面类型枚举
- `MenuItem`：菜单项结构，包含名称和对应的页面
- `MenuState`：菜单状态结构，包含当前页面、当前菜单项等信息

### 6.2 核心函数

- `Menu_Init()`：菜单系统初始化
- `Menu_Run()`：菜单系统运行，处理输入和显示
- `Menu_HandleEncoder()`：处理编码器输入
- `Menu_HandleKeyEvent()`：处理按键事件
- `Menu_DisplayCurrentPage()`：显示当前页面

### 6.3 FreeRTOS 适配函数

- `Menu_FreeRTOS_Init()`：FreeRTOS 菜单系统初始化
- `Menu_FreeRTOS_Start()`：启动 FreeRTOS 调度器
- `Menu_FreeRTOS_Stop()`：停止 FreeRTOS 菜单系统

## 7. 任务说明

### 7.1 基本任务

| 任务 | 优先级 | 栈大小 | 功能 |
|------|--------|--------|------|
| 菜单任务 | 低 | 最小栈大小 * 2 | 处理用户输入和菜单导航 |

### 7.2 FreeRTOS 任务

| 任务 | 优先级 | 栈大小 | 功能 |
|------|--------|--------|------|
| MenuTask | 中 | 最小栈大小 * 2 | 处理用户输入和菜单导航 |
| DisplayTask | 低 | 最小栈大小 * 2 | 负责 OLED 显示更新 |
| SensorTask | 低 | 最小栈大小 * 2 | 读取陀螺仪数据 |

## 8. 扩展方法

### 8.1 添加新的菜单项

1. 在 `menu.c` 中修改 `mainMenuItems` 数组，添加新的菜单项：
   ```c
   static const MenuItem mainMenuItems[] = {
       {"Gyro Info", MENU_PAGE_GYRO},
       {"EC11 Test", MENU_PAGE_EC11_TEST},
       {"New Item", MENU_PAGE_NEW} // 新菜单项
   };
   ```

2. 在 `menu.h` 中添加新的页面类型：
   ```c
   typedef enum {
       MENU_PAGE_MAIN,      // 主菜单
       MENU_PAGE_GYRO,      // 陀螺仪信息页面
       MENU_PAGE_EC11_TEST, // EC11测试页面
       MENU_PAGE_NEW        // 新页面
   } MenuPageType;
   ```

3. 在 `menu.c` 中添加新的页面显示函数：
   ```c
   static void Menu_DisplayNewPage(void) {
       OLED_Clear(0);
       OLED_ShowString(0, 0, "New Page", 16);
       // 显示内容
       OLED_Refresh();
   }
   ```

4. 在 `Menu_DisplayCurrentPage()` 函数中添加新页面的处理：
   ```c
   void Menu_DisplayCurrentPage(void) {
       switch (menuState.currentPage) {
           case MENU_PAGE_MAIN:
               Menu_DisplayMainMenu();
               break;
           case MENU_PAGE_GYRO:
               Menu_DisplayGyroPage();
               break;
           case MENU_PAGE_EC11_TEST:
               Menu_DisplayEc11TestPage();
               break;
           case MENU_PAGE_NEW:
               Menu_DisplayNewPage();
               break;
           default:
               Menu_DisplayMainMenu();
               break;
       }
   }
   ```

### 8.2 修改菜单样式

可以通过修改 `Menu_DisplayMainMenu()`、`Menu_DisplayGyroPage()` 和 `Menu_DisplayEc11TestPage()` 函数来修改菜单的显示样式，包括字体大小、位置和内容等。

## 9. 注意事项

1. 确保正确初始化 OLED 和编码器驱动
2. 确保 `TIM1_100us_Callback()` 函数中调用了 `Encoder_TIM_Callback()`
3. 使用 FreeRTOS 适配时，确保已正确配置 FreeRTOS
4. 如需添加新的传感器或功能，可参考传感器任务的实现方式

## 10. 故障排除

### 10.1 菜单不显示

- 检查 OLED 初始化是否正确
- 检查 `Menu_Init()` 函数是否被调用
- 检查 `Menu_Run()` 函数是否在主循环中被调用

### 10.2 编码器不响应

- 检查编码器初始化是否正确
- 检查 `Encoder_TIM_Callback()` 函数是否在定时器中断中被调用
- 检查编码器硬件连接是否正确

### 10.3 按键不响应

- 检查按键硬件连接是否正确
- 检查 `Encoder_Key_GetEvent()` 函数是否被正确调用
- 检查按键消抖参数是否合适

## 11. 版本历史

| 版本 | 日期 | 描述 |
|------|------|------|
| v1.0 | 2026-02-09 | 初始版本，实现基本菜单功能和 FreeRTOS 适配 |
