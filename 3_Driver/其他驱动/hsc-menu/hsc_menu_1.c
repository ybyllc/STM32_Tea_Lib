//创自Fenck
//改自hsc
//单级菜单

//页面1：调PID等参数
//页面2：调摄像头照片
//页面3：开始

//上下键选择
//左右键 调整
//按下键 切换页面

//代码结构:结构体写好信息
//并在函数中把信息显示出来

#include "hsc_menu_1.h"
#include <string.h>//字符串处理函数

#include "LQ_LCDst7789.h"//屏幕
#include "LQ_KEY.h"//按键
#include "LQ_GPIO.h"//gpio操作

#include "motor-m.h"//麦轮版电机头文件

#include "LQ_SYSTICK.h"//延时头文件

//移植中需要自定义的函数
#define showstr    LCD_ShowString    //显示字符串的函数   ShowStr(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
#define shownum    LCD_ShowIntNum   //显示数字的函数    (u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
#define showchinese LCD_ShowChinese //显示中文 (u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
#define showclear  LCD_Fill         //清屏      (u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
#define showblock  LCD_Fill         //显示色块
#define showline   LCD_DrawLine       //画线  (u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
#define KEY_Scan    KEY_Read(0)//按键检测函数
#define delay systime_delay_ms //延时函数

//返回的按键参数
#define Keyup       3
#define Keydown     5
#define Keyleft     4
#define Keyright    2    
#define Keyclick    1//按下


#define X_max 240//最大x轴分辨率
#define Y_max 135//

#define fontc BLACK//字体颜色
#define backc WHITE//背景颜色
#define s_fontc WHITE//选中字颜色
#define s_backc BLUE//选中背景颜色



u8 keynum;
u8 readytorun;
u8 point=0;//当前显示第几行

u8 key_over=0;//退出标记


u8 y_size=16;//字体大小 //16,24,32
#define x0 40
#define y0 5    //显示屏开始显示的位置

#define y1  18//每行的显示间隔
#define x1 30//显示→箭头的提前量
#define x2 40//显示数值的延后量

//显示的一行菜单行
typedef struct Line
{
    char word[20];//标题文字，中文是2个字节
    u16 *num;//数值指针
    int step;//一次增加的值
}MENU;//用这个名称定义结构体

u16 a1=100,b1=200,c1=100;

extern u16	P;
extern u16	D;
extern u16 PIDmax;//PID限幅

extern u16	p_P;
extern u16	p_D;
extern u16 p_PIDmax;//PID限幅

//旋转的PID
extern u16	a_P;
extern u16	a_D;
extern u16 a_PIDmax;

extern u16	a1_P;
extern u16	a1_D;
extern u16 a1_PIDmax;


//菜单的内容
//定义结构体
const MENU p1[]=
{
    "p_P  :",//标题
    &p_P,//数值的指针
    1,//一次改变的间隔

    "p_D  :",
    &p_D,
    1,
    
    "Max:",
    &p_PIDmax,
    100,

    "a1_P:",
    &a1_P,
    1,

    "a1_D:",
    &a1_D,
    1,
    
    "a1_Max:",
    &a1_PIDmax,
    100,

};

extern u16 rect_zx; //矩形应该在车前面的中点（要根据摄像头和实际车改
extern u16 rect_zy;

const MENU p2[]=
{
    "X:",//标题   //摄像头的中点x坐标
    &rect_zx,//数值的指针
    1,//一次改变的间隔

    "Y:",//标题   //摄像头的中点x坐标
    &rect_zy,//数值的指针
    1,//一次改变的间隔
};

//舵机的下放和上升的角度(0~180°）
extern u16 s_down;
extern u16 s_up;

const MENU p3[]=
{
    "up:",//标题   //摄像头的中点x坐标
    &s_up,//数值的指针
    1,//一次改变的间隔

    "down:",//标题   //摄像头的中点x坐标
    &s_down,//数值的指针
    1,//一次改变的间隔
};

extern u16 car_d;
extern u16 PIDmax;//PWM限幅 *10

extern u16	a_P;//16//8//32;
extern u16	a_I;
extern u16	a_D;//23;
extern u16 a_PIDmax;//2000//速度目标

