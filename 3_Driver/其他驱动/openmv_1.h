#ifndef _OPENMV_1_H
#define _OPENMV_1_H
#include "car-data.h"

//=================引脚定义================
#define UART_INDEX              (UART_4   )				// 默认 UART_1
#define UART_BAUDRATE           (115200)				// 默认 115200
#define UART_TX_PIN             (UART4_TX_C16  )				// 默认 UART1_TX_A9
#define UART_RX_PIN             (UART4_RX_C17  )				// 默认 UART1_RX_A10

#define UART_PRIORITY           (LPUART4_IRQn)    
                              // 对应串口中断的中断编号 在 MIMXRT1064.h 头文件中查看 IRQn_Type 枚举体

extern uint8 uart_get_data[64];                                                        // 串口接收数据缓冲区
extern uint8 fifo_get_data[64];                                                        // fifo 输出读出缓冲区

extern uint8 get_data;                                                             // 接收数据变量
extern uint32 fifo_data_count;                                                     // fifo 数据个数

extern uint8 gpio_status;
extern fifo_struct uart_data_fifo;

void openart_1_init(); //初始化摄像头的接收函数

//================功能函数===============
void Target_get();   //搜索图像

extern int Tar_x;	//图像的中心点
extern int Tar_y;	

extern short rx_pt[21][2];//接收到的点坐标
extern short rx_pt_num;  //接收到的点数

//读取地图的数据  成功返回1
u8 Get_point(void);
#endif
