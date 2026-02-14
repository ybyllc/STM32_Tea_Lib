#ifndef __HSC_PID_H__ // 如果未定义
#define __HSC_PID_H__

// hsc_pid的头文件，内含函数声明，变量声明等
/* Includes ------------------------------------------------------------------*/
#include "hsc.h" //定义变量类型的头文件
#include <math.h>

/*参数初始化--------------------------------------------------------------*/
enum
{ // 枚举，类似于#define  后续枚举成员的值在前一个上加 1
    LLAST = 0,
    LAST = 1,
    NOW = 2,

    Position_Pid,
    Delta_Pid,
};

// 用来储存所有数据的结构体
// 名称叫pid_data_x
typedef struct __pid_data_x // pid数据 结构体定义
{
    float p;
    float i;
    float d;

    float set[3]; // 目标值,包含NOW， LAST， LLAST上上次
    float get[3]; // 测量值
    float err[3]; // 误差

    float pout; // p输出
    float iout; // i输出
    int isum;   // i项总和，供积分限幅用
    float dout; // d输出
    float out;  // pid输出

    float pos_out;        // 本次位置式输出
    float last_pos_out;   // 上次输出
    float delta_out;      // 本次增量值
    float last_delta_out; // 本次增量式输出 = last_delta_out + delta_u

    unsigned int pid_mode;
    unsigned int MaxOutput;     // 输出限幅
    unsigned int IntegralLimit; // 积分限幅
    unsigned int IntegralSign;  // 积分分离转速值

    void (*f_param_init)(struct __pid_data_x *pid, // PID参数初始化
                         unsigned int pid_mode,
                         unsigned int maxOutput,
                         unsigned int integralLimit,
                         unsigned int integralSign,

                         float p,
                         float i,
                         float d);
    void (*f_pid_reset)(struct __pid_data_x *pid, float p, float i, float d, unsigned int maxout); // pid三个参数修改

} pid_data_x;

// 定义pid_t结构体
typedef struct
{
    float pid_get; // 输入每秒转速
    float pid_out; // 输出值
    float target;      // 目标转速
} pid_t;

// 函数声明部分

void PID_struct_init(
    pid_data_x *pid,
    unsigned int mode,
    unsigned int maxout,
    unsigned int intergral_limit,
    unsigned int intergral_sign,

    float kp,
    float ki,
    float kd);

static void pid_init(
    pid_data_x *pid,
    unsigned int mode,
    unsigned int maxout,
    unsigned int intergral_limit,
    unsigned int intergral_sign,
    float kp,
    float ki,
    float kd);

float pid_calc(pid_data_x *pid, float get, float set); // pid计算
void my_abs(float *out, float ABS_MAX);
void pid_reset(pid_data_x *pid, float kp, float ki, float kd, unsigned int maxout); // 静态函数不能被其它文件调用，作用于仅限于本文件


//======================公共数据===========================

extern pid_data_x pid_data_pla[4];  //位置环的数据 , pid的设置
extern pid_t pid_pla[4];			//转速信息 
	
extern pid_data_x pid_data_ang[2];  //角度环
extern pid_t pid_ang[2]; 
	
extern pid_data_x pid_data_spd[4];  //速度环
extern pid_t pid_spd[4];

extern pid_data_x pid_data_sch[4];  //搜索环
extern pid_t pid_sch[4];
//************************************


#endif
