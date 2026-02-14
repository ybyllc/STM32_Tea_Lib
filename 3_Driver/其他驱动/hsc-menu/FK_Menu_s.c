#include "FK_Menu.h"

u8 keynum;
u8 readytorun;
u8 pagepoint=0;
uint16 x,y;

void canshupage_1(void)
{
	float *canshuzhizheng[8];
	canshuzhizheng[0]=&Turn_PID_Line[0];
	canshuzhizheng[1]=&Turn_PID_Line[2];
	canshuzhizheng[2]=&Turn_PID_Z[0];
	canshuzhizheng[3]=&Angle_gyro_PID[0];
	canshuzhizheng[4]=&Angle_gyro_PID[2];
	canshuzhizheng[5]=&Speed_PID[0];
	canshuzhizheng[6]=&turnbias;
	canshuzhizheng[7]=&camera_correct;
	pagepoint=0;
	ips114_showint16(190,0,Turn_PID_Line[0]);
	ips114_showint16(190,1,Turn_PID_Line[2]);
	ips114_showfloat(190,2,Turn_PID_Z[0],2,3);	
	ips114_showint16(190,3,Angle_gyro_PID[0]);
	ips114_showint16(190,4,Angle_gyro_PID[2]);
	ips114_showfloat(190,5,Speed_PID[0],2,3);	
	ips114_showfloat(190,6,turnbias,2,3);	
	ips114_showfloat(190,7,camera_correct,2,3);
	ips114_showstr(60,0,"Turn_PID_Line_P ");
	ips114_showstr(60,1,"Turn_PID_Line_D ");
	ips114_showstr(60,2,"Turn_PID_Z_P    ");
	ips114_showstr(60,3,"Angle_Gyro_PID_P");
	ips114_showstr(60,4,"Angle_Gyro_PID_D");
	ips114_showstr(60,5,"Speed_PID_P     ");
	ips114_showstr(60,6,"Turnbias        ");
	ips114_showstr(60,7,"Camera_Correct  ");
	ips114_showstr(40,pagepoint,(int8*)"->");
	do
	{
		keynum=KEY_Scan(1);
		switch(keynum)
		{
			case keyup:
				if(pagepoint>0) 
				{
					pagepoint--;
					ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
					ips114_showstr(40,pagepoint,(int8*)"->");
				}
				break;
			case keydown:
				if(pagepoint<7)
				{
					pagepoint++;
					ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
					ips114_showstr(40,pagepoint,(int8*)"->");
				}
				break;
			case keyright: 
				if(pagepoint==2||pagepoint==5||pagepoint==7||pagepoint==6)
				{
					if(pagepoint==5)
					{
						*canshuzhizheng[pagepoint]+=0.01;
						ips114_showfloat(190,pagepoint,*canshuzhizheng[pagepoint],3,2);								
					}else
					{
						*canshuzhizheng[pagepoint]+=0.1;
						ips114_showfloat(190,pagepoint,*canshuzhizheng[pagepoint],3,2);						
					}

				}else
				{
					*canshuzhizheng[pagepoint]+=1;
					ips114_showint16(190,pagepoint,*canshuzhizheng[pagepoint]);
				}
				break;
			case keyleft:
				if(pagepoint==2||pagepoint==5||pagepoint==7||pagepoint==6)
				{
					if(pagepoint==5)
					{
						*canshuzhizheng[pagepoint]-=0.01;
						ips114_showfloat(190,pagepoint,*canshuzhizheng[pagepoint],3,2);								
					}else
					{
						*canshuzhizheng[pagepoint]-=0.1;
						ips114_showfloat(190,pagepoint,*canshuzhizheng[pagepoint],3,2);						
					}
				}else
				{
					*canshuzhizheng[pagepoint]-=1;
					ips114_showint16(190,pagepoint,*canshuzhizheng[pagepoint]);
				}
				break;
				default:break;
		}
		
	}while(keynum!=keypress);
	ips114_clear(IPS114_BGCOLOR);
	
	
}

void canshupage_2(void)
{
	int16 *canshuzhizheng[8];
	canshuzhizheng[0]=&encoder1_speed;
	canshuzhizheng[1]=&encoder2_speed;
	canshuzhizheng[2]=(int16 *)&carangy;;
	canshuzhizheng[3]=&maxspeed;
	canshuzhizheng[4]=&minspeed;
	canshuzhizheng[5]=&turnmax;
	canshuzhizheng[6]=&turnmin;
	canshuzhizheng[7]=NULL;
	ips114_showint16(190,3,maxspeed);
	ips114_showint16(190,4,minspeed);	
	ips114_showint16(190,5,turnmax);
	ips114_showint16(190,6,turnmin);		
	ips114_showstr(60,0,"Encoder1_speed  ");
	ips114_showstr(60,1,"Encoder1_speed  ");
	ips114_showstr(60,2,"Carangy         ");
	ips114_showstr(60,3,"Maxspeed        ");
	ips114_showstr(60,4,"Minspeed        ");
	ips114_showstr(60,5,"Turnmax         ");
	ips114_showstr(60,6,"Turnmin         ");
	ips114_showstr(40,pagepoint,(int8*)"->");
	do
	{
		keynum=KEY_Scan(1);
		ips114_showint16(190,0,encoder1_speed);
		ips114_showint16(190,1,encoder2_speed);
		ips114_showint16(190,2,carangy*100);

		switch(keynum)
		{
			case keyup:
				if(pagepoint>0) 
				{
					pagepoint--;
					ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
					ips114_showstr(40,pagepoint,(int8*)"->");
				}
				break;
			case keydown:
				if(pagepoint<6)
				{
					pagepoint++;
					ips114_drawblock(40,0,55,240,IPS114_BGCOLOR);
					ips114_showstr(40,pagepoint,(int8*)"->");
				}
				break;
			case keyright: 

					*canshuzhizheng[pagepoint]+=1;
					ips114_showint16(190,pagepoint,*canshuzhizheng[pagepoint]);
				break;
			case keyleft:

					*canshuzhizheng[pagepoint]-=1;
					ips114_showint16(190,pagepoint,*canshuzhizheng[pagepoint]);
				break;
				default:break;
		}
		
		
		
	}while(keynum!=keypress);
	ips114_clear(IPS114_BGCOLOR);
}

