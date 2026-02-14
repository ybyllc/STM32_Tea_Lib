/**
  ******************************************************************************
  * @file		PID文件
  * @author		by一杯原谅绿茶  qq：420752002
  * @brief   	特性：支持了I积分限幅，和积分分离（积分到一定幅度再打开）
                对于PID， 得到反馈一般叫get/measure/real,
                期望输入一般叫set/target/ref
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hsc_pid.h"


//***创建的变量在这
// 可以增加车轮数和环数

// pid位置环   (位置->速度		
pid_data_x pid_data_pla[4];      // 电机的PID数据存储数组	  每个轮子的数据
// 数据：顺序是(P,I,D,set)
pid_t pid_pla[4]; // 当前转速,目标转速

// pid角度环  (角度->速度		
pid_data_x pid_data_ang[2];      // 电机的PID数据存储数组	  有两个环：角度环和前进时的角度环	
pid_t pid_ang[2]; // 当前转速,目标转速

// pid速度环  (速度->PWM
pid_data_x pid_data_spd[4];    // 电机的PID数据存储数组
pid_t pid_spd[4]; // 当前转速,目标转速

// pid搜索环  (速度->PWM
pid_data_x pid_data_sch[4];    // 电机的PID数据存储数组
pid_t pid_sch[4]; // 当前转速,目标转速



u8 ISA, delta; // 积分分离开启模式
/*!
 *  @brief      pid输出限幅（限制在最大值）
 *  @param     上下限幅out值在+ABS_MAX和-ABS_MAX之间
 *  @note
 *  Sample usage:my_abs(a,100)   意为a被限幅在正负100之间
 */
void my_abs(float *out, float ABS_MAX)
{
    if (*out > ABS_MAX)
        *out = ABS_MAX;
    if (*out < -ABS_MAX)
        *out = -ABS_MAX;
}

/*!
 *  @brief      pid参数初始化   供后面的pid参数初始化使用
 *  @param      mode 模式  maxput 最大输出 Kp Ki Kd  pid参数
 *  @note
 *  Sample usage:      pid_init(pid_data_x *pid, Delta_Pid, 100,70,2,0.1,1);    //初始化 pid参数 为增量式输出  最大输出值为100 积分项限幅为70  P为2 I为0.1 D为1
 */
static void pid_init(
    pid_data_x *pid,
    unsigned int mode,
    unsigned int maxout,
    unsigned int intergral_limit,
    unsigned int intergral_sign,
    float kp,
    float ki,
    float kd)
{
    pid->IntegralLimit = intergral_limit;
    pid->IntegralSign = intergral_sign,
    pid->MaxOutput = maxout;
    pid->pid_mode = mode;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

/*!
 *  @brief      pid初始化
 *  @param      mode 模式  maxput 最大输出 Kp Ki Kd  pid参数
 *  @note
 *  Sample usage:   pid_init(pid_data_x *pid, Delta_Pid, 100,70,2,0.1,1);    //初始化 pid参数 为增量式输出  最大输出值为100 积分限幅为70  积分分离转速值8(积分项开开启差距)  P为2 I为0.1  D为1
 *  上限为65536
 */
void PID_struct_init(
    pid_data_x *pid,
    unsigned int mode,
    unsigned int maxout,
    unsigned int intergral_limit,
    unsigned int intergral_sign,

    float kp,
    float ki,
    float kd)
{
    pid->f_param_init = pid_init;
    pid->f_pid_reset = pid_reset;
    pid->f_param_init(pid, mode, maxout, intergral_limit, intergral_sign, kp, ki, kd);
}

/*!
 *  @brief      pid中途调参（中途修改		多了一个设置输出限幅
 *  @param
 *  @since      v1.0
 *  @note
 *  Sample usage:    pid_reset(&pidsd[1],1,2,3);    //pid调参，电机2更新p为1 i为2  d为3
 */

/*中途更改参数设定(调试)------------------------------------------------------------*/
void pid_reset(pid_data_x *pid, float kp, float ki, float kd, unsigned int maxout)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
    pid->MaxOutput = maxout;
    pid->last_delta_out = 0;
    pid->last_pos_out = 0;
    pid->delta_out = 0;
    pid->err[0] = 0;
    pid->err[1] = 0;
    pid->err[2] = 0; // 全部归零
}

/*!
 *  @brief      pid计算
 *  @param    get 实际速度   set预期速度
 *  @since      v1.0
 *  @note
 *  Sample usage:  pid_calc(&pid_data_spd[1], chassis.pid_get[1], chassis.target[1]);//电机2的参数计算get值和预期值
 */

