#include "oled.h"
#include "u8g2/u8g2.h"

// OLED display instance
u8g2_t u8g2;

// Display buffer for 128x64 SSD1306 (128x64/8 = 1024 bytes)
static uint8_t u8g2_buf[U8G2_BUFFER_SIZE(U8G2_R0)];

// I2C communication callback function
static uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_DELAY_MILLI:
            delay_ms(arg_int);  // 使用项目中已有的延迟函数
            break;
        case U8X8_MSG_GPIO_SCL:
            if (arg_int)
                OLED_SCLK_Set();
            else
                OLED_SCLK_Clr();
            break;
        case U8X8_MSG_GPIO_SDA:
            if (arg_int)
                OLED_SDIN_Set();
            else
                OLED_SDIN_Clr();
            break;
        default:
            return 0;
    }
    return 1;
}

// I2C byte transfer callback function
static uint8_t u8x8_byte_sw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;
    
    switch (msg)
    {
        case U8X8_MSG_BYTE_INIT:
            // 初始化I2C引脚
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while (arg_int > 0)
            {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            // 实现软件I2C发送
            uint8_t i, j;
            OLED_SDA_Set();
            OLED_SCLK_Set();
            delay_ms(1);
            OLED_SDA_Clr();
            delay_ms(1);
            OLED_SCLK_Clr();
            
            for (i = 0; i < buf_idx; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (buffer[i] & (0x80 >> j))
                        OLED_SDA_Set();
                    else
                        OLED_SDA_Clr();
                    
                    OLED_SCLK_Set();
                    delay_ms(1);
                    OLED_SCLK_Clr();
                    delay_ms(1);
                }
                
                // 伪ACK处理（淘宝屏幕通常不需要ACK）
                OLED_SDA_Set();
                OLED_SCLK_Set();
                delay_ms(1);
                OLED_SCLK_Clr();
                delay_ms(1);
            }
            
            OLED_SDA_Clr();
            delay_ms(1);
            OLED_SCLK_Set();
            delay_ms(1);
            OLED_SDA_Set();
            break;
        default:
            return 0;
    }
    return 1;
}

// Initialize OLED with u8g2
void OLED_Init(void)
{
    // Initialize u8g2 with SSD1306 128x64
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_stm32);
    u8g2_SetBufferPtr(&u8g2, u8g2_buf);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearDisplay(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

// Clear OLED display
void OLED_Clear(uint8_t dat)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

// Draw a point on OLED
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    if (t)
        u8g2_DrawPixel(&u8g2, x, y);
    else
    {
        u8g2_SetDrawColor(&u8g2, 0);
        u8g2_DrawPixel(&u8g2, x, y);
        u8g2_SetDrawColor(&u8g2, 1);
    }
}

// Fill a rectangle on OLED
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t temp;
    if (x1 > x2) { temp = x1; x1 = x2; x2 = temp; }
    if (y1 > y2) { temp = y1; y1 = y2; y2 = temp; }
    
    u8g2_SetDrawColor(&u8g2, dot ? 1 : 0);
    u8g2_DrawBox(&u8g2, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    u8g2_SetDrawColor(&u8g2, 1);
}

// Refresh OLED display
void OLED_Show_Display(void)
{
    u8g2_SendBuffer(&u8g2);
}

// Turn on OLED display
void OLED_Display_On(void)
{
    u8g2_SetPowerSave(&u8g2, 0);
}

// Turn off OLED display
void OLED_Display_Off(void)
{
    u8g2_SetPowerSave(&u8g2, 1);
}

// Write command or data to OLED (deprecated in u8g2)
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    // This function is not used in u8g2 implementation
    // u8g2 handles all low-level communication internally
    (void)dat;
    (void)cmd;
}

// Set cursor position
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    u8g2_SetFontPosTop(&u8g2);
    // u8g2 automatically handles position in drawing functions
    (void)x;
    (void)y;
}

// Show a character on OLED
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    u8g2_SetFontPosTop(&u8g2);
    
    // Set font size based on Char_Size
    switch (Char_Size)
    {
        case 12:
            u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
            break;
        case 16:
            u8g2_SetFont(&u8g2, u8g2_font_9x15_tr);
            break;
        case 24:
            u8g2_SetFont(&u8g2, u8g2_font_10x20_tr);
            break;
        default:
            u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
            break;
    }
    
    char buf[2] = {chr, 0};
    u8g2_DrawStr(&u8g2, x, y, buf);
}

// Show a number on OLED
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
    char buf[20];
    sprintf(buf, "%0*ld", len, num);
    OLED_ShowString(x, y, buf, size);
}

// Show a string on OLED
void OLED_ShowString(u8 x, u8 y, const char *p, u8 Char_Size)
{
    u8g2_SetFontPosTop(&u8g2);
    
    // Set font size based on Char_Size
    switch (Char_Size)
    {
        case 12:
            u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
            break;
        case 16:
            u8g2_SetFont(&u8g2, u8g2_font_9x15_tr);
            break;
        case 24:
            u8g2_SetFont(&u8g2, u8g2_font_10x20_tr);
            break;
        default:
            u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
            break;
    }
    
    u8g2_DrawStr(&u8g2, x, y, p);
}

// Draw bitmap on OLED
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    u8g2_DrawBitmap(&u8g2, x0, y0, (x1 - x0 + 7) / 8, y1 - y0 + 1, BMP);
}

// Fill picture (deprecated)
void fill_picture(unsigned char fill_Data)
{
    u8g2_SetDrawColor(&u8g2, fill_Data ? 1 : 0);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 64);
    u8g2_SetDrawColor(&u8g2, 1);
}

// Show picture (deprecated)
void Picture(void)
{
    // This function is usually used for demo purposes
    // Can be implemented if needed
}

// Show Chinese character (requires Chinese font support)
void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{
    // This function requires Chinese font support
    // You need to add Chinese font files to u8g2
    (void)x;
    (void)y;
    (void)no;
}
