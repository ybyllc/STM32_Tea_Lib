//上下键切换
//按下确定
//左键返回
//右键切换功能

//函数结构：
//显示单个字符的函数
//菜单列表
//显示一个页面的菜单
#define u16 unsigned short;
#define u8 unsigned char;
// 1.>护士从

//显示一行字符
void ShowChar(u16 x,u16 y,u8 *s,u16 ccolor,u16 bcolor) //x,y,坐标轴，字符串指针，字体颜色，背景颜色
{
	u16 size=12;//自己选的字体大小，16，24，32
	//******用户修改处
	LCD_ShowChinese(x,y,*s,u16 fc,u16 bc,u8 sizey,1);//来自你的库函数
}

//显示一个页面的菜单
void ShowChar(u16 menulist,u16 num,u16 allnum,u8 len) //菜单指针，当前的行号，每个界面显示的行数,菜单的总项数
{
	u8 i;
	u8 yspace=16;//行间隔
	u16 cc = BLACK;//字体颜色
	u16 bc = BLUE;//选定项的背景颜色
	u16 first_num = (num/allnum)*allnum;//当前菜单的第一项
	for(i=0;i<allnum;i++)
	{
		ShowChar(10,10+i*yspace,first_num+i,ccolor,cc,WHITE);
	}
	ShowChar(10,10+i*yspace,xxx,ccolor,cc,bc);
}

//菜单编号
typedef struct Menu
{
    int mode1;    // 0-代表主菜单，选择进入对应二级菜单    1、2、3...代表二级菜单
    int mode2;    // 0-代表二级菜单，选择进入对应三级菜单  1、2、3...代表三级菜单
    int mode3;    // 0-代表三级菜单，选择进入对应四级菜单  1、2、3...代表四级菜单
    int mode4;    // 同理
}sMenu;

//菜单列表：
//"识别图像"
//“矫正陀螺仪”
//“开始启动”
//“设置舵机角度”
const MENU MenuList[]=
{
	MENU_L_0,//菜单等级
	"小车程序",//文字
	MENU_TYPE_LIST,//菜单类型
	NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//1级菜单
		"识别图像",//中文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

        //上下（按下）切换图像
			MENU_L_2,//菜单等级
			"VSPI OLED",//中文
			MENU_TYPE_FUN,//菜单类型
			test_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"矫正陀螺仪",//中文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"蜂鸣器",//中文
			MENU_TYPE_FUN,//菜单类型
			test_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行


		MENU_L_1,//菜单等级
		"开始启动",//中文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"校准",//中文
			MENU_TYPE_FUN,//菜单类型
			test_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"测试",//中文
			MENU_TYPE_FUN,//菜单类型
			test_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		MENU_L_1,//菜单等级
		"设置舵机角度",//中文
		MENU_TYPE_FUN,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
        	
            MENU_L_2,//菜单等级
			"舵机下限",//中文
			MENU_TYPE_FUN,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"舵机上限",//中文
			MENU_TYPE_FUN,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行


		
	/*最后的菜单是结束菜单，无意义*/			
	MENU_L_0,//菜单等级
	"END",//中文
	MENU_TYPE_NULL,//菜单类型
	NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
};

//一个叫菜单的结构体
/**
 * @brief  菜单结构体
*/
typedef struct _strMenu
{
    MenuLel l;     ///<菜单等级
    char cha[MENU_LANG_BUF_SIZE];   //显示的文字 
    MenuType type;  ///菜单类型
    s32 (*fun)(void);  ///测试函数

} MENU;
//每个菜单项都是一个这个结构体

menu_run((MENU *)&MenuList[0], sizeof(WJQTestList)/sizeof(MENU), FONT_SONGTI_1616, 2);
menu_scan;
key_scan;