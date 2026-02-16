#include "common.h"
#include "stdio.h"//printf的库
#include <stdarg.h>

//**************下面包含delay延时和printf串口函数**********//
////微秒延时（HAL库）
//void Delay_us(uint16_t us)
//{     
//	uint16_t differ = 0xffff-us-5;				
//	__HAL_TIM_SET_COUNTER(&htim1,differ);	//设定TIM1计数器起始值
//	HAL_TIM_Base_Start(&htim1);		//启动定时器	
//	
//	while(differ < 0xffff-5)//判断
//	{	
//		differ = __HAL_TIM_GET_COUNTER(&htim1);		//查询计数器的计数值
//	}
//	HAL_TIM_Base_Stop(&htim1);//关闭定时器
//	
//}

void Delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}


//串口使用前要在usart初始化中加入
 // 	HAL_UART_Receive_DMA(&huart1,USART1_RX_BUF,BUFFER_SIZE);//存入USART3_RX_BUF
//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);	//空闲时就停止


//接收状态（接收的长度）//bit15，	接收完成标志	//bit14，	接收到0x0d   //bit13~0，	接收到的有效字节长度
u16 USART1_RX_STA;         		
u16 USART2_RX_STA;       
u16 USART3_RX_STA;         		
u16 USART4_RX_STA;       
u16 USART5_RX_STA;    
//串口接收字符串的缓冲区		(直接用这个字符串)
//注意,读取USARTx->SR能避免莫名其妙的错误   	
uint8_t USART1_RX_BUF[BUFFER_SIZE];
uint8_t USART2_RX_BUF[BUFFER_SIZE];
uint8_t USART3_RX_BUF[BUFFER_SIZE];    //接收缓冲,最大USART_REC_LEN个字节.
uint8_t USART4_RX_BUF[BUFFER_SIZE];
uint8_t USART5_RX_BUF[BUFFER_SIZE];

//使用：
/*循环检测
	if(USART1_RX_STA&0X8000)  //如果有接收到
	{
		USART1_RX_BUF[x].....//处理字符串
		
		USART1_RX_STA=0;
	}
*/


//重定义printf输出的串口		勾选设置-MicroLIB
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff);
  return ch;	
}
 
/**
  * 函数功能: 重定义scanf串口输入
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart2, &ch, 1, 0xffff);
  return ch;
}

/*-----串口发送函数----------------
带字符解析的发送函数 			使用：Printf_XL(&huart3,"hello");串口3发送hello
----------------------------------*/
uint8_t Printf_XL(UART_HandleTypeDef *huart,const char *format, ...)
{
	char buf[128]; //定义临时数组，根据实际发送大小微调
    va_list args;
    va_start(args, format);
    uint16_t len = vsnprintf((char *)buf, sizeof(buf), (char *)format, args);
    va_end(args);
    return HAL_UART_Transmit(huart,buf,len,1000); //串口打印函数，可以更换为中断发送或者DMA发送
}
//DMA版
uint8_t printf_DMA(UART_HandleTypeDef *huart,const char *format,...)
{
	char buf[128]; //定义临时数组，根据实际发送大小微调
	va_list args;	
	va_start(args,format);//用args接收format
	uint16_t len = vsnprintf((char*)buf,sizeof(buf)+1,(char*)format,args);
	va_end(args);
	return HAL_UART_Transmit_DMA(huart, buf, len);
}



////串口接收的函数		//HAL库没有自带串口接收数据的函数	  不知道为啥调用不了
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)	//系统接收到1个字符会运行这个
//{
//	if(huart->Instance == USART1)
//	{
//		//DMA串口模式	（串口空闲就自动停止完成）
//		if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET)
//		{
//			__HAL_UART_CLEAR_IDLEFLAG(&huart1);	//空闲中断
//			HAL_UART_DMAStop(&huart1);	
//			USART1_RX_STA=BUFFER_SIZE-__HAL_DMA_GET_COUNTER(huart1.hdmarx);//串口字符串长度
//			HAL_UART_Receive_DMA(&huart1,USART1_RX_BUF,BUFFER_SIZE);			//恢复串口中断
//			
//			USART1_RX_STA|=0x8000;//接收完成标识符（供人调用）
//		}
//	}
//	
//	else	if(huart->Instance == USART2)
//	{
//		//DMA串口模式	（串口空闲就自动停止）
//		if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET)
//		{
//			__HAL_UART_CLEAR_IDLEFLAG(&huart2);	//空闲中断
//			HAL_UART_DMAStop(&huart2);	
//			USART2_RX_STA=BUFFER_SIZE-__HAL_DMA_GET_COUNTER(huart2.hdmarx);//串口字符串长度
//			//恢复串口中断
//			HAL_UART_Receive_DMA(&huart2,USART2_RX_BUF,BUFFER_SIZE);			
//			
//			USART2_RX_STA|=0x8000;//接收完成标识符（供人调用）
//		}
//	}
//	
//	else if(huart->Instance == USART3)
//	{
//		//DMA串口模式	（串口空闲就自动停止）
//		if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE) != RESET)
//		{
//			__HAL_UART_CLEAR_IDLEFLAG(&huart3);	//空闲中断
//			HAL_UART_DMAStop(&huart3);	
//			USART3_RX_STA=BUFFER_SIZE-__HAL_DMA_GET_COUNTER(huart3.hdmarx);//串口字符串长度
//			//恢复串口中断
//			HAL_UART_Receive_DMA(&huart3,USART3_RX_BUF,BUFFER_SIZE);			
//			
//			USART3_RX_STA|=0x8000;//接收完成标识符（供人调用）
//		}
//	}
//}