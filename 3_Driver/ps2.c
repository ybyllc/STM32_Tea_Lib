#include "ps2.h"
//#include "usart.h"
/*********************************************************
Copyright (C), 2015-2025, YFRobot.
www.yfrobot.com
File??PS2驱动程序
Author??pinggai    Version:1.0     Data:2015/05/16
Description: PS2驱动程序
**********************************************************/ 
u16 Handkey;
u8 Comd[2]={0x01,0x42};//启动命令。请求数据
u8 Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组
u16 MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	}; //按键值映射表

//PS2初始化    输入  DI->PC0 
//                  输出  DO->PC1    CS->PC2  CLK->PC3
void PS2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 启用GPIOC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // PS2_DI_PIN (DI/DAT) - 输入
    GPIO_InitStruct.Pin = PS2_DI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PS2_DI_PORT, &GPIO_InitStruct);
    
    // PS2_DO_PIN (DO/CMD) - 输出
    GPIO_InitStruct.Pin = PS2_DO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PS2_DO_PORT, &GPIO_InitStruct);
    
    // PS2_CS_PIN (CS) - 输出
    GPIO_InitStruct.Pin = PS2_CS_PIN;
    HAL_GPIO_Init(PS2_CS_PORT, &GPIO_InitStruct);
    
    // PS2_CLK_PIN (CLK) - 输出
    GPIO_InitStruct.Pin = PS2_CLK_PIN;
    HAL_GPIO_Init(PS2_CLK_PORT, &GPIO_InitStruct);
    
    // 初始化默认状态
    CS_H;
    CLK_H;
    DO_L;
}


//发送PS2命令
void PS2_Cmd(u8 CMD)
{
	volatile u16 ref=0x01;
	Data[1] = 0;
	
	// CLK初始状态为高
	CLK_H;
	delay_us(2);
	
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
		// 设置 DO 数据
		if(ref&CMD)
		{
			DO_H;                   //输出1
		}
		else DO_L;

		// CLK保持高电平一段时间
		delay_us(2);
		
		// CLK下降沿 - 数据有效
		CLK_L;
		delay_us(8);
		
		// 在 CLK 低电平时读取数据（下降沿后）
		if(DI)
			Data[1] = ref|Data[1];
		
		// CLK保持低电平一段时间
		delay_us(6);
		
		// CLK回到高电平
		CLK_H;
	}
}
//判断是否为红灯模式
//返回值为0是红灯模式
//        其他值是绿灯模式
u8 PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);  //发送启动命令
	PS2_Cmd(Comd[1]);  //发送请求数据
	CS_H;
	if( Data[1] == 0X73)   return 0 ;
	else return 1;

}
//读取PS2数据
void PS2_ReadData(void)
{
	volatile u8 byte=0;
	volatile u16 ref=0x01;

	CS_L;
	delay_us(20);  // CS拉低后等待一段时间，让手柄准备就绪

	PS2_Cmd(Comd[0]);  //发送启动命令
	PS2_Cmd(Comd[1]);  //发送请求数据

	// CLK初始状态为高
	CLK_H;
	delay_us(2);
	
	for(byte=2;byte<9;byte++)          //开始读取数据
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			// CLK保持高电平一段时间
			delay_us(2);
			
			// CLK下降沿 - 数据有效
			CLK_L;
			delay_us(8);
			
			// 在 CLK 低电平时读取数据（下降沿后）
			if(DI)
				Data[byte] = ref|Data[byte];
			
			// CLK保持低电平一段时间
			delay_us(6);
			
			// CLK回到高电平
			CLK_H;
		}
	}
	CS_H;
	delay_us(20);  // CS拉高后等待
}

//自动读取PS2按键数据并返回按键值        只读取了有按键按下时的数据          默认状态是绿灯模式  只有一个按键按下时
//返回为0时  无按键 为1时
u8 PS2_DataKey()
{
	u8 index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(Data[4]<<8)|Data[3];     //合成16位键值  按下为0， 未按下为1
	for(index=0;index<16;index++)
	{
		if((Handkey&(1<<(MASK[index]-1)))==0)
			if(index!=0)
				return index+1;
	}
	return 0;          //没有任何按键按下
}

//得到一个摇杆的模拟量	 范围0~256
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//清除数据缓冲区
void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}


//PS2发送短查询
void PS2_ShortPoll(void) 
{
  CS_L; 
  delay_us(10); 
  PS2_Cmd(0x01); 
  PS2_Cmd(0x42); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0x00); 
  PS2_Cmd(0x00); 
  CS_H; 
  delay_us(10);
}


//进入配置模式
void PS2_EnterConfing(void) 
{
  CS_L;
  delay_us(10); 
  PS2_Cmd(0x01); 
  PS2_Cmd(0x43); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0x01); 
  PS2_Cmd(0x00); 
  PS2_Cmd(0X00);
  PS2_Cmd(0X00); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0X00); 
  CS_H; 
  delay_us(10);
}


