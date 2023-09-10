#include "LCD.h"
#include "font.h"
#include "stm8l15x_it.h"

unsigned char LCD_RAM[128][8] = {0};

/************************************************************************************/
void writecommand(int cmd) // 写指令
{
    int i;
    GPIO_ResetBits(LCD_GPIO_PORT, LCD_CS_PIN);
    GPIO_ResetBits(LCD_GPIO_PORT, LCD_A0_PIN);
    for (i = 0; i < 8; i++)
    {
        if (cmd & 0x80)
            GPIO_SetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        else
            GPIO_ResetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        GPIO_ResetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        GPIO_SetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        cmd = cmd << 1;
    }
    GPIO_SetBits(LCD_GPIO_PORT, LCD_CS_PIN);
}
/****************************************************/
void writedata(int dat) // 写数据
{
    int i;
    GPIO_ResetBits(LCD_GPIO_PORT, LCD_CS_PIN);
    GPIO_SetBits(LCD_GPIO_PORT, LCD_A0_PIN);
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)
            GPIO_SetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        else
            GPIO_ResetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        GPIO_ResetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        GPIO_SetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        dat = dat << 1;
    }
    GPIO_SetBits(LCD_GPIO_PORT, LCD_CS_PIN);
}
/****************************************************/
void writedata1(int dat) // 写数据  CS=1
{
    int i;
    GPIO_SetBits(LCD_GPIO_PORT, LCD_CS_PIN);
    GPIO_SetBits(LCD_GPIO_PORT, LCD_A0_PIN);
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)
            GPIO_SetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        else
            GPIO_ResetBits(LCD_GPIO_PORT, LCD_SDA_PIN);
        GPIO_ResetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        GPIO_SetBits(LCD_GPIO_PORT, LCD_SCL_PIN);
        dat = dat << 1;
    }
    GPIO_SetBits(LCD_GPIO_PORT, LCD_CS_PIN);
}
/****************************************************/
void delay(uint32_t delay) // 延迟程序
{
    uint32_t i, j;

    for (i = 0; i < delay; i++)
        for (j = 0; j < 100; j++)
            ;
}
/****************************************************/
void lcd_init(void) // 初使化
{
    GPIO_Init(LCD_GPIO_PORT, LCD_CS_PIN | LCD_RES_PIN | LCD_A0_PIN | LCD_SCL_PIN | LCD_SDA_PIN, GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_SetBits(LCD_GPIO_PORT, LCD_RES_PIN);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    delay(30);
    GPIO_ResetBits(LCD_GPIO_PORT, LCD_RES_PIN);
    delay(30);
    GPIO_SetBits(LCD_GPIO_PORT, LCD_RES_PIN);
    delay(100);

    writecommand(0xE2);

    delay(10);

    writecommand(0xA2); // 0xa2设置偏压比为1/9

    writecommand(0xA0); // 0xA0设置列地址从00H开始

    writecommand(0xC8); // 0xc8设置com扫描方向，从COM(n-1)到 COM0

    writecommand(0x23); // vop粗调

    writecommand(0x81); // vop双指令

    writecommand(0x32); // vop微调

    writecommand(0x2F); // 0x2f电源状态、输出缓冲开、内部电压调整开，电压调节开关

    writecommand(0xB0); // 0xb0设置页列地址

    writecommand(0xAF); // 0xaf设置显示LCD开关

    writecommand(0xA6); // 0xa6设置正常显开关

    LCD_Clear();
}

void LCD_Clear(void)
{
    unsigned char i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            LCD_RAM[n][i] = 0X00;
    LCD_Refresh_Gram();
}
void LCD_Refresh_Gram(void)
{
    unsigned char i, n;
    for (i = 0; i < 8; i++)
    {
        writecommand(0xb0 + i);
        writecommand(0x00);
        writecommand(0x10);
        for (n = 0; n < 128; n++)
            writedata(LCD_RAM[n][i]);
    }
}
void LCD_DrawPoint(unsigned char x, unsigned char y, unsigned char t)
{
    unsigned char pos, bx, temp = 0;
    if (x > 127 || y > 63)
        return;
    pos = y / 8;
    bx = y % 8;
    temp = 1 << bx;
    if (t)
        LCD_RAM[x][pos] |= temp;
    else
        LCD_RAM[x][pos] &= ~temp;
}
void LCD_Fill(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char dot)
{
    unsigned char x, y;
    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
            LCD_DrawPoint(x, y, dot);
    }
}

