//保存智能车所有要用的公共数据
//默认单位：mm  1°
#include "car-data.h"

//文件数据：
//****车的坐标
//*****地图坐标
//*****识图数据
//*****跑点数据

/**PID结构:
 位置环 -> 距离-速度环
 角度环 -> 角度-速度环
**/
//********调参点
//全局变量

//====================车的坐标=========================
int car_x = 200;
int car_y = 200;


//车的初始位置
int car_x0 = 100;//车的初始位置，点（1,1）
int car_y0 = -540;//车库	  车长250mm	

//回去车库的坐标
int ku_x=280;
int ku_y=-500;


//====================点的信息=========================
u8 point_i=0;   //点的总数

//**当值为0时，表示这个点失效**
u16 point_X[51];	//点的点位(最大支持50个) 可以用结构体
u16 point_Y[51];

int point_x[51]={0};//真实坐标(单位:mm)
int point_y[51]={0};

//typedef struct {
//    int X; //点位
//    int Y;
//	  int x; //真实坐标
//    int y;
//} Point;

int best_path[51]; //最优的路径顺序

//====================地图坐标========================
//摄像头1：搜索地图及图像
//摄像头2：识别图像

//场地的真实宽高    ->单位：mm	 20cm一格
int map_kuan=25     *20*10;
int map_gao=20      *20*10;

//地图的坐标点数 （识图计算）
int map_Xmax=35;
int map_Ymax=25;    //两个y是真实坐标，一个y是地图坐标

//能跑到的场地点数
int point_Xmax=30;
int point_Ymax=20;


//====================图片数据========================
//图片的大类和小类
u8 pic_dalei=0;
u8 pic_xiaolei=0;


//***公共变量***
//#define s_map 1
//#define s_rect 2
//u8 openmv_mode=1;//openmv模式，模式1识别地图，模式2搜索矩形
u8 rect_a;//识别到矩形的标识符
u16 rect_4xy[8];//矩形的四个顶点
u16 rect_zxy[2];//中点
u16 rect_zxy0[2]={92,69};//设定的中点坐标
u16 rect_zx=88;//92; //矩形应该在车前面的中点（要根据摄像头和实际车改
u16 rect_zy=61;//68;  //默认的搜索矩形中点


u8 line_mode=1;//搬运的模式，1是最近的方向，0是垂直方向



//陀螺仪零值和当前角度
float g_z0=0;//陀螺仪的值有正负
float g_z=90;//默认角度

//u32 m_angle;//磁力计算出的角度值(一圈3600) 单位 0.1°
//float angle_list[3600]={-1};//一周磁力计角度对应的陀螺仪角度（磁力计稳，陀螺仪短时准）

//*******跑点的数据

extern u16 rx[20];//圆点的坐标位置(最大支持20个)
extern u16 ry[20];
extern u8 ri;
u16 r_n=0;//圆点的真正个数

extern u16 map_4xy[8];//接收到的摄像头四角
extern u16 c_W,c_H;//接收到的宽高 //其实用宽就够了
#define ck_b chang/gao
//例:接收2000 1000 真实200 100,则坐标1000 500->100 50(*1/10)
#define real_Wb chang/c_W //真实的宽度比例, 真实宽度x = x * real_Wb
#define real_Hb gao/c_H //等比缩放,高度比例

//#define max_y C_H*real_Hb //y的上限,用来反转y



u8 rect_search=0;//搜索矩形模式
u8 rect_ready=0;//矩形移动成功

//车轮一圈的周长 mm
#define lunzi 19.8

float motor_speed[4];//车轮的速度 单位:mm
float motor_mm[4]={0,0,0,0};   //走过的距离   （用float记录效果好）

//pid模式
u8 pidmode=10;	//pid模式
#define stop 0			//设置pid停止
#define place_mode 1    //位置环pid
#define angle_mode 2	//角度环计算模式
#define speed_mode 3 	//纯速度环，    测试用
#define xmove_mode 4	//横向移动
#define search_mode 5   //校正图片

//extern u8 pic_n;//识别到的图片序号
int start_speed = 0;//缓慢起步的加速度

//extern u8 pic_o;//图片识别成功的标识符
//pic_dalei等于0就是识别失败
extern u8 pic_dalei,pic_xiaolei;//发送的图片大类和小类

//----------------其他--------------
int rand_seed;//随机数种子，（0-10000）在定时器里加