const MENU p4[]=
{
    "car_d:",//标题   //摄像头的中点x坐标
    &s_up,//数值的指针
    10,//一次改变的间隔
	
	"PWMmax:",//标题   //摄像头的中点x坐标
    &s_up,//数值的指针
    1000,//一次改变的间隔

    "a_P:",//标题
    &a_P,//数值的指针
    1,//一次改变的间隔
	
	"a_I:",//标题
    &a_I,//数值的指针
    1,//一次改变的间隔
	
	"a_D:",//标题
    &a_D,//数值的指针
    1,//一次改变的间隔
	
	"a_PIDmax:",//标题
    &a_PIDmax,//数值的指针
    100,//一次改变的间隔
};

//example 菜单示例函数
/*
//界面1显示
void menu_p1(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;//要使用的循环
    point=0;//当前第几行

    key_over=0;
	while(!key_over)
	{
		switch(KEY_Scan)
		{
			case Keyup:
				break;
			case Keydown:
				break;
			case Keyright: 
                break;
			case Keyleft:
				break;
            case Keyclick:
                key_over=1;//退出
                break;
			default:break;
		}	
	}
}
*/



//界面1显示
void menu_p1(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;//要使用的循环
    point=0;//当前第几行
    u8 len;
    len=sizeof(p1) / sizeof(p1[0]);//菜单的长度
    for(i=0;i<len;i++){
        showstr(x0,y0+y1*i,p1[i].word,fontc,backc,y_size,1);//显示标题		//字符串用指针输入
        shownum(x0+x2,y0+y1*i,*p1[i].num,5,fontc,backc,y_size);//显示数值
    }
	showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);

    key_over=0;
	while(!key_over)
	{
		switch(KEY_Scan)
		{
			case Keyup:
				if(point>0) 
				{
					point--;
                    LCD_Fill(0,0,x0,Y_max,WHITE);
					//ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
                    showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);
				}
				break;
			case Keydown:
				if(point<len-1)
				{
					point++;
                    LCD_Fill(0,0,x0,Y_max,WHITE);
					//ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
                    showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);
				}
				break;
			case Keyright: 
                *p1[point].num+=p1[point].step;//数值使用指针
                shownum(x0+x2,y0+y1*point,*p1[point].num,5,fontc,backc,y_size);//显示数值
				break;
			case Keyleft:
                *p1[point].num-=p1[point].step;//数值使用指针
                shownum(x0+x2,y0+y1*point,*p1[point].num,5,fontc,backc,y_size);//显示数值
				break;
            case Keyclick:
                key_over=1;//退出
                break;
			default:break;
		}	
	}
}

//界面4调整 car_d 和a_PID转向环
void menu_p4(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;//要使用的循环
    point=0;//当前第几行
    u8 len;
    len=sizeof(p4) / sizeof(p4[0]);//菜单的长度
    for(i=0;i<len;i++){
        showstr(x0,y0+y1*i,p4[i].word,fontc,backc,y_size,1);//显示标题
        shownum(x0+x2,y0+y1*i,*p4[i].num,5,fontc,backc,y_size);//显示数值
    }
	showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);

    key_over=0;
	while(!key_over)
	{
		switch(KEY_Scan)
		{
			case Keyup:
				if(point>0) 
				{
					point--;
                    LCD_Fill(0,0,x0,Y_max,WHITE);
					//ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
                    showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);
				}
				break;
			case Keydown:
				if(point<len-1)
				{
					point++;
                    LCD_Fill(0,0,x0,Y_max,WHITE);
					//ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
                    showstr(x0-x1,y0+y1*point,"->",fontc,backc,y_size,1);
				}
				break;
			case Keyright: 
                *p4[point].num+=p4[point].step;//数值使用指针
                shownum(x0+x2,y0+y1*point,*p4[point].num,5,fontc,backc,y_size);//显示数值
				break;
			case Keyleft:
                *p4[point].num-=p4[point].step;//数值使用指针
                shownum(x0+x2,y0+y1*point,*p4[point].num,5,fontc,backc,y_size);//显示数值
				break;
            case Keyclick:
                key_over=1;//退出
                break;
			default:break;
		}	
	}
}

extern u16 motor_t[4];
extern short velocity[4];//编码器	1580 1550 1610

