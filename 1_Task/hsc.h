#ifndef _HSC_H	//
#define _HSC_H  //避免重复编译，只是自己的取名，只能包含字母和下划线
#include "stm32f4xx_hal.h"
#include "main.h"

static u32 fac_us=0;							//us延时倍乘数
void Delay_us(u32 nus);
#define delay_ms(n) HAL_Delay(n)
#define delay_us(n) Delay_us(n)
//多串口发送
uint8_t Printf_XL(UART_HandleTypeDef *huart,const char *format, ...);
uint8_t printf_DMA(UART_HandleTypeDef *huart,const char *format,...);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#define BUFFER_SIZE 128

//接收状态标记	
extern u16 USART1_RX_STA;         
extern u16 USART2_RX_STA;         
extern u16 USART3_RX_STA;       
extern u16 USART4_RX_STA;         
extern u16 USART5_RX_STA;   

extern u8  USART1_RX_BUF[BUFFER_SIZE]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART2_RX_BUF[BUFFER_SIZE]; 
extern u8  USART3_RX_BUF[BUFFER_SIZE]; 
extern u8  USART4_RX_BUF[BUFFER_SIZE]; 
extern u8  USART5_RX_BUF[BUFFER_SIZE]; 




//变量

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
///////////////////////////////////////////////////////////////
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
//F103
 #define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
 #define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
 #define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
 //IO口地址映射
 #define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
 #define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
 #define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
 #define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
 #define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
 #define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
 #define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

 #define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
 #define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
 #define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
 #define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
 #define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
 #define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
 #define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
 //IO口操作,只对单一的IO口!
 //确保n的值小于16!
 #define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
 #define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

 #define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
 #define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

 #define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
 #define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

 #define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
 #define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

 #define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
 #define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

 #define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
 #define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

 #define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
 #define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
 /////////////////////////////////////////////////////////////////
 //引脚定义
 #define GPIO_A 0
 #define GPIO_B 1
 #define GPIO_C 2
 #define GPIO_D 3
 #define GPIO_E 4
 #define GPIO_F 5
 #define GPIO_G 6 
 #define FTIR   1  //下降沿触发
 #define RTIR   2  //上升沿触发
 /////////////////////////////////////////////////////////////////

#endif