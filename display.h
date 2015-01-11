// управление LCD
// 08-Dec-2014
// 17-Dec-2014
// HI-TECH C Compiler


#include "font.h"


#define SET_BIT(reg,bit) reg |= (1<<bit)
#define CLEAR_BIT(reg,bit) reg &= (~(1<<bit))


typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;


#define LCD_PWR_PIN 2
#define LCD_PWR_TRIS TRISD
#define LCD_PWR_PORT PORTD

#define LCD_SCK_PIN 1
#define LCD_SCK_TRIS TRISD
#define LCD_SCK_PORT PORTD

#define LCD_MOSI_PIN 0
#define LCD_MOSI_TRIS TRISD
#define LCD_MOSI_PORT PORTD

#define LCD_DC_PIN 2
#define LCD_DC_TRIS TRISC
#define LCD_DC_PORT PORTC

#define LCD_CS_PIN 1
#define LCD_CS_TRIS TRISC
#define LCD_CS_PORT PORTC

#define LCD_RST_PIN 0
#define LCD_RST_TRIS TRISC
#define LCD_RST_PORT PORTC


#define LCD_PWR_H  SET_BIT(LCD_PWR_PORT, LCD_PWR_PIN)
#define LCD_PWR_L  CLEAR_BIT(LCD_PWR_PORT, LCD_PWR_PIN)

#define LCD_SCK_H  SET_BIT(LCD_SCK_PORT, LCD_SCK_PIN)
#define LCD_SCK_L  CLEAR_BIT(LCD_SCK_PORT, LCD_SCK_PIN)

#define LCD_MOSI_H  SET_BIT(LCD_MOSI_PORT, LCD_MOSI_PIN)
#define LCD_MOSI_L  CLEAR_BIT(LCD_MOSI_PORT, LCD_MOSI_PIN)

#define LCD_DC_H  SET_BIT(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_DC_L  CLEAR_BIT(LCD_DC_PORT, LCD_DC_PIN)

#define LCD_CS_H  SET_BIT(LCD_CS_PORT, LCD_CS_PIN)
#define LCD_CS_L  CLEAR_BIT(LCD_CS_PORT, LCD_CS_PIN)

#define LCD_RST_H  SET_BIT(LCD_RST_PORT, LCD_RST_PIN)
#define LCD_RST_L  CLEAR_BIT(LCD_RST_PORT, LCD_RST_PIN)


void _lcd_write(uint8_t data, uint8_t mode);
void _lcd_goto(uint8_t line, uint8_t row);
void lcd_clear(void);
void lcd_init(void);
void _lcd_char(uint8_t c, uint8_t wide, uint8_t v, uint8_t h);
void lcd_print(uint8_t v, uint8_t h, const char *c);
void lcd_print_wide(uint8_t v, uint8_t h, const char *c);


///////////////////////////////////////////////////////////////////////////////////////////////////
void _lcd_write(uint8_t data, uint8_t mode) //отправка данных/команд в дисплей
{
uint8_t s=0x80;

LCD_MOSI_L;
LCD_SCK_L;
if(mode)LCD_DC_H; //data
else LCD_DC_L; //command
LCD_CS_L;
for(uint8_t i=0; i<8; i++)
	{
	if(data & s) LCD_MOSI_H;
	else LCD_MOSI_L;
	s = s>>1;
	LCD_SCK_H;
	LCD_SCK_L;
	}
LCD_CS_H;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void _lcd_goto(uint8_t line, uint8_t row) //позиция //0..7 //0..100
{
_lcd_write(line+64,0);
_lcd_write(row+128,0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_clear(void) //чистим экран
{
_lcd_goto(0,0);
for(uint16_t i=0; i<((101*64)/8+8); i++) _lcd_write(0,1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_init(void) //включаем дисплей
{
CLEAR_BIT(LCD_PWR_TRIS, LCD_PWR_PIN); //питание  lcd
LCD_PWR_H;

CLEAR_BIT(LCD_RST_TRIS, LCD_RST_PIN);
LCD_RST_L;

CLEAR_BIT(LCD_SCK_TRIS, LCD_SCK_PIN);
LCD_SCK_L;

CLEAR_BIT(LCD_MOSI_TRIS, LCD_MOSI_PIN);
LCD_MOSI_L;

CLEAR_BIT(LCD_DC_TRIS, LCD_DC_PIN);
LCD_DC_L;

CLEAR_BIT(LCD_CS_TRIS, LCD_CS_PIN);
LCD_CS_L;

LCD_RST_H;

//инициализация дисплея
_lcd_write(0x21,0); //chip is active, extended instruction set
_lcd_write(0x09,0); //set HV-gen voltage multiplier factor
_lcd_write(0xF7,0); //set Vop
_lcd_write(0x14,0); //bias system
_lcd_write(0x06,0); //temperature control
_lcd_write(0x20,0); //standart instruction set
_lcd_write(0x0C,0); //normal mode, display non-inverted

lcd_clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_off(void) //выключаем дисплей
{
LCD_RST_L;
LCD_CS_L;
LCD_DC_L;
LCD_MOSI_L;
LCD_SCK_L;
LCD_PWR_L; //питание  lcd
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void _lcd_char(uint8_t c, uint8_t wide, uint8_t v, uint8_t h) //рисуем символ
{
if(wide) h+=1;
_lcd_goto(8-v, 100-h*8);

if(c<0x20 || c>0x7f) c=0x20;
int16_t pos = 8*(c-0x20);
for(uint8_t i=0; i<8; i++)
	{
	_lcd_write(font8x8[pos+i],1);
	if(wide) _lcd_write(font8x8[pos+i],1);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_print(uint8_t v, uint8_t h, const char *c) //рисуем строку
{
for(uint8_t i=0; ((i<12) && (*(c+i) != '\0')); i++) _lcd_char(*(c+i),0,v,h+i);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_print_wide(uint8_t v, uint8_t h, const char *c) //рисуем строку широкими символами
{
for(uint8_t i=0; ((i<6) && (*(c+i) != '\0')); i++) _lcd_char(*(c+i),1,v,h+i*2);
}