void menu_motor_test(void)//电机测试菜单
{
	showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;//要使用的循环
	u8 txt[20];
	u8 size=16;
	point=0;//当前第几行
	key_over=0;   
	
	for(i=0;i<4;i++)
	{
		sprintf(txt,"%5d ",motor_t[i]); 
		Motor_Set(i+1,motor_t[i]);
		LCD_ShowString(150,y1*(i+2),txt,BLACK,WHITE,size,0); 
	}
		
	while(!key_over)
	{
		sprintf(txt,"ENC1:%5d ",velocity[0]); 
		LCD_ShowString(0,y1*2,txt,BLACK,WHITE,size,0);

		sprintf(txt,"ENC2:%5d ",velocity[1]);
		LCD_ShowString(0,y1*3,txt,BLACK,WHITE,size,0);
		
		sprintf(txt,"ENC3:%5d ",velocity[2]); 
		LCD_ShowString(0,y1*4,txt,BLACK,WHITE,size,0);
		
		sprintf(txt,"ENC4:%5d ",velocity[3]); 
		LCD_ShowString(0,y1*5,txt,BLACK,WHITE,size,0);

		switch(KEY_Scan)
		{
			case Keyup:
				if(point>0) 
				{
					point--;
                    sprintf(txt,"%5d ",motor_t[point]); 
		            LCD_ShowString(150,y1*(point+2),txt,BLACK,WHITE,size,0);      
				}
				break;
			case Keydown:
				if(point<3)
				{
					//Motor_Set(point,motor_t[point]);
					point++;
                    sprintf(txt,"%5d ",motor_t[point]); 
		            LCD_ShowString(150,y1*(point+2),txt,s_fontc,s_backc,size,0);
				}
				break;
			case Keyright: 
                motor_t[point]+=10;//数值使用指针
                sprintf(txt,"%5d ",motor_t[point]); 
		        LCD_ShowString(150,y1*(point+2),txt,BLACK,WHITE,size,0);  
                Motor_Set(point+1,motor_t[point]);
				break;
			case Keyleft:
                motor_t[point]-=10;//数值使用指针
                sprintf(txt,"%5d ",motor_t[point]); 
		        LCD_ShowString(150,y1*(point+2),txt,BLACK,WHITE,size,0);  
                Motor_Set(point+1,motor_t[point]);
				break;
            case Keyclick:
                key_over=1;//退出
                break;
			default:break;
		}	
		delay(100);	
	}	
}


