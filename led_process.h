#ifndef LED_PROCESS_H
#define LED_PROCESS_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include <math.h>

#include "pt/pt.h"
#define LEDPORT		 	PORTB
#define LEDPORT2		PORTC

#define CS_1			LEDPORT |=(1<<PB2)
#define WR_DATA_1		LEDPORT |=(1<<PB3)
#define CLK_1			LEDPORT |=(1<<PB5)

#define CS_0			LEDPORT &=~(1<<PB2)
#define WR_DATA_0		LEDPORT &=~(1<<PB3)
#define CLK_0			LEDPORT &=~(1<<PB5)
//-----------------------------------------
#define CS_11			LEDPORT2 |=(1<<PC2)
#define WR_DATA_11		LEDPORT2 |=(1<<PC1)
#define CLK_11			LEDPORT2 |=(1<<PC0)

#define CS_10			LEDPORT2 &=~(1<<PC2)
#define WR_DATA_10		LEDPORT2 &=~(1<<PC1)
#define CLK_10			LEDPORT2 &=~(1<<PC0)
//--------------------------------------------

#define LED_DIG_NUM	5
#define INDICATOR_NUM 5

//состояния автомата передачи данных bit-bang
//-------------------------------------------
#define INDICATOR_1	0
#define INDICATOR_2	1
#define INDICATOR_3	2
#define INDICATOR_4	3

//-------------------------------------------

//void LED_Process(void);//главный процесс вывода
PT_THREAD(LED_Process(struct pt *pt));
//void LED_Out_Buf(unsigned int out_buf[], unsigned char out_buf_len);//вывод буфера в SPI bit-bang

//void LED_Out_Integer(unsigned long num);//вывод целочисленного значения
void LED_Out_Float(unsigned char display_num,float num);//вывод значения с плавающей точкой

void LED_Set_Digit_Num(unsigned char display_num,unsigned char num);//установить количество отображаемых чисел
void LED_Set_Brightness(unsigned char display_num,unsigned char bright);//установить яркость индикаторов
void LED_Set_Decode(unsigned char display_num,unsigned char decode);//установить яркость индикаторов
void LED_SetPoint(unsigned char display_num,unsigned char pnt);//на какое знакоместо ставим точку, начиная слева
void LED_SetBlink(unsigned char display_num,unsigned char blink);//мигание
PT_THREAD(LED_BlinkTask(struct pt *pt));
 void SetBarGraph(unsigned char val,unsigned char point_1,unsigned char point_2, unsigned char invert);
#endif