void imagepage(void)
{
	uint8 *canshuzhizheng[8];
	canshuzhizheng[0]=&imthreshold[0];
	canshuzhizheng[1]=&imthreshold[1];
	canshuzhizheng[2]=&imthreshold[2];
	canshuzhizheng[3]=NULL;
	canshuzhizheng[4]=NULL;
	canshuzhizheng[5]=NULL;
	canshuzhizheng[6]=NULL;
	canshuzhizheng[7]=NULL;
	pagepoint=0;
	ips114_showint16(190,0,imthreshold[0]);
	ips114_showint16(190,1,imthreshold[1]);
	ips114_showint16(190,2,imthreshold[2]);
	for(int i=0;i<MT9V03X_H;i++)
	{
		ips114_drawpoint(MT9V03X_W,i,WHITE);
	}
	for(int i=0;i<MT9V03X_W;i++)
	{
		ips114_drawpoint(i,MT9V03X_H,WHITE);
	}	
	do
	{
		keynum=KEY_Scan(1);
		switch(keynum)
		{
			case keyup:

				break;
			case keydown:

				break;
			case keyright: 

				if(imthreshold[0]<255&&imthreshold[1]<255&&imthreshold[2]<255)
				{
					*canshuzhizheng[0]+=1;
					*canshuzhizheng[1]+=1;
					*canshuzhizheng[2]+=1;
					ips114_showint16(190,0,*canshuzhizheng[0]);
					ips114_showint16(190,1,*canshuzhizheng[1]);
					ips114_showint16(190,2,*canshuzhizheng[2]);
				}
				break;
			case keyleft:
				if(imthreshold[0]>0&&imthreshold[1]>0&&imthreshold[2]>0)
				{
					*canshuzhizheng[0]-=1;
					*canshuzhizheng[1]-=1;
					*canshuzhizheng[2]-=1;
					ips114_showint16(190,0,*canshuzhizheng[0]);
					ips114_showint16(190,1,*canshuzhizheng[1]);
					ips114_showint16(190,2,*canshuzhizheng[2]);
				}
				break;
				default:break;
		}
	if(mt9v03x_finish_flag)	//图像处理标志位
	{			
		image_process();
		y=xinbiao_row;
		x=xinbiao_col;
		ips114_displayimage032(bitmap[0], MT9V03X_W, MT9V03X_H);
		for(int i=0;i<MT9V03X_H;i++)
		{
			ips114_drawpoint(x,i,RED);
		}
		for(int i=0;i<MT9V03X_W;i++)
		{
			ips114_drawpoint(i,y,RED);
		}		
		mt9v03x_finish_flag = 0;
	}			

	
	}while(keynum!=keypress);
	ips114_clear(IPS114_BGCOLOR);
	
}

void readytorunpage(void)
{
		for(int i=0;i<MT9V03X_H;i++)
		{
			ips114_drawpoint(MT9V03X_W,i,WHITE);
		}
		for(int i=0;i<MT9V03X_W;i++)
		{
			ips114_drawpoint(i,MT9V03X_H,WHITE);
		}	
		ips114_showstr(0,150,"Press key to run  ");
	do
	{
		keynum=KEY_Scan(1);
		readytorun=0;
	if(mt9v03x_finish_flag)	//图像处理标志位
	{		
		image_process();
		y=xinbiao_row;
		x=xinbiao_col;
		ips114_displayimage032(bitmap[0], MT9V03X_W, MT9V03X_H);

		for(int i=0;i<MT9V03X_H;i++)
		{
			ips114_drawpoint(x,i,RED);
		}
		for(int i=0;i<MT9V03X_W;i++)
		{
			ips114_drawpoint(i,y,RED);
		}		
		mt9v03x_finish_flag = 0;
	}		

	}while(keynum!=KEY1_PRES);
	readytorun=1;
	ips114_clear(IPS114_BGCOLOR);
}

void Menu(void)
{
	canshupage_1();
	canshupage_2();
	imagepage();
	readytorunpage();
}
















































