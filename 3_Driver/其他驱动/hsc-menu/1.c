#include "stdio.h"

//显示的一行选项


//显示的一行选项
typedef struct Line
{
    char word[20];//标题文字，中文是2个字节
    float *num;//数值指针
    float step;//一次增加的值
}MENU;//用这个名称定义结构体

//定义结构体
const MENU p1[]= 
{
    "abc",//标题
    123,//值
    1,//一次改变的间隔
    
    "abc",//标题
    123,//值
    1,//一次改变的间隔

    "abc",//标题
    123,//值
    1,//一次改变的间隔

};




int main()
{
    int len;
    len=sizeof(p1) / sizeof(p1[0]);
    int i;
    //Page p1;
    //page1.len=3;
    //p1.l[0].word[0] = 'a';
    //page1.a1={"pid",&a,1};

    for(i=0;i<len;i++)
    {
        printf("%s",p1[i].word);
    }

    
    return 0;
}