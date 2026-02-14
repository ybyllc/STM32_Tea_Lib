#include "lcd.h"
#include "car-data.h"
 //逐飞的库函数

//显示字符串 //x,y坐标轴，字符串
#define ShowString(x,y,String)		ips114_show_string(x,y,String)

//显示整数	 //x,y坐标轴，变量，整数位数
#define ShowInt(x,y,Int,Int_Long)   ips114_show_int(x,y,Int,Int_Long);

//显示小数	//x,y坐标轴，小数，整数位数，小数位数
#define ShowFloat(x,y,Float,Int_Long,Float_Long)	ips114_show_float(x,y,Float,Int_Long,Float_Long)

//初始化显示屏
#define LCD_Init() 		ips114_init()



