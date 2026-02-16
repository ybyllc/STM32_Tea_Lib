/**
 ******************************************************************************
 * @file		智能车要用到的数学函数
 * @author		一杯原谅绿茶 胡书畅 qq：420752002
 * @brief 功能：处理字符串
 *         计算两点间的距离
 *         计算两点间的角度
 *         找出最近的那个点
 *****************************************************************************/

#include "math.h"
#include "tea_math.h"
	
#define ABS(x) ((x)>0?(x):-(x))		//求绝对值

////***公用变量

//**********字符串处理函数*******
//res_Buff(str,i,c)
//把{10,20,-30} 变为：
// c[0]= 10
// c[1] = 20
// c[2] = -30

u8 split_Chars(u8 *get, u8 i, short *c)	//(字符串指针，字符个数，输出的变量数组)
{
	char over = '}';//结束符
	char fen  = ' ';//分割符
	
    u8 n = 0;    //变量的个数
    short pos_sign = 1; //这个数的正负符号
	
	u8 j;	
    //把c数组清零,"10"是c的个数
    for (j = 0; j < 10; j++)
    {
        *(c + j) = 0;
    }

    for (j = 0; j < i; j++) //例：把“1 0 0 0 0”变成10000
    {
        //判断这个字符是否是分隔符或结束符
        if (*(get + j) != fen && *(get + j) != over && *(get + j) != '\n' && *(get + j) != '\r' && *(get + j) != '\0')
        {
            if (*(get + j) == '-') //检测到-号，变量变负数
                pos_sign = -1;
            if (*(get + j) >= '0' && *(get + j) <= '9')//检测到数字
                *(c + n) = *(c + n) * 10 + (short)(*(get + j) - '0'); //字符转化为数字
            //字符串里的其他字符就无视，比如括号(
        }
        else
        {
            c[n] *= pos_sign; //数据设置正负
            n++;       		  //数字个数+1
            pos_sign = 1;     //符号恢复成正号
        }
    }

    return n;
}


//******计算两点之间的距离******
u16 distance(int x1, int y1, int x2, int y2) //输入单位是mm
{
    float d;
    d = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); // d=根号（x2-x1）^2+（y2-y1）^2
    return (u16)(d + 0.5);                                   //四舍五入输出
}


//*****计算两点的角度***** (最近的） （比如225°是-135°）
float xy_angle(int x1, int y1, int x2, int y2)
{
    float ang;
    ang = (float)atan2((y2 - y1), (x2 - x1)) * 180 / PI; //计算两点的正切值，并转换成角度
	
	//本来是x轴的角度是0度
	//改为车朝前的角度是0度
	ang = -ang+90;
    return ang;
}


//********计算最近的点***********(贪心算法)
//输入:原点坐标,各个点坐标的数组名(指针),点的个数
//输出:最近的点的序号
//圆点数组:
// rx[]={1, 3, 5};
// ry[]={1, 3, 5}; //两个一一对应
//这是3个点
// max = distance_rr(x,y,rx,ry,i

u16 distance_mins(int x0, int y0, int *rx, int *ry, u8 i)
{
    // distance(x0, y0, &(rx+j),&(ry+j));
    //从(1,1)开始,所以设(-1,-1)点是无效点
    // if(&(rx+j)!=0 && &(ry+j)!=0);

    u8 min = 0;//最小距离的点  //这里不能是。。某个点
    u8 j;
    for (j = 0; j < i; j++)
    {
        if (*(rx + j) != -1 && *(ry + j) != -1)//不是无效点
        {
            if (distance(x0, y0, *(rx + min), *(ry + min)) > distance(x0, y0, *(rx + j), *(ry + j)))
            {
                min = j;
            }
        }
        else if(min==j) min++;//排除掉最开始的无效点是最小点的情况
    }
	if(min==i) return 0;//全部点跑完的情况
    else return min;
}



//****最小的旋转角****
float min_angle(float set_a,float get_a)//输入角，目标角
{
    float min = set_a;//最小的角度
	u8 j;
	float kx1,kx2;
    for (j = 0; j < 6; j++)
    {
		kx1=j*360+set_a;//正的加角
		kx2=-j*360+set_a;//负的加角
		if(ABS((int)(kx1-get_a))<ABS((int)(min-get_a)))	//abs只能用于整数的绝对值
		{
			min = kx1;
		}
		if(ABS((int)(kx2-get_a))<ABS((int)(min-get_a)))
		{
			min = kx2;
		}
    }
    return min;
}

