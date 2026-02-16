#ifndef _CAR_DATA_H
#define _CAR_DATA_H

//***************导入库函数************
//来自逐飞的基础库

#include "common.h" //hsc的基础头文件
#include "motor-m.h"	//电机驱动
#include "wit_gyro_sdk.h"	//维特陀螺仪

#include "tim.h"
//#include "hsc_pid.h"	//pid计算
//#include "hsc_math.h"	//数学函数

//#include "main.h"
//#include "test.h"  //测试文件
//#include "car-m.h"//car总控制

//#include "control.h"	//操作控制
//#include "PID.h"		//PID函数
//#include "box.h"		//拾取放到盒子
//#include "others.h"		//除了运动以外的控制
////#include "ACO.h"		//路径规划算法

////菜单界面
////#include "Menu_main.h"
////----------驱动文件-----------------
//#include "motor-m.h"	//电机驱动
//#include "encoder.h"	//编码器
//#include "servo.h"		//舵机驱动
////#include "lcd.h"		//屏幕驱动
//#include "key.h"		//按键驱动

//#include "openmv_1.h"		//摄像头1驱动

//本程序统一坐标单位：毫米（mm）
//====================车辆的坐标=========================
//#define car_d 210 //车伸长摄像头的距离 //车距离点的距离
//extern int car_x;//车的当前真实位置（mm）
//extern int car_y;

//extern float car_ang;  //车的当前角度（初始90度）

//extern float car_ax; //车的加速度计
//extern float car_ay;
//extern float car_az;

////====================点的信息=========================

////点的总数
//extern u8 point_i;   
////点的坐标
//extern u16 point_X[51];//点的点位(最大支持20个)
//extern u16 point_Y[51];

//extern int point_x[51];//真实坐标 (单位:mm)
//extern int point_y[51];

//extern int best_path[51]; //最优的路径顺序

////====================地图坐标=========================

////场地的真实宽高    ->单位：mm	 20cm一格
//extern int map_kuan;
//extern int map_gao ;

////地图的坐标点数 （识图计算）
//extern int map_Xmax;
//extern int map_Ymax;

////能跑到的场地点数
//extern int point_Xmax;
//extern int point_Ymax;

////====================图片数据=========================

//extern int clas; //类别
//extern u8 pic_dalei;
//extern u8 pic_xiaolei;

////搜索图像
//extern int Tar_x;	//图像的中心点
//extern int Tar_y;	

//extern short rx_pt[21][2];//接收到的点坐标
//extern short rx_pt_num;  //接收到的点数

////====================电机的参数=========================

////车轮一圈的周长 mm
//#define lunzi 198
////*轮子与编码器参数*
//extern int	 Encoder[4];	//编码器的值
//extern float motor_speed[4];//轮速
//extern float motor_mm[4];//走过的距离  (不能用int，因为在5ms的计算下，误差会很大)
////车的平均速度
//#define car_speed  (abs(motor_speed[1])+abs(motor_speed[2])+abs(motor_speed[3])+abs(motor_speed[4]))/4


////====================================================
////====================PID参数=========================

////pid计算模式
//extern u8 pidmode;	//pid模式
//#define stop 0			//设置pid停止
//#define place_mode 1    //位置环pid
//#define angle_mode 2	//角度环计算模式
//#define speed_mode 3 	//速度环，测试和寻矩形用

//#define xmove_mode 4	//横向移动
//#define search_mode 5   //校正图片

//extern int slow_start;//缓慢起步的标识符

////PID参数
////位置环的PID
//extern float	p_P;//只决定开始减速的距离 300mm
//extern float	p_I;
//extern float	p_D;
//extern u16		p_PIDmax;//*** 小车前进的速度 每秒mm ** //极限4300

////矫正环的PID
//extern float	a1_P;	   //输入 1°
//extern float	a1_I;
//extern float	a1_D;
//extern u16		a1_PIDmax;//旋转速度限制  每秒速度mm


////角度环的PID
//extern float	a_P;	   //输入 1°
//extern float	a_I;
//extern float	a_D;
//extern u16		a_PIDmax;//旋转速度限制  每秒速度mm

////速度环的PID
//extern float	P[4];  //2
//extern float	I[4];  //0.3
//extern float	D[4];  //2
//extern u16		PIDmax;//电机PWM限幅 上限10000

////矫正环的PID
//extern float 	s_P[4]; 
//extern float 	s_I[4];  
//extern float 	s_D[4];
//extern u16		search_max;//电机PWM限幅

////PID数据
//extern pid_data_x pid_data_pla[4];  //位置环的数据 , pid的设置
//extern pid_t pid_pla[4];			//转速信息 
//	
//extern pid_data_x pid_data_ang[2];  //角度环
//extern pid_t pid_ang[2]; 
//	
//extern pid_data_x pid_data_spd[4];  //速度环
//extern pid_t pid_spd[4];

//extern pid_data_x pid_data_sch[4];  //搜索环
//extern pid_t pid_sch[4];

//extern u16	PIDmax1[4];//电机PWM前进限幅 决定加速度 上限10000
//extern int	PIDmax0[4];//电机PWM后退限幅 决定后退加速度 上限10000

////----------------其他--------------
//extern int rand_seed;//随机数种子，（0-10000）在定时器里加

#endif