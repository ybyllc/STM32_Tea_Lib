////// 接收摄12w222222 `	123w像头1的数据并处理
////// 摄像头1用来识别地图和图像搜索
////// 摄像头2用来


//#include "Get_map.h"
//// 采用了串口+fifo暂存技术，实现快速传输
//// 作用：生成一个64大小的缓冲区，让单片机一次接收64个字符后再读


#include "openmv_1.h"

uint8 uart_get_data[64]; // 串口接收数据缓冲区
uint8 fifo_get_data[64]; // fifo 输出读出缓冲区

uint8 get_data = 0;			// 接收数据变量
uint32 fifo_data_count = 0; // fifo 数据个数

uint8 gpio_status;

fifo_struct uart_data_fifo; //fifo的连接体

int t = 0;
char *dian; //fifo暂存区
int d = 1;
int num = 0;
int c = 0;
int x = 0;
int y = 0;

void openart_1_init() //初始化摄像头的接收函数
{
		fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // 初始化 fifo 挂载缓冲区
		uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // 初始化编码器模块与引脚 正交解码编码器模式
		uart_rx_interrupt(UART_INDEX, ZF_ENABLE);                                   // 开启 UART_INDEX 的接收中断
		interrupt_set_priority(UART_PRIORITY, 3);                                   // 设置对应 UART_INDEX 的中断优先级为 0	
//		tft_frame();        //画地图框
}

//========================搜索图像====================
char *Tar_dian;
int T_d=0;
uint8 T_num = 0;

char *Tar_dian1;
int T_d1=0;	
uint8 T_num1 = 0;

int Tar_x=200;//160; //图像中心点
int Tar_y=200;//120;

void Target_get()   //搜索图像，返回距中心点的距离
{
	//x,y轴偏差值
    int T_x = 999;//每次更新
    int T_y = 999;

	int pos_sign=1;//正负符号
	
	uart_write_string(UART_INDEX, "A"); //切换搜索模式
    fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
	
    if(fifo_data_count != 0)                                                // 读取到数据了
    {
        fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
        Tar_dian = (char*)fifo_get_data;
        while(Tar_dian[T_d]!='{')
		{
            T_d++;
        }
        T_d++;
        while (Tar_dian[T_d] != '}') 
		{
			if (Tar_dian[T_d] == ',') 
			{
				T_x = T_num;        // 是图片处于视野中心坐标（0，0）
				T_num = 0;
			} 
			else
				T_num = T_num * 10 + (int) (Tar_dian[T_d] - '0');
			
			T_d++;
		}
        T_y = T_num;          //如（10，10）就是需要分别向前与向右移动
		
        T_num = 0;
        T_d = 0;
	}
	
        Tar_x = T_x-165;  //和中心点的差

        Tar_y = 120-T_y;
			
		//printf("%d,%d\n",Tar_x,Tar_y);	
}


int clas=0;       //类别

void Classify_get()   //图像识别的分类结果
{

	uart_write_string(UART_INDEX, "A"); //切换搜索模式
    // Rx_pt[0][0] = 0;
    fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
    if(fifo_data_count != 0)                                                // 读取到数据了
    {
        fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
        Tar_dian1 = (char*)fifo_get_data;
		
        while(Tar_dian1[T_d1]!='(') 
		{
            T_d1++;
        }
        T_d1++;
        while (Tar_dian1[T_d1] != ')')
			{
            T_num1 = T_num1 * 10 + (int) (Tar_dian1[T_d1] - 48);
            T_d1++;
			}
			if(T_num1<=15&&T_num1>=1)
        clas = T_num1;        
        T_num1 = 0;
        T_d1 = 0;
    }



}

void uart_rx_interrupt_handler (void)
{ 		
    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
	
    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中

	//wireless_uart_send_byte(get_data);

}



void Map_init()
{
	fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);	// 初始化 fifo 连接串口
	uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN); // 初始化串口引脚
	uart_rx_interrupt(UART_INDEX, ZF_ENABLE);						// 开启 UART_INDEX 的接收中断
	interrupt_set_priority(UART_PRIORITY, 4);						// 设置对应 UART_INDEX 的中断优先级为 4
	//		tft_frame();        //画地图框
}

