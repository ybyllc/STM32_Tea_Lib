#ifndef _HSC_MENU_1_h_
#define _HSC_MENU_1_h_
#include "stdint.h"

// extern u8 keynum;
// extern u8 readytorun;
void Menu_start(void);
void menu_map(void);//测试识别地图
u8 menu_angle(void);//陀螺仪校准，返回1
void menu_run(void);//开始，空函数
void menu_motor_test(void);

void menu_p4(void);//调节旋转环

//画图函数
//画十字函数“+”     （中点）
//void showcross(u16 x,u16 y,u16 color);
//用四条线画矩形    (四个点的数组)
void showrect_line(u16 r_xy[8]);
#endif
