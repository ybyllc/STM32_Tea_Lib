#include "car-data.h"


//按键引脚
#define KEY1 C12     //中      KEY1 C12     //中
#define KEY2 C8      //上 U    KEY2 C9      //上 U
#define KEY3 C7      //下 D    KEY3 C10     //下 D
#define KEY4 C9      //左 L    KEY4 C7      //左 L
#define KEY5 C10     //右 R    KEY5 C8      //右 R
#define KEY6 C5      //独立    KEY6 C5      //独立 

uint8_t KEY1_Read();
uint8_t KEY2_Read();
uint8_t KEY3_Read();
uint8_t KEY4_Read();
uint8_t KEY5_Read();
uint8_t KEY6_Read();

void KEY_Pin_Init(void);	//按键初始化函数
