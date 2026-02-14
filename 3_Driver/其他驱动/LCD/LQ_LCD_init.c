#include "LQ_LCD_init.h"

#include "LQ_GPIO.h"
#include "LQ_GPIO_Cfg.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "LQ_SGP18T.h"
#include "stdio.h"


//移植1 接口
//-----------------LCD端口定义---------------- 

// TFTSPI_CS  ------ 管脚D11
// TFTSPI_SCK ------ 管脚D12
// TFTSPI_SDI ------ 管脚E12
// TFTSPI_RST ------ 管脚B13
// TFTSPI_DC  ------ 管脚E11 //看自己板子的管脚

// IPS BL（背光）---- 管脚G11
//--------------------

#define LCD_SCLK_Clr() LQ_PinWrite(D12, 0U) //SCL=SCLK
#define LCD_SCLK_Set() LQ_PinWrite(D12, 1U)

#define LCD_MOSI_Clr() LQ_PinWrite(E12, 0U) //SDA=MOSI
#define LCD_MOSI_Set() LQ_PinWrite(E12, 1U)

#define LCD_CS_Clr()   LQ_PinWrite(D11, 0U)//CS
#define LCD_CS_Set()   LQ_PinWrite(D11, 1U)

#define LCD_RES_Clr()  LQ_PinWrite(B13, 0U)//RST B13
#define LCD_RES_Set()  LQ_PinWrite(B13, 1U)

#define LCD_DC_Clr()   LQ_PinWrite(E11, 0U) //DC E11
#define LCD_DC_Set()   LQ_PinWrite(E11, 1U)

#define LCD_BLK_Clr()  LQ_PinWrite(G11, 0U)//BLK
#define LCD_BLK_Set()  LQ_PinWrite(G11, 1U)

//我的板子上，DC和RST对调了，所以管脚相反
//后面改过来了

void delay_ms(uint16_t ms) //移植2 延时时间
{
//	volatile uint32_t i = 0;
//	while(ms--)
//	{
//		for (i = 0; i < 20000; ++i)
//		{
//			__asm("NOP"); /* delay */
//		}
//	}	
	//systime_delay_ms(ms);
	systime_delay_us(300*ms);
}

//驱动层和应用层都是源码

void LCD_GPIO_Init(void)
{
    //-----端口初始化----//
    CLOCK_EnableClock(kCLOCK_Iomuxc);           // IO口时钟使能
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_05_GPIO2_IO21,0U);                                                          
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_04_GPIO2_IO20, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_10_GPIO2_IO26, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_15_GPIO2_IO15, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_GPIO2_IO19, 0U);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_05_GPIO2_IO21,0x10B0u);                                                   
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_04_GPIO2_IO20,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_10_GPIO2_IO26,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_15_GPIO2_IO15,0x10B0u); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_GPIO2_IO19,0x10B0u); 
    //输出口管脚配置结构体
        gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIO为输出方向
                                                1,                   //电平
                                                kGPIO_NoIntmode      //非中断模式
                                                };  
        
    GPIO_PinInit(GPIO2, 21, &GPIO_Output_Config);      // CS   D12
    GPIO_PinInit(GPIO2, 20, &GPIO_Output_Config);      // SCL  E12
    GPIO_PinInit(GPIO2, 26, &GPIO_Output_Config);      // SDI  B13
    GPIO_PinInit(GPIO2, 15, &GPIO_Output_Config);      // DC   E11 
    GPIO_PinInit(GPIO2, 19, &GPIO_Output_Config);      // RST  D11
												
												
	LQ_PinInit(G11, PIN_MODE_OUTPUT, 1);	//BL	//打开屏幕背光
}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();//拉低时钟线
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}	
  LCD_CS_Set();	
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+52);
		LCD_WR_DATA(x2+52);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+40);
		LCD_WR_DATA(y2+40);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+53);
		LCD_WR_DATA(x2+53);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+40);
		LCD_WR_DATA(y2+40);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+40);
		LCD_WR_DATA(x2+40);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+53);
		LCD_WR_DATA(y2+53);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+40);
		LCD_WR_DATA(x2+40);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+52);
		LCD_WR_DATA(y2+52);
		LCD_WR_REG(0x2c);//储存器写
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	
	LCD_RES_Clr();//复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_BLK_Set();//打开背光
	delay_ms(100);
	
	LCD_WR_REG(0x11); 
	delay_ms(120); 
	LCD_WR_REG(0x36); 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33); 

	LCD_WR_REG(0xB7); 
	LCD_WR_DATA8(0x35);  

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x19);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x12);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);  

	LCD_WR_REG(0xC6); 
	LCD_WR_DATA8(0x0F);    

	LCD_WR_REG(0xD0); 
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0x21); 

	LCD_WR_REG(0x29); 
}








