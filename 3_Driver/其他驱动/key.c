#include "key.h"

//初始化按键
void KEY_Pin_Init(void)
{
    gpio_init(KEY1, GPI, 1, GPI_PULL_UP);           //
    gpio_init(KEY2, GPI, 1, GPI_PULL_UP);           //
    gpio_init(KEY3, GPI, 1, GPI_PULL_UP);           //
    gpio_init(KEY4, GPI, 1, GPI_PULL_UP);           //
    gpio_init(KEY5, GPI, 1, GPI_PULL_UP);           //
    gpio_init(KEY6, GPI, 1, GPI_PULL_UP);           //
}

//读取按键的函数
uint8_t KEY1_Read()
{
    return gpio_get_level(KEY1);
}

uint8_t KEY2_Read()
{
    return gpio_get_level(KEY2);
}

uint8_t KEY3_Read()
{
    return gpio_get_level(KEY3);
}

uint8_t KEY4_Read()
{
    return gpio_get_level(KEY4);
}

uint8_t KEY5_Read()
{
    return gpio_get_level(KEY5);
}

uint8_t KEY6_Read()
{
    return gpio_get_level(KEY6);
}