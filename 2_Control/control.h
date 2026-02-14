#ifndef CONTROL_H
#define CONTROL_H

#include "car-data.h"

// 去往某个点
void go_point(short to_x, short to_y, short ang); // 去目标点，朝的方向
void xymove(float x_mm,float y_mm);//平移移动
void search_move(float x_mm,float y_mm); // 搜索移动

void search_pic(void); // 搜索图片

float get_speed();//获取车的平均车速
void mm_clear();//清理距离信息

//pid运算
void pid_main(void);


#endif