//页面3
//电磁铁校准
void menu_p3(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;//要使用的循环
    point=0;//当前第几行
    //显示X:
    showstr(x0,y0,p3[0].word,fontc,backc,y_size,1);//显示标题
    shownum(x0+x2,y0,*p3[0].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中

    //显示X：    Y：    
    showstr(x0+x2+x2,y0,p3[1].word,fontc,backc,y_size,1);//显示标题
    shownum(x0+x2+x2+x2,y0,*p3[1].num,3,fontc,backc,y_size);//显示数值
    
    showchinese(x0,Y_max/2,"电磁铁校准",fontc,backc,24,0);
	//LQ_PinWrite(H14, 1);//打开电磁铁
	
    key_over=0;
	while(!key_over)
	{
		switch(KEY_Scan)//检测按键
		{
			case Keyleft:
				if(point==0) 
				{
					LQ_PinWrite(H14, 1);//打开电磁铁
					point=1;//只有两个选项的写法
					shownum(x0+x2,y0,*p3[0].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中
					shownum(x0+x2+x2+x2,y0,*p3[1].num,3,s_fontc,s_backc,y_size);//显示数值
				}
				else if(point==1)
				{
					LQ_PinWrite(H14,0);
					point=0;
					shownum(x0+x2,y0,*p3[0].num,3,fontc,backc,y_size);//显示数值    
					shownum(x0+x2+x2+x2,y0,*p3[1].num,3,fontc,fontc,y_size);//显示数值 //数字被选中
				}
                //选中X：
                Servo_Set(*p3[point].num);//舵机角度移动
                //shownum(x0+x2,y0,*p3[0].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中
                //shownum(x0+x2+x2+x2,y0,*p3[1].num,3,fontc,backc,y_size);//显示数值
				break;
			case Keyright:
				if(point==0) 
				{
					LQ_PinWrite(H14, 1);//打开电磁铁
					point=1;//只有两个选项的写法
					shownum(x0+x2,y0,*p3[0].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中
					shownum(x0+x2+x2+x2,y0,*p3[1].num,3,s_fontc,s_backc,y_size);//显示数值
				}
				else if(point==1)
				{
					LQ_PinWrite(H14, 0);
					point=0;
					shownum(x0+x2,y0,*p3[0].num,3,fontc,backc,y_size);//显示数值    
					shownum(x0+x2+x2+x2,y0,*p3[1].num,3,fontc,backc,y_size);//显示数值 //数字被选中
				}
                //选中Y：
                Servo_Set(*p3[point].num);//舵机角度移动
                //shownum(x0+x2,y0,*p3[0].num,3,fontc,backc,y_size);//显示数值    //数字被选中
                //shownum(x0+x2+x2+x2,y0,*p3[1].num,3,s_fontc,s_backc,y_size);//显示数值
				break;
			case Keyup: 
                *p3[point].num+=p3[point].step;//数值使用指针
                shownum(x0+x2+x2*point*2,y0,*p3[point].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中
				Servo_Set(*p3[point].num);//舵机角度移动
				break;
			case Keydown:
                *p3[point].num-=p3[point].step;//数值使用指针
                shownum(x0+x2+x2*point*2,y0,*p3[point].num,3,s_fontc,s_backc,y_size);//显示数值    //数字被选中
				Servo_Set(*p3[point].num);//舵机角度移动
				break;
            case Keyclick:
				LQ_PinWrite(H14, 0);//关闭电磁铁
                key_over=1;//退出
                break;
			default:break;
		}	
	}
}


extern short c[4];//有负号，接收到的三个数字     矩形的大小
extern short cc[3];//接收到的圆点的两个坐标数字

extern u16 c_W,c_H;//地图宽高
extern float WH_b;//场地宽高的缩放比例

extern u8 ri;//圆点个数
extern u16 rx[20];//圆点的坐标位置(最大支持20个)
extern u16 ry[20];

extern u8 rect_a;//识别到矩形的标识符 0：未识别
extern u16 rect_4xy[8];//矩形四点坐标
extern u16 rect_zxy[2];//中点坐标

extern u16 rect_zxy0[2];//设定的中点坐标

//用四条线画矩形    (四个点的数组)(主要靠这个函数
void showrect_line(u16 r_xy[8])
{
	u8 i;
	u16 xy[8];
	for(i=0;i<8;i++)
	{	
		xy[i]=r_xy[i]/2;
	}
    showline(xy[0],xy[1],xy[2],xy[3],fontc);
    showline(xy[2],xy[3],xy[4],xy[5],fontc);
    showline(xy[4],xy[5],xy[6],xy[7],fontc);
    showline(xy[6],xy[7],xy[0],xy[1],fontc);//四条线画完
}


//画十字函数“+”     （中点）
void showcross(u16 x,u16 yy,u16 color)
{
    showline(x,yy,x,yy-4,color);
    showline(x,yy,x,yy+4,color);
    showline(x,yy,x-4,yy,color);
    showline(x,yy,x+4,yy,color);
}
//页面2
//显示摄像头1的中点（为了抓取）
void menu_p2(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	LCD_DrawRectangle(0, 0, 160, 120, RED);//画红色边框
	u8 i;//要使用的循环
    //point=0;//当前第几行	
	//第一次获取中点值
	*p2[0].num=rect_zxy[0];
	*p2[1].num=rect_zxy[1];
	
	//显示rect标志	
	showstr(170,y0,"rect_zd",fontc,backc,y_size,1);//显示标题
	
	//显示X:	
	showstr(170,y0+y1,p2[0].word,fontc,backc,y_size,1);//显示标题

	//显示     Y：    
	showstr(170,y0+y1*2,p2[1].word,fontc,backc,y_size,1);//显示标题
    
    key_over=0;
	while(!key_over)
	{	
		//画矩形
		showrect_line(rect_4xy);
		LCD_Fill(rect_zxy[0]-1,rect_zxy[1]-1,rect_zxy[0]+1,rect_zxy[1]+1,RED);//画中点
		showcross(*p2[0].num,*p2[1].num,s_backc);	

		shownum(170+x2,y0+y1,*p2[0].num,3,fontc,backc,y_size);//显示数值 

		shownum(170+x2,y0+y1*2,*p2[1].num,3,fontc,backc,y_size);//显示数值
		switch(KEY_Read(1))
		{
			case Keyleft://坐标左移(x--，y不动)
				showclear(1,1,160-1,120-1,WHITE);
                *p2[0].num-=p2[0].step;
//                showstr(x0+x2+x2,y0,p2[1].word,fontc,backc,y_size,1);//显示标题
//                shownum(x0+x2+x2+x2,y0,*p2[1].num,3,fontc,backc,y_size);//显示数值
//                showcross(*p2[0].num,*p2[1].num);
				break;
			case Keyright://坐标右移(x++，y不动)
				showclear(1,1,160-1,120-1,WHITE);
                *p2[0].num+=p2[0].step;
//                shownum(x0+x2,y0,*p2[0].num,3,fontc,backc,y_size);//显示数值    //数字被选中
//                shownum(x0+x2+x2+x2,y0,*p2[1].num,3,s_fontc,s_backc,y_size);//显示数值
//                showcross(*p2[0].num,*p2[1].num);
				break;
			case Keyup: //坐标上移（x不动，y++）
				showclear(1,1,160-1,120-1,WHITE);
                *p2[1].num+=p2[1].step;
//                shownum(x0+x2,y0,*p2[0].num,3,fontc,backc,y_size);//显示数值    //数字被选中
//                shownum(x0+x2+x2+x2,y0,*p2[1].num,3,s_fontc,s_backc,y_size);//显示数值
//                showcross(*p2[0].num,*p2[1].num);
				break;
			case Keydown://坐标下移（x不动，y--）
				showclear(1,1,160-1,120-1,WHITE);
                *p2[1].num-=p2[1].step;
//                shownum(x0+x2,y0,*p2[0].num,3,fontc,backc,y_size);//显示数值    //数字被选中
//                shownum(x0+x2+x2+x2,y0,*p2[1].num,3,s_fontc,s_backc,y_size);//显示数值
//                showcross(*p2[0].num,*p2[1].num);
				break;
            case Keyclick:
				rect_zxy0[0]=*p2[0].num;
				rect_zxy0[1]=*p2[1].num;
                key_over=1;//退出
				showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
                break;
			default:break;
		}	
	}
}

//（旧版）画收到的地图矩形
void draw_map()	//应该是校正后的，应该是按一个按钮显示
{
    //矩形画面：210*105
    
    //**画矩形函数(传入的参数是矩形的长宽
    //从起始点(10,20） 开始画
    LCD_DrawRectangle(10, 20, 10+c_W, 20+c_H, RED);
    
    //显示矩形的尺寸,和比例 W:20 H:10 B/:2
    char c_txt[16];
    sprintf(c_txt,"W:%3d H:%3d B/:%1.2f",c_W,c_H,(float)c_W/c_H); 
    LCD_ShowString(50,2,c_txt, BLACK, WHITE,16,0);//显示字符 显示宽高和比例
}


void draw_circle()
{
    u8 i;
    //********画矩形里的这个圆点    
    //显示当前圆的个数
    char txt[16];
    sprintf(txt,"r:%2d ",ri+1);	//因为数组的关系,个数+1 
    LCD_ShowString(10,2,txt, BLACK, WHITE,16,0);//显示字符串

    //画一个大小是4的方块		
    for(i=0;i<=ri;i++)
    {
        LCD_Fill(10+rx[ri]*WH_b-3, 20+ry[ri]*WH_b-3,10+rx[ri]*WH_b+3, 20+ry[ri]*WH_b+3,BLACK);
        //圆的坐标轴也要缩放
    }
}


extern u16 map_4xy[8];//地图的四角
extern u16 r_n;//圆点的真正次数

//页面4
//摄像头2识别地图
void menu_map(void)
{
    showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	u8 i;
	showstr(170,y0,"map",fontc,backc,y_size,1);//显示标题
    //识别一次图像		//（实时识别？
    UART_PutChar(LPUART2, 'a');//摄像头1 发送识别一次的字符
    LCD_DrawRectangle(0, 0, 160, 120, RED);
    delay(500);//延时等待500ms //等待识图
    showrect_line(map_4xy);//画矩形

	showstr(170,y0+y1*2,"W",fontc,backc,y_size,1);//显示W
	showstr(170,y0+y1*3,"H",fontc,backc,y_size,1);

	shownum(190,y0+y1*2,c_W,fontc,backc,y_size,1);//显示W
	shownum(190,y0+y1*3,c_H,fontc,backc,y_size,1);
    
    for(i=0;i<=ri;i++)  //画圆点
    {
        LCD_Fill(rx[i]/2-3, ry[i]/2-3,rx[i]/2+3,ry[i]/2+3,RED);  //  /2,画面太大
    }
	r_n=ri;

    key_over=0;
	while(!key_over)
	{
		//自动识别地图
			//重新识别一次新的图像
		UART_PutChar(LPUART2, 'a');//摄像头2 发送识别一次的字符
		showclear(1,1,160-1,120-1,WHITE);
		delay(300);//延时等待500ms
		showclear(1,1,160-1,120-1,WHITE);
		showrect_line(map_4xy);//画地图
		for(i=0;i<=ri;i++)  //画圆点
		{
			LCD_Fill(rx[i]/2-3, ry[i]/2-3,rx[i]/2+3,ry[i]/2+3,RED);
		}
		r_n=ri;
		shownum(190,y0+y1*2,c_W,fontc,backc,y_size,1);//显示W
		shownum(190,y0+y1*3,c_H,fontc,backc,y_size,1);
		for(i=0;i<130;i++)	//延时等待500ms，并检测按键
		{
			delay(5);
			switch(KEY_Scan)
			{
				case Keyup:
					break;
				case Keydown:
					break;
				case Keyright: 
				//重新识别一次新的图像
					UART_PutChar(LPUART2, 'a');//摄像头2 发送识别一次的字符
					showclear(1,1,160-1,120-1,WHITE);
					delay(500);//延时等待500ms
					showrect_line(map_4xy);//画地图
					for(i=0;i<=ri;i++)  //画圆点
					{
						LCD_Fill(rx[i]/2-3, ry[i]/2-3,rx[i]/2+3,ry[i]/2+3,RED);
					}
					r_n=ri;
					shownum(190,y0+y1*2,c_W,fontc,backc,y_size,1);//显示W
					shownum(190,y0+y1*3,c_H,fontc,backc,y_size,1);

					
					break;
				case Keyleft:
					break;
				case Keyclick:
					key_over=1;//退出
					UART_PutChar(LPUART2, 'm');//摄像头2 退出地图模式
					delay(500);
					break;
				default:break;
			}
        //delay(100);	
		}
	}
}

u8 menu_angle(void)
{
	showchinese(x0,Y_max/2,"按→陀螺仪校准",fontc,backc,24,0);
	
    key_over=0;
	while(!key_over)
	{


		switch(KEY_Scan)
		{
			case Keyup:
				break;
			case Keydown:
				break;
			case Keyright: 
				return 1;//开始校准
				break;
			case Keyleft:
				break;
            case Keyclick:
                key_over=1;//退出
				//return 0;
				return 1;//开始校准
                break;
			default:break;
		}
        //delay(100);	
	}
}

void menu_run(void)
{
	//showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
	showstr(x0+x1,Y_max/2-30,"camera",fontc,backc,24,0);
	showchinese(x0+x1,Y_max/2,"开始",s_fontc,s_backc,24,0);
	key_over=0;
	while(!key_over)
	{


		switch(KEY_Scan)
		{
			case Keyup:
				break;
			case Keydown:
				break;
			case Keyright: 
				key_over=1;//开始校准
				break;
			case Keyleft:
				break;
            case Keyclick:
				showclear(0,0,X_max,Y_max,WHITE);   //清空屏幕
                key_over=1;//退出
                break;
			default:break;
		}
        //delay(100);	
	}
}

void Menu_start(void)//菜单显示主程序
{
	menu_p1();//调PID
    menu_p4();//调旋转环
	
	menu_p3();//校准电磁铁
    menu_p2();//显示摄像头1检测矩形中点
	//menu_map();
}

