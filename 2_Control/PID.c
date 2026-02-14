//PID的功能
#include "PID.h"
#include "main.h"
#include "car-data.h"
#include "hsc_math.h"

//包含：
//4个电机的PID的计算
//陀螺仪两个环的计算（原地旋转和直线矫正）
//加入缓慢加速
//位置环的PID
//float	p_P=5.5;//横走3  //只决定开始减速的距离 300mm
//float	p_I=0;
//float	p_D=10;//1
//u16		p_PIDmax=2000;//*** 小车前进的速度 每秒mm ** //极限4300

//角度环的PID
float	a1_P=6;	   //输入 1°
float	a1_I=0;
float	a1_D=5;
u16		a1_PIDmax=600;//旋转速度限制  每秒速度mm

////旋转环的PID
//float	a_P=40;//50	   //输入 1°
//float	a_I=0;
//float	a_D=100;
//u16		a_PIDmax=1300;//旋转速度限制  每秒速度mm


//////速度环的PID
//float P[4]={2.5,2.5,2.5,2.5}; 
//float I[4]={1.2,0.8,0.8,0.8};  
//float D[4]={  1,  1,  1,0.8};
//u16	    PIDmax=4000;

//////为2000速度调的参数  横向   //基准 2200
//u16	PIDmax1[4]={4000,4000,4000,4000};//电机PWM前进限幅 决定加速度 上限10000
//int	PIDmax0[4]={-4000,-4000,-4000,-4000};//电机PWM后退限幅 决定后退加速度 上限10000   电机2是真慢

////矫正环的PID
//float s_P[4]={5  ,5  ,5  ,5  }; 
//float s_I[4]={0.1,0.1,0.1,0.1};  
//float s_D[4]={  4,  4,  4,  4};
//u16	search_max=2000;//电机PWM限幅

////横向 前 ↑ ↓  后 ↓ ↑
////		  ↓ ↑ 	  ↑ ↓

void PID_init()
{
	//pidmode = stop;
//	//速度环初始化
//	PID_struct_init(&pid_data_spd[0],Position_Pid,PIDmax,2000,10000,P[0],I[0],D[0]);  //初始化 pid参数 为增量式输出  最大输出值 积分项限 积分分离转速值}
//	PID_struct_init(&pid_data_spd[1],Position_Pid,PIDmax,2000,10000,P[1],I[1],D[1]);
//	PID_struct_init(&pid_data_spd[2],Position_Pid,PIDmax,2000,10000,P[2],I[2],D[2]);
//	PID_struct_init(&pid_data_spd[3],Position_Pid,PIDmax,2000,10000,P[3],I[3],D[3]);
//	
//	//位置环初始化  每个轮子 
//	for(int i=0;i<4;i++) 
//	{
//		PID_struct_init(&pid_data_pla[i],Position_Pid,p_PIDmax,0,0,p_P,p_I,p_D);  //结果/10？
//		pid_pla[i].target=0;
//	}
		
//	//角度环初始化
//	PID_struct_init(&pid_data_ang[0],Position_Pid,a1_PIDmax,0,0,a1_P,0,a1_D); 
	//旋转环初始化
	PID_struct_init(&pid_data_ang[1],Position_Pid,a1_PIDmax,0,0,a1_P,0,a1_D); 
//	
//	//搜索环初始化
//	PID_struct_init(&pid_data_sch[0],Position_Pid,search_max,1500,5000,s_P[0],s_I[0],s_D[0]);  //初始化 pid参数 为增量式输出  最大输出值 积分项限 积分分离转速值}
//	PID_struct_init(&pid_data_sch[1],Position_Pid,search_max,1500,5000,s_P[1],s_I[1],s_D[1]);
//	PID_struct_init(&pid_data_sch[2],Position_Pid,search_max,1500,5000,s_P[2],s_I[2],s_D[2]);
//	PID_struct_init(&pid_data_sch[3],Position_Pid,search_max,1500,5000,s_P[3],s_I[3],s_D[3]);
}


////==================================PID计算===================== 每10ms算一次
//int slow_start;//缓慢加速的标识符
//int pid_outmax=0;//速度环当前最大输出


