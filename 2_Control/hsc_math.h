#ifndef _HSC_MATH_H
#define _HSC_MATH_H

#include "hsc.h"	//定义变量类型的头文件
#include "math.h"
//智能车要用到的数学函数		//单位为:mm

#define PI 3.14159	//定义的π值
#define ABS(x) ((x)>0?(x):-(x))		//求绝对值 自带的abs只能求整数

//字符串处理的函数	（输入的字符串，字符串个数，输出的变量）
u8 split_Chars(u8 *get, u8 i, short *c);

//计算两点之间的距离
u16 distance(int x1, int y1, int x2, int y2);

//计算两点之间的角度(且是最近的）（比如225°是-135°）
float xy_angle(int x1, int y1, int x2, int y2);

///计算最近的点
//输入:原点坐标,各个点坐标的数组名(指针),点的个数
u16 distance_mins(int x0, int y0, int *rx, int *ry, u8 i);

//最小的旋转角度
float min_angle(float set_a,float get_a);//当前角，目标角
	
//最近的朝向
u8 min_dir(int x0, int y0, int to_x, int to_y, u16 d);

//两个点一条直线的最短距离（将军饮马）
//先到直线，再到另一个点
int min_line(int x0, int y0, int to_x, int to_y, int line_xy,u16 n);//n=0左，n=1上，n=2右

//滑动滤波
float slide_filter1(float raw);
float slide_filter2(float raw);
float slide_filter3(float raw);


#endif