void LCD_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size, unsigned char mode)
{
    unsigned char temp, t, t1;
    unsigned char y0 = y;
    if (size == 19)
    {
        chr = chr - '0';
        size = 48;
        for (t = 0; t < size; t++)
        {
            temp = pic_number_19x16[chr][t];
            for (t1 = 0; t1 < 8; t1++)
            {
                if (temp & 0x80)
                    LCD_DrawPoint(x, y, mode);
                else
                    LCD_DrawPoint(x, y, !mode);
                temp <<= 1;
                y++;
                if ((y - y0) == 19)
                {
                    y = y0;
                    x++;
                    break;
                }
            }
        }
    }
    else
    {
        chr = chr - ' '; //???????

        for (t = 0; t < size; t++)
        {
            if (size == 12)
            {
                temp = oled_asc2_1206[chr][t];
            }
            else if (size == 16)
            {
                temp = oled_asc2_1608[chr][t];
            }
            for (t1 = 0; t1 < 8; t1++)
            {
                if (temp & 0x80)
                    LCD_DrawPoint(x, y, mode);
                else
                    LCD_DrawPoint(x, y, !mode);
                temp <<= 1;
                y++;
                if ((y - y0) == size)
                {
                    y = y0;
                    x++;
                    break;
                }
            }
        }
    }
}

static uint32_t oled_pow(unsigned char m, unsigned char n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

void LCD_ShowNum(unsigned char x, unsigned char y, uint32_t num, unsigned char len, unsigned char _isZeroShow, unsigned char size)
{
    unsigned char t, temp;
    unsigned char enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (!_isZeroShow)
                {
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
                    continue;
                }
            }
            else
                enshow = 1;
        }
        if (size == 19)
        {
            LCD_ShowChar(x + 16 * t, y, temp + '0', size, 1);
        }
        else
        {
            LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
        }
    }
}
void LCD_ShowString(unsigned char x, unsigned char y, char *p, unsigned char size)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while (*p != '\0')
    {
        if (x > MAX_CHAR_POSX)
        {
            x = 0;
            y += 16;
        }
        if (y > MAX_CHAR_POSY)
        {
            y = x = 0;
            LCD_Clear();
        }
        LCD_ShowChar(x, y, *p, size, 1);
        if (size == 19)
        {
            x += 16;
        }
        else if (size == 16)
        {
            x += 8;
        }
        else
        {
            x += 6;
        }

        p++;
    }
}
void GUI_Line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill)
{
    unsigned char x, y;
    unsigned char dx;
    unsigned char dy;
    if (y0 == y1)
    {
        if (x0 <= x1)
        {
            x = x0;
        }
        else
        {
            x = x1;
            x1 = x0;
        }
        while (x <= x1)
        {
            LCD_DrawPoint(x, y0, fill);
            x++;
        }
        return;
    }
    else if (y0 > y1)
    {
        dy = y0 - y1;
    }
    else
    {
        dy = y1 - y0;
    }

    if (x0 == x1)
    {
        if (y0 <= y1)
        {
            y = y0;
        }
        else
        {
            y = y1;
            y1 = y0;
        }
        while (y <= y1)
        {
            LCD_DrawPoint(x0, y, fill);
            y++;
        }
        return;
    }
    else if (x0 > x1)
    {
        dx = x0 - x1;
        x = x1;
        x1 = x0;
        y = y1;
        y1 = y0;
    }
    else
    {
        dx = x1 - x0;
        x = x0;
        y = y0;
    }

    if (dx == dy)
    {
        while (x <= x1)
        {
            x++;
            if (y > y1)
            {
                y--;
            }
            else
            {
                y++;
            }
            LCD_DrawPoint(x, y, fill);
        }
    }
    else
    {
        LCD_DrawPoint(x, y, fill);
        if (y < y1)
        {
            if (dx > dy)
            {
                char p = dy * 2 - dx;
                char twoDy = 2 * dy;
                char twoDyMinusDx = 2 * (dy - dx);
                while (x < x1)
                {
                    x++;
                    if (p < 0)
                    {
                        p += twoDy;
                    }
                    else
                    {
                        y++;
                        p += twoDyMinusDx;
                    }
                    LCD_DrawPoint(x, y, fill);
                }
            }
            else
            {
                char p = dx * 2 - dy;
                char twoDx = 2 * dx;
                char twoDxMinusDy = 2 * (dx - dy);
                while (y < y1)
                {
                    y++;
                    if (p < 0)
                    {
                        p += twoDx;
                    }
                    else
                    {
                        x++;
                        p += twoDxMinusDy;
                    }
                    LCD_DrawPoint(x, y, fill);
                }
            }
        }
        else
        {
            if (dx > dy)
            {
                char p = dy * 2 - dx;
                char twoDy = 2 * dy;
                char twoDyMinusDx = 2 * (dy - dx);
                while (x < x1)
                {
                    x++;
                    if (p < 0)
                    {
                        p += twoDy;
                    }
                    else
                    {
                        y--;
                        p += twoDyMinusDx;
                    }
                    LCD_DrawPoint(x, y, fill);
                }
            }
            else
            {
                char p = dx * 2 - dy;
                char twoDx = 2 * dx;
                char twoDxMinusDy = 2 * (dx - dy);
                while (y1 < y)
                {
                    y--;
                    if (p < 0)
                    {
                        p += twoDx;
                    }
                    else
                    {
                        x++;
                        p += twoDxMinusDy;
                    }
                    LCD_DrawPoint(x, y, fill);
                }
            }
        }
    }
}

