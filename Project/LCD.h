/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _LCD_H
#define _LCD_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stdbool.h"

#define LCD_GPIO_PORT GPIOE
#define LCD_CS_PIN GPIO_Pin_0
#define LCD_RES_PIN GPIO_Pin_1
#define LCD_A0_PIN GPIO_Pin_2
#define LCD_SCL_PIN GPIO_Pin_3
#define LCD_SDA_PIN GPIO_Pin_4

void lcd_init(void); // 初使化
void disp_clear(void);
void disp_full(void);
void disp_col(void);
void disp_row(void); // 竖条
void dispgraphiCS(void);

void LCD_Clear(void);
void LCD_Refresh_Gram(void);
void LCD_DrawPoint(unsigned char x, unsigned char y, unsigned char t);
void LCD_Fill(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char dot);
void LCD_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size, unsigned char mode);
void LCD_ShowNum(unsigned char x, unsigned char y, uint32_t num, unsigned char len, unsigned char _isZeroShow, unsigned char size);
void LCD_ShowString(unsigned char x, unsigned char y, char *p, unsigned char size);
void GUI_Line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill);
void GUI_Circle(unsigned char cx, unsigned char cy, unsigned char r, unsigned char color, unsigned char fill);
void GUI_Rectangle(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char color, unsigned char fill);

#endif
