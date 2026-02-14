/**
 * @file menu_freertos.c
 * @brief FreeRTOS 菜单系统适配例程
 * @note 展示如何在多任务环境下使用菜单系统
 */

#include "menu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// 任务优先级
#define MENU_TASK_PRIORITY           (tskIDLE_PRIORITY + 2)
#define DISPLAY_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define SENSOR_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)

// 任务栈大小
#define MENU_TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE * 2)
#define DISPLAY_TASK_STACK_SIZE      (configMINIMAL_STACK_SIZE * 2)
#define SENSOR_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE * 2)

// 队列和信号量
static QueueHandle_t xMenuEventQueue = NULL;
static SemaphoreHandle_t xDisplaySemaphore = NULL;
static SemaphoreHandle_t xSensorSemaphore = NULL;

// 任务句柄
static TaskHandle_t xMenuTaskHandle = NULL;
static TaskHandle_t xDisplayTaskHandle = NULL;
static TaskHandle_t xSensorTaskHandle = NULL;

// 事件类型
typedef enum {
    MENU_EVENT_ENCODER,    // 编码器事件
    MENU_EVENT_KEY,        // 按键事件
    MENU_EVENT_PAGE_CHANGE // 页面切换事件
} MenuEventType;

// 事件结构
typedef struct {
    MenuEventType type;     // 事件类型
    union {
        int32_t encoderCount; // 编码器计数
        uint8_t keyEvent;     // 按键事件
        MenuPageType page;    // 页面类型
    } data;
} MenuEvent;

/**
 * @brief 菜单任务
 * @param pvParameters 任务参数
 */
static void MenuTask(void *pvParameters)
{
    MenuEvent event;
    int32_t lastEncoderCount = 0;
    
    // 初始化菜单系统
    Menu_Init();
    
    while (1) {
        // 获取编码器计数
        int32_t encoderCount = Encoder_Get_Count();
        
        // 处理编码器输入
        if (encoderCount != lastEncoderCount) {
            event.type = MENU_EVENT_ENCODER;
            event.data.encoderCount = encoderCount;
            xQueueSend(xMenuEventQueue, &event, 0);
            lastEncoderCount = encoderCount;
        }
        
        // 获取按键事件
        uint8_t keyEvent = Encoder_Key_GetEvent();
        if (keyEvent != 0) {
            event.type = MENU_EVENT_KEY;
            event.data.keyEvent = keyEvent;
            xQueueSend(xMenuEventQueue, &event, 0);
        }
        
        // 处理队列中的事件
        if (xQueueReceive(xMenuEventQueue, &event, 10) == pdTRUE) {
            switch (event.type) {
                case MENU_EVENT_ENCODER:
                    Menu_HandleEncoder(event.data.encoderCount);
                    // 通知显示任务更新
                    xSemaphoreGive(xDisplaySemaphore);
                    break;
                case MENU_EVENT_KEY:
                    Menu_HandleKeyEvent(event.data.keyEvent);
                    // 通知显示任务更新
                    xSemaphoreGive(xDisplaySemaphore);
                    break;
                case MENU_EVENT_PAGE_CHANGE:
                    // 页面切换事件处理
                    xSemaphoreGive(xDisplaySemaphore);
                    break;
                default:
                    break;
            }
        }
        
        // 任务延迟
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/**
 * @brief 显示任务
 * @param pvParameters 任务参数
 */
static void DisplayTask(void *pvParameters)
{
    while (1) {
        // 等待显示信号
        if (xSemaphoreTake(xDisplaySemaphore, portMAX_DELAY) == pdTRUE) {
            // 显示当前页面
            Menu_DisplayCurrentPage();
        }
    }
}

/**
 * @brief 传感器任务
 * @param pvParameters 任务参数
 */
static void SensorTask(void *pvParameters)
{
    while (1) {
        // 等待传感器信号
        if (xSemaphoreTake(xSensorSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
            // 更新陀螺仪数据
            Gryo_Update();
        }
        
        // 定期更新陀螺仪数据
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief FreeRTOS 菜单系统初始化
 */
void Menu_FreeRTOS_Init(void)
{
    // 创建队列和信号量
    xMenuEventQueue = xQueueCreate(10, sizeof(MenuEvent));
    xDisplaySemaphore = xSemaphoreCreateBinary();
    xSensorSemaphore = xSemaphoreCreateBinary();
    
    // 创建任务
    xTaskCreate(
        MenuTask,
        "MenuTask",
        MENU_TASK_STACK_SIZE,
        NULL,
        MENU_TASK_PRIORITY,
        &xMenuTaskHandle
    );
    
    xTaskCreate(
        DisplayTask,
        "DisplayTask",
        DISPLAY_TASK_STACK_SIZE,
        NULL,
        DISPLAY_TASK_PRIORITY,
        &xDisplayTaskHandle
    );
    
    xTaskCreate(
        SensorTask,
        "SensorTask",
        SENSOR_TASK_STACK_SIZE,
        NULL,
        SENSOR_TASK_PRIORITY,
        &xSensorTaskHandle
    );
    
    // 初始通知显示任务
    xSemaphoreGive(xDisplaySemaphore);
}

/**
 * @brief FreeRTOS 菜单系统启动
 */
void Menu_FreeRTOS_Start(void)
{
    // 启动 FreeRTOS 调度器
    // 注意：此函数应在 main() 中调用，而不是在此文件中
    // vTaskStartScheduler();
}

/**
 * @brief FreeRTOS 菜单系统停止
 */
void Menu_FreeRTOS_Stop(void)
{
    // 删除任务
    if (xMenuTaskHandle != NULL) {
        vTaskDelete(xMenuTaskHandle);
        xMenuTaskHandle = NULL;
    }
    
    if (xDisplayTaskHandle != NULL) {
        vTaskDelete(xDisplayTaskHandle);
        xDisplayTaskHandle = NULL;
    }
    
    if (xSensorTaskHandle != NULL) {
        vTaskDelete(xSensorTaskHandle);
        xSensorTaskHandle = NULL;
    }
    
    // 删除队列和信号量
    if (xMenuEventQueue != NULL) {
        vQueueDelete(xMenuEventQueue);
        xMenuEventQueue = NULL;
    }
    
    if (xDisplaySemaphore != NULL) {
        vSemaphoreDelete(xDisplaySemaphore);
        xDisplaySemaphore = NULL;
    }
    
    if (xSensorSemaphore != NULL) {
        vSemaphoreDelete(xSensorSemaphore);
        xSensorSemaphore = NULL;
    }
}