float pid_calc(pid_data_x *pid, float get, float set)
{
    pid->get[NOW] = get;
    pid->set[NOW] = set;
    pid->err[NOW] = set - get; // set - measure

    delta = pid->err[NOW] > 0 ? pid->err[NOW] : -pid->err[NOW];
    if (delta > pid->IntegralSign && pid->IntegralSign !=0)
        ISA = 0;
    else
        ISA = 1;

    if (pid->pid_mode == Position_Pid) // 位置式PID    会影响初始值
    {
        pid->pout = pid->p * pid->err[NOW];                    // p项计算
        pid->iout += pid->i * pid->err[NOW];                   // i项计算
        pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]); // d项计算

        pid->iout *= ISA;

        my_abs(&(pid->iout), pid->IntegralLimit); // 积分项限幅
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        my_abs(&(pid->pos_out), pid->MaxOutput); // 总输出限制

        pid->last_pos_out = pid->pos_out; // update last time
    }
    else if (pid->pid_mode == Delta_Pid) // 增量式PID     常用
    {

        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);                       // 此次偏差
        pid->iout = pid->i * pid->err[NOW];                                          // 积分，累计偏差
        pid->dout = pid->d * (pid->err[NOW] - 2 * pid->err[LAST] + pid->err[LLAST]); // 微分，偏差速率

        // pid->isum += pid->iout;//把积分项分离出来
        // pid->isum *= ISA;		//并判断要不要归零

        my_abs(&(pid->iout), pid->IntegralLimit);
        pid->out = pid->pout + pid->iout + pid->dout;
        pid->delta_out = pid->last_delta_out + pid->out;
        my_abs(&(pid->delta_out), pid->MaxOutput);

        pid->last_delta_out = pid->delta_out; // update last time
    }

    // 更新状态
    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST] = pid->err[NOW];
    pid->get[LLAST] = pid->get[LAST];
    pid->get[LAST] = pid->get[NOW];
    pid->set[LLAST] = pid->set[LAST];
    pid->set[LAST] = pid->set[NOW];

    // 返回输出pid值
    return (pid->pid_mode == Position_Pid) ? pid->pos_out : pid->delta_out;
}

// 示例使用函数
/*main.c文件里用的初始化代码
    extern pid_data_x pid_data_spd[4] ;//电机的PID数据存储数组
    extern pid_t chassis;//当前转速,目标转速
    u16 P,I,D;//PID变量

    //PID初始化
    P=74;//开始猛
    I=56;//不超过40
    D=23;//跑出的参数 3.2 0.8 2.3	3.3 0.2 1.2   2.5 0.2 1.4   1.0 0 0.5    3.7 1 0   100 40 10   46 37 15（积分分离22精髓）74 39 39（很猛） 74 37 28   74 56 60/23
//	P=2.3;
//	I=1.7;//(?)
//	D=1.1;//跑出的参数 	原始参数3，0.1，1
    chassis.target[0]=49;//初始目标转速 PI过大，单数稳定
    PID_struct_init(&pid_data_spd[0],Position_Pid,3000,2500,228,P,I,D); //初始化 pid参数 为增量式输出  最大输出值为100 积分项限幅为90 积分分离转速值8  P为2 I为0.1 D为1 每分最大输出值改成6000
*/

/*tim.c定时器文件里使用的代码
extern pid_data_x pid_data_spd[4] ;//电机的PID数据存储数组
extern pid_t chassis;//当前转速,目标转速
extern u16 P,I,D;//PID变量

u16 count=0;
u16 now_count=0;//当前触发数
u16 last_count=0;//当前触发数
u16 speed=0;
float speed1=0;//两次平均值
u8 a=0;

//转速获取（输入捕获）
            count++;//一个高脉冲


//PID计算->修改pwm的值
            //计算当前速度
            //last_count=now_count;//这次变成上次
            now_count=count;		//最新一次
            count=0;
            speed=now_count;			//转速（刚好也是每秒圈数）96是一圈，刚好定时器是96hz 每秒转速


            chassis.pid_get[0]=speed;
            PID_send_data((int)chassis.pid_out[0],(int)speed*60,(int)chassis.target[0]*60,(int)(pid_data_spd[0].iout),(int)(I),(int)(D));//发送上一次pid生效的数据，每分转速
        //pid算法输出
            if(chassis.pid_get[0]!=chassis.target[0]+1)//相等时，pid值不变
                chassis.pid_out[0] = pid_calc(&pid_data_spd[0], chassis.pid_get[0], chassis.target[0])+2;//输出到电机,get输入值，target预期值    校准


            if(chassis.pid_out[0]<0)
                chassis.pid_out[0]=0;

            if(chassis.pid_out[0]<=0){
                LED0_PWM_VAL=6000;
            }
            else
                 {
                     LED0_PWM_VAL=6000-6000*(chassis.pid_out[0]/3000.0);//输出PWM占空比，pwm宽度为6000
                 }
*/
