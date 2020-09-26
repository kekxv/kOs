#ifndef _LQOLED_H
#define _LQOLED_H
#include "stdlib.h"
#include "main.h"
#include "util.h"


//汉字大小，英文数字大小
#define 	TYPE8X16		1
#define 	TYPE16X16		2
#define 	TYPE6X8			3

//-----------------OLED端口定义----------------

#define LCD_SCL_CLR()	UtilGpio('B',12,GPIO_PIN_LOW);
#define LCD_SCL_SET()	UtilGpio('B',12,GPIO_PIN_HIGH);

#define LCD_SDA_CLR()	UtilGpio('B',13,GPIO_PIN_LOW);
#define LCD_SDA_SET()	UtilGpio('B',13,GPIO_PIN_HIGH);

#define LCD_RST_CLR()	UtilGpio('B',14,GPIO_PIN_LOW);
#define LCD_RST_SET()	UtilGpio('B',14,GPIO_PIN_HIGH);

#define LCD_DC_CLR()	UtilGpio('B',15,GPIO_PIN_LOW);
#define LCD_DC_SET()	UtilGpio('B',15,GPIO_PIN_HIGH);


extern void LCD_Init(void);
extern void LCD_CLS(void);
extern void LCD_CLS_y(char y);
extern void LCD_CLS_line_area(uint8_t start_x,uint8_t start_y,uint8_t width);
extern void LCD_P6x8Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F6x8);
extern void LCD_P8x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F8x16);
extern void LCD_P14x16Str(uint8_t x,uint8_t y,uint8_t ch[],const uint8_t *F14x16_Idx,const uint8_t *F14x16);
extern void LCD_P16x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F16x16_Idx,const uint8_t *F16x16);
//extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch);
extern void LCD_PutPixel(uint8_t x,uint8_t y);
extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch,uint8_t char_size, uint8_t ascii_size);
extern void LCD_Rectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t gif);
extern void Draw_BMP(uint8_t x,uint8_t y,const uint8_t *bmp); 
extern void LCD_Fill(uint8_t dat);
#endif