// 开启模拟模式
void PS2_TurnOnAnalogMode(void) 
{
  CS_L; 
  PS2_Cmd(0x01); 
  PS2_Cmd(0x44); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0x01);//analog=0x01;digital=0x00 设为模拟模式
  PS2_Cmd(0xEE);//Ox03 锁定设置，退出后必须重新进入配置才可以更改模式        //0xEE 不锁定设置，退出后无需重新进入配置就可以更改模式
  PS2_Cmd(0X00); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0X00); 
  CS_H; 
  delay_us(10);
}


// 开启震动
void PS2_VibrationMode(void) 
{
  CS_L; 
  delay_us(10); 
  PS2_Cmd(0x01); 
  PS2_Cmd(0x4D); 
  PS2_Cmd(0X00); 
  PS2_Cmd(0x00); 
  PS2_Cmd(0X01); 
  CS_H; 
  delay_us(10);
}


// 退出配置模式
void PS2_ExitConfing(void)
{
  CS_L;
  delay_us(10);
  PS2_Cmd(0x01);
  PS2_Cmd(0x43); 
  PS2_Cmd(0X00);
  PS2_Cmd(0x00); 
  PS2_Cmd(0x5A); 
  PS2_Cmd(0x5A); 
  PS2_Cmd(0x5A);
  PS2_Cmd(0x5A); 
  PS2_Cmd(0x5A); 
  CS_H; 
  delay_us(10);
}

// PS2初始化
void PS2_SetInit(void) 
{
  PS2_ShortPoll();
  PS2_ShortPoll();
  PS2_ShortPoll(); 
  PS2_EnterConfing(); // 进入配置模式 
  PS2_TurnOnAnalogMode(); // 开启模拟模式
  PS2_VibrationMode(); // 开启震动模式
  PS2_ExitConfing(); // 退出配置模式
}

//震动，大小可调
//
void PS2_Vibration(u8 motor1,u8 motor2)
{
   CS_L; 
   delay_us(10); 
   PS2_Cmd(0x01); // 发送启动命令
   PS2_Cmd(0x42);// 发送请求数据
   PS2_Cmd(0X00);
   PS2_Cmd(motor1);
   PS2_Cmd(motor2); 
   PS2_Cmd(0X00); 
   PS2_Cmd(0X00); 
   PS2_Cmd(0X00); 
   PS2_Cmd(0X00); 
   CS_H; 
   delay_us(10);
} 

/*
 参考例程
 
//PS2手柄测试例程
void ps2_test()
{
	// 初始化PS2手柄
	PS2_Init();
	
	// 配置PS2手柄为模拟模式并开启震动
	PS2_SetInit();
	
	// 检查是否为红灯模式
	if (PS2_RedLight() == 0)
	{
		// 红灯模式（模拟模式）
		uart_write_string(UART_4, "PS2: Red Light Mode\r\n");
	}
	else
	{
		// 绿灯模式（数字模式）
		uart_write_string(UART_4, "PS2: Green Light Mode\r\n");
	}
	
	while(1)
	{
		// 读取PS2手柄数据
		PS2_ReadData();
		
		// 获取按键状态
		u8 key = PS2_DataKey();
		if (key != 0)
		{
			// 按键按下，发送按键信息
			uart_write_string(UART_4, "PS2 Key: ");
			// 这里可以根据按键值发送具体的按键名称
			uart_write_string(UART_4, "\r\n");
			
			// 示例：根据不同按键执行不同操作
			switch(key)
			{
				case PSB_PAD_UP:
					// 向上键
					uart_write_string(UART_4, "UP\r\n");
					break;
				case PSB_PAD_DOWN:
					// 向下键
					uart_write_string(UART_4, "DOWN\r\n");
					break;
				case PSB_PAD_LEFT:
					// 向左键
					uart_write_string(UART_4, "LEFT\r\n");
					break;
				case PSB_PAD_RIGHT:
					// 向右键
					uart_write_string(UART_4, "RIGHT\r\n");
					break;
				case PSB_START:
					// 开始键
					uart_write_string(UART_4, "START\r\n");
					break;
				case PSB_SELECT:
					// 选择键
					uart_write_string(UART_4, "SELECT\r\n");
					break;
				default:
					break;
			}
		}
		
		// 获取摇杆数据
		u8 rx = PS2_AnologData(PSS_RX); // 右摇杆X轴
		u8 ry = PS2_AnologData(PSS_RY); // 右摇杆Y轴
		u8 lx = PS2_AnologData(PSS_LX); // 左摇杆X轴
		u8 ly = PS2_AnologData(PSS_LY); // 左摇杆Y轴
		
		// 每100ms发送一次摇杆数据
		static u32 last_time = 0;
		if (HAL_GetTick() - last_time > 100)
		{
			last_time = HAL_GetTick();
			
			// 发送摇杆数据
			uart_write_string(UART_4, "PS2 Analog: ");
			// 这里可以发送具体的摇杆数值
			uart_write_string(UART_4, "\r\n");
		}
		
		// 短延时
		delayms(10);
	}
}

*/