#include "gryo.h"

//**********能输出的变量****************
float car_ang=0;  //车的角度（初始0度，向前）

float car_ax=0;	//加速度
float car_ay=0;
float car_az=0;

float Gyro_z0;  //零飘
float Gyro_z;	//陀螺仪的当前值  （初始0度


//加速度计平均滤波
float Filter_accx(int x);
float Filter_accy(int x);
float Filter_accz(int x);

u8 Gryo_start=0; //陀螺仪启动状态
//初始化imu963陀螺仪
void Gryo_Init()
{
    //初始化
    while(imu963ra_init())
        printf("\r\nIMU963RA init error."); 
	Gryo_start=1;
	
	//一定时间未检测到,跳过
//	for(int i=0;i<100;i++)
//	{
//		if(imu963ra_init()==0)
//		{
//			Gryo_start=1;
//			break;
//		}
//		delayms(5);
//	}
		
	
}

//把零飘计算独立出来，让陀螺仪有更长时间等待车的稳定
void Gryo_z0_cal()
{
	if(Gryo_start==1) //陀螺仪成功初始化
	{
		static float Sum_gyro=0; // 角速度数组   0漂总和
		
		static float Sum_accx=0; // 加速度数组   0漂总和
		static float Sum_accy=0; // 加速度数组   0漂总和
		static float Sum_accz=0; // 加速度数组   0漂总和
		
		start_z0:
		delayms(500); //等车静止

		//零飘更取决于总计算时间，和初始状态时车的抖动
		//计算零飘    （50次平均，每次10ms）
		Sum_gyro=0;
		int max_z0,min_z0;			//最大值，最小值。用来计算抖动
		max_z0 = min_z0= imu963ra_gyro_z;
		for(u16 i=0;i<10;i++)
		{
			imu963ra_get_gyro(); // 获得角速度
			Sum_gyro += imu963ra_gyro_z; // 角速度数组存一次角速度
			delayms(50);
			
			if(imu963ra_gyro_z>max_z0) max_z0 = imu963ra_gyro_z;
			if(imu963ra_gyro_z<min_z0) min_z0 = imu963ra_gyro_z;
		}

		Gyro_z0 = Sum_gyro / 10.0;
		
	//（抖动检测），重新校正 (静止状态下，值在1-3浮动)
		if (abs(max_z0 - min_z0)>5) goto start_z0;
		
	}
}

//不带滤波的陀螺仪数据
void Get_Ang()
{
    imu963ra_get_gyro(); // 获取一次角速度
	//减去零飘
	Gyro_z = ((float)imu963ra_gyro_z - Gyro_z0)/2857;//;   //(/14.285 *0.005);	*0.00035
									// 14.285换算成度0.07  0.005定时器时间（改定时器后，要改这里）
	
	//陀螺仪反向安装
	car_ang += -Gyro_z; // (Cal_gyro_z-Gyro_z0)/714.25; 
	
	//限幅到 ±360° 以内
//	if (car_ang >= 360)
//		car_ang = car_ang - 360;
//	if (car_ang <= -360)
//		car_ang = car_ang + 360;

}

//窗口平均滤波——前20个平均值
float Filter_accx(int x)
{
	u8 L=20;
	static int F_accx[20],i = 0;
	int F_accx_sum=0;
	
	F_accx[i] = x;
	
	if(i<19) i++;
	else i = 0;
	//循环取20个值
	
	for(u8 j=0;j<20;j++)
	{
		F_accx_sum +=F_accx[j];//平均值
	}
	
	return (float)F_accx_sum/20;
	
}

float Filter_accy(int x)
{
	u8 L=20;
	static int F_accx[20],i = 0;
	int F_accx_sum=0;
	
	F_accx[i] = x;
	
	if(i<19) i++;
	else i = 0;
	//循环取20个值
	
	for(u8 j=0;j<20;j++)
	{
		F_accx_sum +=F_accx[j];//平均值
	}
	
	return (float)F_accx_sum/20;
	
}

float Filter_accz(int x)
{
	u8 L=20;
	static int F_accx[20],i = 0;
	int F_accx_sum=0;
	
	F_accx[i] = x;
	
	if(i<19) i++;
	else i = 0;
	//循环取20个值
	
	for(u8 j=0;j<20;j++)
	{
		F_accx_sum +=F_accx[j];//平均值
	}
	
	return (float)F_accx_sum/20;
	
}

//=========================备用函数==============

int Turn_over = 0;	// 转向结束标志位

/***

float Acc_x;	//加速度计的当前值
float Acc_y;	
float Acc_z;	
float Acc_x0;	//加速度计的零飘
float Acc_y0;	
float Acc_z0;	

int Gyro_i = 0;		// 角速度数组下标
int Turn_over = 0;	// 转向结束标志位
int Gyro0_flag = 0; // 0漂计算标志位

float IMU963RA_FIFO[10];
static int Gyro_flag;			  // 0漂计算标志位  角速度滤波标志位

//带滤波的获取陀螺仪数据
void Gyro_Filter()
{

	float Sum = 0;					  // 递推均值总和

	imu963ra_get_gyro(); // 获得一次角速度


    // ips114_show_string(170,90,"Z0_OVER!");
    Gyro_z = (imu963ra_gyro_z - Gyro_z0) / 14.285;		//减去零飘

    if (abs(Gyro_z) < 1) // 角速度小于1时  默认为小车静止
    {
        Gyro_z = 0;
        Turn_over = 1; // 转向结束
    }
    else
        Turn_over = 0; // 转向未结束

    //滤波
    for (Gyro_flag = 1; Gyro_flag < 10; Gyro_flag++)
    {
        IMU963RA_FIFO[Gyro_flag - 1] = IMU963RA_FIFO[Gyro_flag]; // FIFO 操作
    }
    IMU963RA_FIFO[9] = Gyro_z;
    for (Gyro_flag = 0; Gyro_flag < 10; Gyro_flag++)
    {
        Sum += IMU963RA_FIFO[Gyro_flag]; // 求当前数组的合，再取平均值
    }
    Gyro_z = Sum / 1000; // 10个数 100时间系数  /10/100
	
}


***/