//****最近的朝向****
//车的坐标，要去的坐标。上下左右的间距		//返回值：车应该的朝向 0直 1右 2后 3左
u8 min_dir(int x0, int y0, int to_x, int to_y, u16 d)
{
	u16 D[4];//四个朝向的距离
	D[0] = distance(x0, y0, to_x, to_y-d);//车在图片下方
	D[1] = distance(x0, y0, to_x-d, to_y);//车在图片左边
	D[2] = distance(x0, y0, to_x, to_y+d);//车在图片上方
	D[3] = distance(x0, y0, to_x+d, to_y);//车在图片右边
	u16 min=D[0];
    u8 mind = 0;//最小距离的方向
	u8 j;
	for(j=0;j<4;j++)
	{
		if(D[j]<min)
		{
			min=D[j];
			mind=j;
		}
	}
	return mind;
}

//两个点一条直线的最短距离（将军饮马）
//先到直线，再到另一个点
int min_line(int x0, int y0, int to_x, int to_y, int line_xy,u16 n)//n=1左，n=2上，n=3右
//如果输入x值的线，则输出y轴坐标
{
    int to_x1,to_y1;//镜像的点
    float ang1;
    if(n==1)    //输入的line_xy是x轴值
    {
        u16 d2;//到线的距离
        u16 d;//目标点的y轴距离
        int to_line_y;
        to_x1=line_xy-(to_x-line_xy);//镜像的点
        to_y1=to_y;
        ang1=xy_angle(x0,y0,to_x1,to_y1);
        d2=((float)ABS(x0-line_xy)/ABS(x0-to_x1))*distance(x0,y0,to_x1,to_y1);
        d=d2*sin(ang1/180*PI);//这里要输入角度？
        to_line_y=y0+d;
        return to_line_y;
    }
    else if(n==2)
    {
        u16 d2;//到线的距离
        u16 d;//目标点的y轴距离
        int to_line_x;
        to_x1=to_x;
        to_y1=line_xy-(to_y-line_xy);
        ang1=xy_angle(x0,y0,to_x1,to_y1);
        d2=((float)ABS(y0-line_xy)/ABS(y0-to_y1))*distance(x0,y0,to_x1,to_y1);//比例*distance
        d=d2*cos(ang1/180*PI);//这里要输入角度？
        to_line_x=x0+d;
        return to_line_x;
    }
    else if(n==3)   //和n=1时的情况一样
    {
        u16 d2;//到线的距离 65536 6553cm 65m
        u16 d;//目标点的y轴距离
        int to_line_y;
        to_x1=line_xy-(to_x-line_xy);
        to_y1=to_y;
        ang1=xy_angle(x0,y0,to_x1,to_y1);
        d2=((float)ABS(x0-line_xy)/ABS(x0-to_x1))*distance(x0,y0,to_x1,to_y1);
        d=d2*sin(ang1/180*PI);//这里要输入角度？
        to_line_y=y0+d;
        return to_line_y;
    }
	
}

//滑动滤波(2个)
#define FILTER_LENGTH 10 // 滤波宽度
float slide_filter1(float raw)
{
    static float filterBuf[FILTER_LENGTH] = {0.0f}; // 静态滤波缓冲区，已初始化
    static uint8_t i = 0; // 位置索引
    static float out = 0.0f; // 输出值
    
    out -= filterBuf[i] / FILTER_LENGTH; // 移除旧值影响
    filterBuf[i] = raw; // 添加新值到缓冲区
    out += filterBuf[i] / FILTER_LENGTH; // 加入新值影响
    
    i++; // 更新索引
    if (i == FILTER_LENGTH) i = 0; // 索引循环回零
    
    return out;
}

float slide_filter2(float raw)
{
    static float filterBuf[FILTER_LENGTH] = {0.0f}; // 静态滤波缓冲区，已初始化
    static uint8_t i = 0; // 位置索引
    static float out = 0.0f; // 输出值
    
    out -= filterBuf[i] / FILTER_LENGTH; // 移除旧值影响
    filterBuf[i] = raw; // 添加新值到缓冲区
    out += filterBuf[i] / FILTER_LENGTH; // 加入新值影响
    
    i++; // 更新索引
    if (i == FILTER_LENGTH) i = 0; // 索引循环回零
    
    return out;
}

float slide_filter3(float raw)
{
    static float filterBuf[FILTER_LENGTH] = {0.0f}; // 静态滤波缓冲区，已初始化
    static uint8_t i = 0; // 位置索引
    static float out = 0.0f; // 输出值
    
    out -= filterBuf[i] / FILTER_LENGTH; // 移除旧值影响
    filterBuf[i] = raw; // 添加新值到缓冲区
    out += filterBuf[i] / FILTER_LENGTH; // 加入新值影响
    
    i++; // 更新索引
    if (i == FILTER_LENGTH) i = 0; // 索引循环回零
    
    return out;
}