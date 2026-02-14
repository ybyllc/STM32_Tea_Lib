//#include "openmv_2.h"

//u8 Rx_pt[20][2];
// 
//uint8 uart_get_data[64];                                                        // 串口接收数据缓冲区
//uint8 fifo_get_data[64];                                                        // fifo 输出读出缓冲区

//uint8 get_data = 0;                                                             // 接收数据变量
//uint32 fifo_data_count = 0;                                                     // fifo 数据个数

//uint8 gpio_status;

//fifo_struct uart_data_fifo;

//uint8 point[9];

//int t = 0;
//char *Map_dian;
//int d=1;
//int num = 0;
//int c = 0;
//int x = 0;
//int y = 0;	 
//		
//u8 map_flag=0;

//void Art_rx_init()
//{
//	fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // 初始化 fifo 挂载缓冲区
//    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // 初始化编码器模块与引脚 正交解码编码器模式
//	uart_rx_interrupt(UART_INDEX, ZF_ENABLE);                                   // 开启 UART_INDEX 的接收中断
//    interrupt_set_priority(UART_PRIORITY, 3);                                   // 设置对应 UART_INDEX 的中断优先级为 0	

//}

//void Get_point()
//{

//	//	Rx_pt[0][0] = 0;
//				fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
//        if(fifo_data_count != 0)                                                // 读取到数据了
//        {
//           fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
//					Map_dian = (char*)fifo_get_data;
//					while(Map_dian[d]!='}'){
//						if(Map_dian[d]==','){	
//							Rx_pt[x][y] = num;
//							y = 1-y;
//							num=0;
//						}
//						else 
//							num = num*10+(int)(Map_dian[d]-48);
//						d++;			
//					}
//					Rx_pt[x][y] = num;
//					y = 1-y;
//					num=0;
//					d=1;
//					x++;
//					while(Rx_pt[c/2][0]==0){
////  					tft_dot(Rx_pt[c/2][0],Rx_pt[c/2][1]);
//// 						tft_word(Rx_pt[c/2][0],Rx_pt[c/2][1],c,t);					
//						c+=2;
//						if(c%16==0)
//								t+=1;
//					}
//        }
//				system_delay_ms(5);
//		
//			for(int i = 0;i<20;i++){   //判断收到的点数
//				if(Rx_pt[i][0] == 99){
////					Set_pt = i;
//					printf("A");         //art1切换模式
////					Run_flag = 1;        //跑点识别标志位被置1
//					map_flag = 1;        //下次主循环不进入地图识别
//					ips114_clear();
//					

//					break;
//				}
//					
//			}
//		
//		
////		  if(c>=COUNT_PT*2)      //判断收到的点数
////			{
////			printf("A");         //art1切换模式
////			Run_flag = 1;
////			map_flag = 1;
////			ips114_show_uint(0,110,c,2);
////			ips114_show_string(20,110,"Map_get!");		
////			}
////			else
////			{
////			ips114_show_uint(0,110,c,2);
////			}
//		
//				
//}

//void uart_rx_interrupt_handler (void)
//{ 		
//    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
//	
//    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中

//	wireless_uart_send_byte(get_data);

//}