short rx_pt[21][2];
short rx_pt_num=0; //接收到的点数
//读取地图的数据  成功返回1
u8 Get_point()
{
	uart_write_string(UART_INDEX, "M"); //切换搜索模式
	u8 map_flag;//地图识别成功标志位
	//	rx_pt[0][0] = 0;
	fifo_data_count = fifo_used(&uart_data_fifo); // 查看 fifo 是否有数据
	if (fifo_data_count != 0)					  // 读取到数据了
	{
		fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN); // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
		dian = (char *)fifo_get_data;
		while (dian[d] != '}') //读每个字符，直到停止符
		{
			if (dian[d] == ',')
			{
				rx_pt[x][y] = num;
				y = 1 - y;
				num = 0;
			}
			else
				num = num * 10 + (dian[d] - '0'); //接收数字
			d++;
		}
		rx_pt[x][y] = num;
		y = 1 - y;
		num = 0;
		d = 1;
		x++;
		
		rx_pt_num++;
					while(rx_pt[c/2][0]==0){			
						c+=2;
						if(c%16==0)
								t+=1;
					}
        }
				system_delay_ms(5);
		
			for(int i = 0;i<20;i++){   //判断收到的点数
				if(rx_pt[i][0] == 99){
					printf("A");         //art1切换模式
					map_flag = 1;        //下次主循环不进入地图识别
					ips114_clear();
					
					break;
				}
					
			}
	return map_flag;
}


////串口接收中断（存入fifo）
//void uart_rx_interrupt_handler(void)
//{
//	uart_query_byte(UART_INDEX, &get_data);			  // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
//	fifo_write_buffer(&uart_data_fifo, &get_data, 1); // 将数据写入 fifo 中
//}

//short map_4xy[8];//地图四个顶点的数据
//short D[8];		//存放数字


//// 收到字符格式：

//// 收到一个字符
//// 进入处理函数
//void Get_Char_openmv1(char ch)
//{
//	static char str[30]; // 存到字符串里，上限30
//	static u8 ch_i = 0;	 // 字符的个数			（从1开始）
//	ch_i++;

//	// 一直收到字符串结束符
//	if (ch != '&' && ch != '\r' && ch != '\n' && ch != ')')
//	{
//		str[ch_i] = ch;
//	}
//	else // 接收完，开始处理
//	{
//		//=====================识别地图模式===================

//		// 标识符 地图模式
//		if (str[0] == 'f') 
//		{
//			ri = 0;

//			//**分割字符串
//			// 字符串, 位数ch_i, 分隔符, 存入数组		（函数：遇到分隔号就截断）
//			Split_Buff(str, ch_i, ',', D);

//			// 会有8个坐标	 (x1,y1)(x2,y2)(x3,y3)(x4,y4)
//			for (u8 i = 0; i < 8; i++)
//				map_4xy[i] = D[i]; // //画面太大

//			// 计算地图尺寸

//			map_W = abs(map_4xy[4] - map_4xy[0]); // 顺时针 x3-x1
//			map_H = abs(map_4xy[1] - map_4xy[5]); // y1-y3
//			// 顶点顺序  4  3
//			//		   1  2
//		}

//		// 标识符 地图上的目标点
//		else if (str[0] == 'r') // 输入圆点的开始符
//		{
//			Split_Buff(str, ch_i, ',', D); // 处理字符串函数

//			map_px[map_pi] = D[0]; // 输入一个圆点
//			map_py[map_pi] = D[1];

//			map_pi++; // 圆的个数+1

//			//******后面记得在显示屏上画地图
//		}

//		else if (str[0] == 'p') // 识别图片的标志符
//		{
//			// LCD_ShowString(0,0,str,BLACK,WHITE,24,0);	//显示屏显示接收的数值//不要显示，容易卡死
//			// 处理字符串
//			// 例:{2,138,256}
//			// 位数ch_i,分隔符逗号,,结束符大括号},存入d2数组		（原理：遇到分隔号和结束符截断）
//			Split_Buff(str, ch_i, ',', ')', d2);
//			// 传递数值
//			pic_ture = 1; // 识别成功
//			pic_dalei = d2[0];
//			pic_xiaolei = d2[1];
//			pic_x = d2[2];
//			pic_y = d2[3];
//		}

//		//=====================搜索矩形模式===================

//		// 标识符 搜索矩形模式
//		else if (str[0] == '{')
//		{
//			Split_Buff(str, ch_i, ',', rect_4xy);
//		}
//		else if (str[0] == 'z') // 矩形的中点
//		{
//			Split_Buff(str, ch_i, ',', rect_zxy);
//		}


//		////数据清空
//		ch_i = 0;
//		D[0] = 0;
//		D[1] = 0;
//		D[2] = 0;
//		D[3] = 0;
//	}
//}