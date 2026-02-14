// 控制小车移动的函数
// 写小车的各项控制
#include "control.h"


#include "car-data.h"

//==================================跑一个点==============================
void go_point(short to_x, short to_y, short ang) // 去目标点，最后朝的角度（初始是0度）
{
	// 逻辑：
	// 1.先转目标角度to_ang
	// 2.再直行距离
	// 3.然后旋转回某个方向

	//======================1.自身旋转=========================
	//***原点旋转，到目标角度
	//逆时针，且原点是0°
	float to_ang;									 // 目标角度
	to_ang = xy_angle(car_x, car_y, to_x, to_y); // 计算目标角度
	to_ang = min_angle(to_ang, car_ang);		//寻找最近角
	
	pid_ang[1].target = to_ang; // 设置角度环目标
	pidmode = angle_mode;			// 开始pid旋转

	while (abs(to_ang - car_ang) > 1 || car_speed != 0)
		; // 等待到达目标角度,陀螺仪角度;
	//pidmode = stop;
	
	//======================2.前进距离=========================
	//***前进
	u16 to_dis; // 算出来的目标距离
	to_dis = distance(car_x, car_y, to_x, to_y);
	

	slow_start=1; //开启缓慢加速

	for (u8 i = 0; i < 4; i++)
	{
		motor_mm[i] = 0;				// 走过的距离归零
		pid_pla[i].target = to_dis; // 设置每个轮子的目标距离
	}
	pid_ang[0].target = to_ang; // 角度环
	pidmode = place_mode;
	
	delayms(200);
	
//	while ((motor_mm[0]<to_dis*0.9))
//		; // 等待到达目标位置
	while (car_speed > 80)
		; // 等待车辆停下
	

	//======================3.转回角度=========================
	//***到点后转回原来的角度（目标角度）

	ang = min_angle(ang, car_ang);		//寻找最近角
	pid_ang[1].target = ang; // 设置角度环目标,单位 °
	pidmode = angle_mode;

	while (abs(ang - car_ang) > 1 || car_speed != 0) // 等待到达目标角度,陀螺仪角度;
		; 

	//设定车的位置
	car_x = to_x; 
	car_y = to_y;
}

// 横向平移移动 （车头始终超前）
void xymove(float x_mm,float y_mm) 
{
	pid_ang[0].target=0; //矫正环角度 车头朝前

	slow_start=1; //开启缓慢加速

	pid_pla[0].target += x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[1].target +=-x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[2].target +=-x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[3].target += x_mm+y_mm; // 设置每个轮子的目标距离
	
	pidmode = xmove_mode;

}

// 搜索图片
void search_pic() 
{
	while(1)	
	{
		Target_get(); //获取图片位置
//		if(Tar_x!=999) search_move(1.8*Tar_x,1.8*Tar_y);
//	
//		//多次检测 等待停下
//		static u8 stop_i;
//		if(get_speed()<100)
//		{
//			if(abs(Tar_x)<5 && abs(Tar_y)<5) //搜寻到
//				stop_i++;
//		}
//		else stop_i=0;
//		if(stop_i==10) break;  //搜索到
//		
//		delayms(10);
	}
}

// 搜索移动
void search_move(float x_mm,float y_mm) 
{
	pid_ang[0].target=car_ang; //角度环角度 车头朝前
	
	mm_clear();//清除上一次距离信息

	pid_pla[0].target += x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[1].target +=-x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[2].target +=-x_mm+y_mm; // 设置每个轮子的目标距离
	pid_pla[3].target += x_mm+y_mm; // 设置每个轮子的目标距离
	
	pidmode = search_mode;

}


float get_speed()//获取车的平均车速
{
	return (abs(motor_speed[1])+abs(motor_speed[2])+abs(motor_speed[3])+abs(motor_speed[4]))/4;
}

void mm_clear()	 //重置马达和pid的距离信息
{
	for (u8 i = 0; i < 4; i++) 
	{	
		motor_mm[i] = 0;				// 走过的距离归零
		pid_pla[i].target = 0;			// 走过的距离归零
	}
}