void GUI_Circle(unsigned char cx, unsigned char cy, unsigned char r, unsigned char color, unsigned char fill)
{
    unsigned char x, y;
    int delta, tmp;
    x = 0;
    y = r;
    delta = 3 - (r << 1);

    while (y > x)
    {
        if (fill)
        {
            GUI_Line(cx + x, cy + y, cx - x, cy + y, color);
            GUI_Line(cx + x, cy - y, cx - x, cy - y, color);
            GUI_Line(cx + y, cy + x, cx - y, cy + x, color);
            GUI_Line(cx + y, cy - x, cx - y, cy - x, color);
        }
        else
        {
            LCD_DrawPoint(cx + x, cy + y, color);
            LCD_DrawPoint(cx - x, cy + y, color);
            LCD_DrawPoint(cx + x, cy - y, color);
            LCD_DrawPoint(cx - x, cy - y, color);
            LCD_DrawPoint(cx + y, cy + x, color);
            LCD_DrawPoint(cx - y, cy + x, color);
            LCD_DrawPoint(cx + y, cy - x, color);
            LCD_DrawPoint(cx - y, cy - x, color);
        }
        x++;
        if (delta >= 0)
        {
            y--;
            tmp = (x << 2);
            tmp -= (y << 2);
            delta += (tmp + 10);
        }
        else
        {
            delta += ((x << 2) + 6);
        }
    }
}
void GUI_Rectangle(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char color, unsigned char fill)
{
    if (fill)
    {
        uint32_t i;
        if (x0 > x1)
        {
            i = x1;
            x1 = x0;
        }
        else
        {
            i = x0;
        }
        for (; i <= x1; i++)
        {
            GUI_Line(i, y0, i, y1, color);
        }
        return;
    }
    GUI_Line(x0, y0, x0, y1, color);
    GUI_Line(x0, y1, x1, y1, color);
    GUI_Line(x1, y1, x1, y0, color);
    GUI_Line(x1, y0, x0, y0, color);
}