void pid_cal(u8 mode)
{
	u8 num; // 轮子的序号
	//Get_Ang(); // 获取陀螺仪角度
	float car_ang;
	car_ang=fAngle[2];
	
//	Get_Encoder(); // 获取编码器
//	
//	// 计算速度
//	for (num = 0; num < 4; num++)
//	{
//		motor_speed[num] = (lunzi * Encoder[num] / 1024.0 * 30 / 70) / 0.010; // 车轮速度换算（调
//	// 每秒速度mm = 轮子周长 *编码器数/编码器一圈 	//还有轮齿比		小轮30 大轮齿数70
//		
//		motor_mm[num] += motor_speed[num]* 0.010; //电机已行驶的路程
//	}
	
//	// 车辆停止    0
//	if (pidmode == stop)
//	{
//		for (num = 0; num < 4; num++)
//		{
//			pid_spd[num].target = 0; // 电机目标

//			pid_spd[num].pid_get = motor_speed[num];
//			pid_spd[num].pid_out = pid_calc(&pid_data_spd[num], pid_spd[num].pid_get, pid_spd[num].target); // 输出电机
//		}
//		Motor_Set(pid_spd[0].pid_out,pid_spd[1].pid_out,pid_spd[2].pid_out,pid_spd[3].pid_out);
//	}

//	// 位置环pid  ======  1
//	if (pidmode == place_mode)
//	{
//		//***位置环
//		
//		//**电机缓慢加速功能
//		if(slow_start != 0)
//		{
//			pid_outmax=0;//每次把最大速度重置
//			slow_start=1;
//		}
//		if(pid_outmax < PIDmax)
//			pid_outmax  +=75;//70-80
//		
//		for (num = 0; num < 4; num++) // 电机1-4分别计算
//		{
//			// 输入路程
//			pid_pla[num].pid_get = (int)motor_mm[num];
//			
//			// 输出目标速度
//			pid_pla[num].pid_out = pid_calc(&pid_data_pla[num], pid_pla[num].pid_get, pid_pla[num].target);

//			//**缓慢加速功能
//			if(pid_pla[num].pid_out>pid_outmax) pid_pla[num].pid_out=pid_outmax;
//			
//			if(pid_pla[num].pid_out<-pid_outmax) pid_pla[num].pid_out=-pid_outmax;

// 			// 输入目标速度（位置环+速度环）
//			pid_spd[num].target = pid_pla[num].pid_out;

//		}
//		
//		//***很小的角度环  避免跑偏
//		// 输入角度(获取陀螺仪的值),输出小车旋转速度
//		pid_ang[0].pid_get = car_ang;
//		pid_ang[0].pid_out = pid_calc(&pid_data_ang[0], pid_ang[0].pid_get, pid_ang[0].target);

//		// 转动车轮方向： ↗ ↘ -
//		//				  ↖ ↙ -
//		pid_spd[0].target += pid_ang[0].pid_out;	 // 左前驱动
//		pid_spd[1].target += -pid_ang[0].pid_out; // 右前
//		pid_spd[2].target += pid_ang[0].pid_out;	 // 右后
//		pid_spd[3].target += -pid_ang[0].pid_out; // 左后		
//		
//		//***位置-速度环
//		for (num = 1; num <= 4; num++) // 电机1-4分别计算
//		{
//			
//			// 输入当前速度
//			pid_spd[num].pid_get = motor_speed[num];
//			
//			// 计算速度的PID
//			pid_spd[num].pid_out = pid_calc(&pid_data_spd[num], pid_spd[num].pid_get, pid_spd[num].target);
//		
//		}
//		//马达启动
//		Motor_Set(pid_spd[0].pid_out,pid_spd[1].pid_out,pid_spd[2].pid_out,pid_spd[3].pid_out);
//	}
		
	// 角度环的pid  ======  2
	if (1)//pidmode == angle_mode)
	{
		// 获取陀螺仪角度, 开始计算
		pid_ang[1].pid_get = min_angle(car_ang,pid_ang[1].target);//最小旋转角
		pid_ang[1].pid_out = pid_calc(&pid_data_ang[1], pid_ang[1].pid_get, pid_ang[1].target);

		//套上+速度环
//		// 转动车轮方向：↗ ↘ -
//		//				 ↖ ↙ -
//		pid_spd[0].target = pid_ang[1].pid_out;  // 左前驱动
//		pid_spd[1].target = -pid_ang[1].pid_out; // 右前
//		pid_spd[2].target = pid_ang[1].pid_out;  // 右后
//		pid_spd[3].target = -pid_ang[1].pid_out; // 左后
//		for (num = 0; num < 4; num++)						   // 电机1-4分别计算速度
//		{

//			// 输入当前速度
//			pid_spd[num].pid_get = motor_speed[num];

//			// 计算速度的PID
//			pid_spd[num].pid_out = pid_calc(&pid_data_spd[num], pid_spd[num].pid_get, pid_spd[num].target);
//		}
//		//马达启动
//		Motor_Set(pid_spd[0].pid_out,pid_spd[1].pid_out,pid_spd[2].pid_out,pid_spd[3].pid_out);

	if(ABS(pid_ang[1].pid_out)>200)
		Motor_Set(-pid_ang[1].pid_out,pid_ang[1].pid_out,-pid_ang[1].pid_out,pid_ang[1].pid_out);
	}
	

//	// 纯速度环，测试用  ====== 3
//	if (pidmode == speed_mode)
//	{
//		for (num = 0; num <= 3; num++) // 电机1-4分别计算速度
//		{
//			// 输入当前速度
//			pid_spd[num].pid_get = motor_speed[num];

//			// 计算速度的PID
//			pid_spd[num].pid_out = pid_calc(&pid_data_spd[num], pid_spd[num].pid_get, pid_spd[num].target);
//			
//			//PWM最大限幅
//			//电机PWM前进限幅 决定加速度
//			if(pid_spd[num].pid_out > PIDmax1[num]) pid_spd[num].pid_out = PIDmax1[num-1];
//			//电机PWM后退限幅 决定后退加速度			
//			if(pid_spd[num].pid_out < PIDmax0[num]) pid_spd[num].pid_out = PIDmax0[num-1];	
//		}
//		Motor_Set(pid_spd[0].pid_out,pid_spd[1].pid_out,pid_spd[2].pid_out,pid_spd[3].pid_out);

//	}
//	
//	// 横向移动
//	if (pidmode == xmove_mode)
//	{
//		// 移动方向：   -→
//		// 转动车轮方向： ↗ ↖ -
//		//				  ↖ ↗ -
//		//***位置环
//		
//		//**电机缓慢加速功能
//		if(slow_start != 0)
//		{
//			pid_outmax=0;//每次把最大速度重置
//			slow_start=1;
//		}
//		if(pid_outmax < PIDmax)
//			pid_outmax  +=20;	//速度每5ms增长20

//		for (num = 0; num < 4; num++) // 电机1-4分别计算
//		{
//			// 输入路程
//			pid_pla[num].pid_get = motor_mm[num];
//			
//			// 输出目标速度
//			pid_pla[num].pid_out = pid_calc(&pid_data_pla[num], pid_pla[num].pid_get, pid_pla[num].target);
//			
//			//**起步缓慢加速
//			if(pid_pla[num].pid_out>pid_outmax) pid_pla[num].pid_out=pid_outmax;
//			
//			if(pid_pla[num].pid_out<-pid_outmax) pid_pla[num].pid_out=-pid_outmax;
//			
//			pid_spd[num].target = pid_pla[num].pid_out;
//			
//		}
//		
//		//***很小的角度环  避免跑偏
//		// 输入角度(获取陀螺仪的值),输出小车旋转速度
//		pid_ang[0].pid_get = car_ang;
//		pid_ang[0].pid_out = pid_calc(&pid_data_ang[0], pid_ang[0].pid_get, pid_ang[0].target);

//		// 转动车轮方向： ↗ ↘ -
//		//				  ↖ ↙ -
//		pid_spd[0].target += pid_ang[0].pid_out;	 // 左前驱动
//		pid_spd[1].target -= pid_ang[0].pid_out; // 右前
//		pid_spd[2].target += pid_ang[0].pid_out;	 // 右后
//		pid_spd[3].target -= pid_ang[0].pid_out; // 左后		
//		
//		//***位置-速度环
//		for (num = 0; num < 4; num++) // 电机1-4分别计算
//		{
//			// 输入当前速度
//			pid_spd[num].pid_get = motor_speed[num];
//			// 计算速度的PID
//			pid_spd[num].pid_out = pid_calc(&pid_data_spd[num], pid_spd[num].pid_get, pid_spd[num].target);
//			
//			//电机PWM前进限幅 决定加速度
//			if(pid_spd[num].pid_out > PIDmax1[num]) pid_spd[num].pid_out = PIDmax1[num];
//			//电机PWM后退限幅 决定后退加速度			
//			if(pid_spd[num].pid_out < PIDmax0[num]) pid_spd[num].pid_out = PIDmax0[num];	
//		
//		}
//		//马达启动
//		Motor_Set(pid_spd[0].pid_out,pid_spd[1].pid_out,pid_spd[2].pid_out,pid_spd[3].pid_out);
//		
//	}
//	
//	
//	//搜索环
//	//用于art对目标的定位
//	if (pidmode == search_mode)
//	{
//		// 移动方向：   -→
//		// 转动车轮方向： ↗ ↖ -
//		//				  ↖ ↗ -
//		//***位置环
//		for (num = 0; num < 4; num++) // 电机1-4分别计算
//		{
//			// 输入距离
//			pid_pla[num].pid_get = motor_mm[num];
//			
//			// 输出目标速度
//			pid_pla[num].pid_out = pid_calc(&pid_data_pla[num], pid_pla[num].pid_get, pid_pla[num].target);
//			
//			// 输入目标速度（位置环+速度环）
//			pid_sch[num].target = pid_pla[num].pid_out;
//		}
//		
//		//***很小的角度环  避免跑偏
//		// 输入角度(获取陀螺仪的值),输出小车旋转速度
//		pid_ang[0].pid_get = car_ang;
//		pid_ang[0].pid_out = pid_calc(&pid_data_ang[0], pid_ang[0].pid_get, pid_ang[0].target);

//		// 转动车轮方向： ↗ ↘ -
//		//				  ↖ ↙ -
//		pid_sch[0].target += pid_ang[0].pid_out;	 // 左前驱动
//		pid_sch[1].target -= pid_ang[0].pid_out; // 右前
//		pid_sch[2].target += pid_ang[0].pid_out;	 // 右后
//		pid_sch[3].target -= pid_ang[0].pid_out; // 左后		
//		
//		//***位置-速度环
//		for (num = 0; num < 4; num++) // 电机1-4分别计算
//		{
//			// 输入当前速度
//			pid_sch[num].pid_get = motor_speed[num];
//			// 计算速度的PID
//			pid_sch[num].pid_out = pid_calc(&pid_data_sch[num], pid_sch[num].pid_get, pid_sch[num].target);
//		}
//		//马达启动
//		Motor_Set(pid_sch[0].pid_out,pid_sch[1].pid_out,pid_sch[2].pid_out,pid_sch[3].pid_out);
//		
//	}
	
	//测试，发送监测数据
//	printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
//	(int)motor_speed[0],(int)motor_speed[1],(int)motor_speed[2],(int)motor_speed[3],(int)(car_ang*10),
//	(int)motor_mm[0],(int)motor_mm[1],(int)motor_mm[2],(int)motor_mm[3],Tar_x,Tar_y);
	//(int)pid_spd[1].pid_out,(int)pid_spd[2].pid_out,(int)pid_spd[3].pid_out,(int)pid_spd[4].pid_out,Tar_x,Tar_y);
	//printf("%d,%d,%d,%d,%d\n",(int)motor_mm[1],(int)motor_mm[2],(int)motor_mm[3],(int)motor_mm[4],(int)(car_ang*10));

}


