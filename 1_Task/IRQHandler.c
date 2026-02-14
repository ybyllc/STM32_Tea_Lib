

//#include "main.h"
//#include "hsc.h"
//#include "hsc_math.h"
//#include "PID.h"

//typedef struct
//{
//   	float kp, ki, kd; //三个系数
//    float error, lastError; //误差、上次误差
//    float integral, maxIntegral; //积分、积分限幅
//    float output, maxOutput; //输出、输出限幅
//}PID;
// 
////用于初始化pid参数的函数
//void PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOut)
//{
//    pid->kp = p;
//    pid->ki = i;
//    pid->kd = d;
//    pid->maxIntegral = maxI;
//    pid->maxOutput = maxOut;
//}
// 
////进行一次pid计算
////参数为(pid结构体,目标值,反馈值)，计算结果放在pid结构体的output成员中
//void PID_Calc(PID *pid, float reference, float feedback)
//{
// 	//更新数据
//    pid->lastError = pid->error; //将旧error存起来
//    pid->error = reference - feedback; //计算新error
//    //计算微分
//    float dout = (pid->error - pid->lastError) * pid->kd;
//    //计算比例
//    float pout = pid->error * pid->kp;
//    //计算积分
//    pid->integral += pid->error * pid->ki;
//    //积分限幅
//    if(pid->integral > pid->maxIntegral) pid->integral = pid->maxIntegral;
//    else if(pid->integral < -pid->maxIntegral) pid->integral = -pid->maxIntegral;
//    //计算输出
//    pid->output = pout+dout + pid->integral;
//    //输出限幅
//    if(pid->output > pid->maxOutput) pid->output =   pid->maxOutput;
//    else if(pid->output < -pid->maxOutput) pid->output = -pid->maxOutput;
//}


//PID mypid = {0}; //创建一个PID结构体变量
//float pidout=0;

////定时器中断 处理函数
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance == TIM2)	//定时器2 20ms中断
//	{
////		static u8 a=1;
////		if(a==1)
////		{
////			a=0;
////			PID_Init(&mypid, 5, 0, 5, 0, 800);
////		}
////		Get_Ang(); // 获取陀螺仪角度
//// 	PID_Calc(&mypid, 90, fAngle[2]);
////		pidout=(mypid.output/mypid.output+500)+mypid.output/3;
////		Motor_Set(-pidout,pidout,-pidout,pidout);
//		
//		Get_Ang(); // 获取陀螺仪角度
//		if(ABS(fAngle[2]-90)>2)
//		{
//			if(fAngle[2]<90)
//				Motor_Set(-630,630,-630,630);
//			if(fAngle[2]>90)
//				Motor_Set(630,-630,630,-630);
//		}
//		else Motor_Set(0,0,0,0);
//		
//		//pid_cal(1);//PID 启动
//		//
//		static u8 i=0;
//		i++;
//		if(i==10)
//		{
//			i=0;
//			printf_DMA(&huart1,"%.1f,%.1f,%.1f\r\n",fAngle[2],fAcc[0]*100,fAcc[1]*100);
//		}
//		

//	}
//}