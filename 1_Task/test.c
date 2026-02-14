
//�ܲ����ļ�
#include "test.h"
#include "car-data.h"
#include "ps2.h"



void speed_test(int speed)	//�ٶȲ��� (�ٶȵ�λ��mm/s)
{
	delayms(1000);
	pidmode = speed_mode;
	
	pid_spd[1].target = speed;	 // ��ǰ����
	pid_spd[2].target = speed; // ��ǰ
	pid_spd[3].target = speed;	 // �Һ�
	pid_spd[4].target = speed; // ���		
	
//	pidmode = speed_mode;
}

void run_back_test() //�����ܾ������
{
	run_start:
	
	for (u8 i = 1; i <= 4; i++) 
	{	
		motor_mm[i] = 0;				// �߹��ľ������
		pid_pla[i].target = 0;			// �߹��ľ������
	}
	xymove(500,0);
	delayms(200);
	while(get_speed()>10);
	
	for (u8 i = 1; i <= 4; i++) 
	{	
		motor_mm[i] = 0;				// �߹��ľ������
		pid_pla[i].target = 0;			// �߹��ľ������
	}
	xymove(-500,0);
	delayms(200);
	while(get_speed()>10);
	
	goto run_start;
	

}

//�������ٶȲ���
void speed_back_test()
{
	start1:
		Motor_Set(1700,1900,1800,2000);//�ٶȲ���
		delayms(800);
		Motor_Set(0,0,0,0);//�ٶȲ���
		delayms(1000);
		Motor_Set(-1850,-2000,-1500,-1600);//�ٶȲ���
		delayms(800);
		Motor_Set(0,0,0,0);//�ٶȲ���
		delayms(1000);
	goto start1;
}



void xymove_test() 	//�����������
{
	for(int i=0;i<1;i++)
	{
		int flag1=1;
		
		move_start:  //���ű���
		xymove(4000,0); //������PID����
		delayms(500);
		while(get_speed()>400) //��ʻ�����У��ȴ���ͣ��
		{
			//����ͼƬ
			uart_write_string(UART_4, "A"); //�л�����ģʽ
			Target_get();//��ȡ����ͷ����
		
			if(abs(Tar_x)<60 && abs(Tar_y)<60)
			//if(Tar_x!=-170)
			{
				//��¼��ͣ�����ĵ�
				float static_mm[5];
				for (u8 i = 1; i <= 4; i++) static_mm[i] = motor_mm[i];//�ݴ�ͣ�����ĵ�
				pid_pla[1].target = static_mm[1]; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[2].target = static_mm[2]; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[3].target = static_mm[3];; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[4].target = static_mm[4];; // ����ÿ�����ӵ�Ŀ�����
				delayms(300); //�ȴ���ȥ
					
				while(1)//================����ͼƬ��==================
				{
					Target_get();
					if(Tar_x!=-170) search_move(1.8*Tar_x,1.8*Tar_y);
				
					//��μ�� �ȴ�ͣ��
					static u8 stop_i;
					if(get_speed()==0)
					{
						stop_i++;
					}
					else stop_i=0;
					if(stop_i==5) break;  //������
					
					delayms(10);
				}
				
				//ʶ����ϣ�������
				//�ص�ͣ�µ�
				pid_pla[1].target = 0; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[2].target = 0; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[3].target = 0; // ����ÿ�����ӵ�Ŀ�����
				pid_pla[4].target = 0; // ����ÿ�����ӵ�Ŀ�����
				//����ǰ��
				for (u8 i = 1; i <= 4; i++) motor_mm[i] = static_mm[i] ;//ȡ��ͣ�����ĵ�
				
				goto move_start; //�ص�������·��
			}
			
			delayms(10);
			//д����ͷ�жϺ���
		}
		
	}
	xymove(-4000,0);
}

//PS2手柄测试例程
void ps2_test()
{
	// 初始化PS2手柄
	PS2_Init();
	
	// 配置PS2手柄为模拟模式并开启震动
	PS2_SetInit();
	
	// 检查是否为红灯模式
	if (PS2_RedLight() == 0)
	{
		// 红灯模式（模拟模式）
		uart_write_string(UART_4, "PS2: Red Light Mode\r\n");
	}
	else
	{
		// 绿灯模式（数字模式）
		uart_write_string(UART_4, "PS2: Green Light Mode\r\n");
	}
	
	while(1)
	{
		// 读取PS2手柄数据
		PS2_ReadData();
		
		// 获取按键状态
		u8 key = PS2_DataKey();
		if (key != 0)
		{
			// 按键按下，发送按键信息
			uart_write_string(UART_4, "PS2 Key: ");
			// 这里可以根据按键值发送具体的按键名称
			uart_write_string(UART_4, "\r\n");
			
			// 示例：根据不同按键执行不同操作
			switch(key)
			{
				case PSB_PAD_UP:
					// 向上键
					uart_write_string(UART_4, "UP\r\n");
					break;
				case PSB_PAD_DOWN:
					// 向下键
					uart_write_string(UART_4, "DOWN\r\n");
					break;
				case PSB_PAD_LEFT:
					// 向左键
					uart_write_string(UART_4, "LEFT\r\n");
					break;
				case PSB_PAD_RIGHT:
					// 向右键
					uart_write_string(UART_4, "RIGHT\r\n");
					break;
				case PSB_START:
					// 开始键
					uart_write_string(UART_4, "START\r\n");
					break;
				case PSB_SELECT:
					// 选择键
					uart_write_string(UART_4, "SELECT\r\n");
					break;
				default:
					break;
			}
		}
		
		// 获取摇杆数据
		u8 rx = PS2_AnologData(PSS_RX); // 右摇杆X轴
		u8 ry = PS2_AnologData(PSS_RY); // 右摇杆Y轴
		u8 lx = PS2_AnologData(PSS_LX); // 左摇杆X轴
		u8 ly = PS2_AnologData(PSS_LY); // 左摇杆Y轴
		
		// 每100ms发送一次摇杆数据
		static u32 last_time = 0;
		if (HAL_GetTick() - last_time > 100)
		{
			last_time = HAL_GetTick();
			
			// 发送摇杆数据
			uart_write_string(UART_4, "PS2 Analog: ");
			// 这里可以发送具体的摇杆数值
			uart_write_string(UART_4, "\r\n");
		}
		
		// 短延时
		delayms(10);
	}
}

