
//#include "globals.h"
#include "keyboard.h"
#include "i2c.h"
//#include <stdio.h>
#include "timer1.h"




#define KBD_PIN_LEFT		PD7
#define KBD_PIN_RIGHT		PD6
#define KBD_PIN_UP			PD5
#define KBD_PIN_DOWN		PD4
#define KBD_PIN_OK			PD3
#define KBD_PIN_GND			PD2

#define KBD_MASK (_BV(KBD_PIN_UP)|_BV(KBD_PIN_DOWN)|_BV(KBD_PIN_OK))

//#define KBD_MASK2 (_BV(KBD_PIN_DOWN)|_BV(KBD_PIN_OK)|_BV(KBD_PIN_GND))

#define KBD_PIN			PIND
#define KBD_PORT		PORTD
#define KBD_DDR			DDRD

uint8_t lastKey,Key_1,Key_2;

unsigned char *display_mas,*i2c_mas;

volatile unsigned char display_buf[20]={1,2,3,4,5,6};

unsigned char display_mask=0xFF,temp_mask=0xFF;//маска отображения для мигания



unsigned char TuneClock_Flag=0;//флаг включен режим настройки часов
unsigned char Screen_Flag=0;//какой экран сейчас отображается/настраивается
unsigned char I2C_Stop_Flag=0;
extern volatile struct pt i2c_read, i2c_write;
extern unsigned char brightness;
//------------------------------------------------
unsigned char getKeyCode(uint8_t ah);
//------------------------------------------------
unsigned char getKeyCode(uint8_t ah)
{
	if (!(ah & _BV(KBD_PIN_OK)))
	{
		return KEY_OK;
	}

	if (!(ah & _BV(KBD_PIN_UP))) 
	{
		return KEY_UP;
	}

	if (!(ah & _BV(KBD_PIN_DOWN))) 
	{
		return KEY_DOWN;
	}

	if (!(ah & _BV(KBD_PIN_LEFT))) 
	{
		return KEY_LEFT;
	}

	if (!(ah & _BV(KBD_PIN_GND))) 
	{
		return KEY_GND;
	}

	if (!(ah & _BV(KBD_PIN_RIGHT))) 
	{
		return KEY_RIGHT;
	}
	return 0;
}
//----------------------------
void  KBD_init() 
{
	KBD_PORT |= KBD_MASK;
	KBD_DDR &= ~ KBD_MASK;

	i2c_mas=display_mas=&display_buf;

	lastKey = 0;
	temp_mask=0xFF;
}
//--------------------------------
PT_THREAD(Keyboard_Process(struct pt *pt))
{
   PT_BEGIN(pt);

   while(1) 
   {
   		PT_DELAY(pt,60);
		Key_1=getKeyCode(KBD_PIN & KBD_MASK);
		PT_DELAY(pt,10);
		Key_2=getKeyCode(KBD_PIN & KBD_MASK);

		if((Key_1==Key_2)&&(Key_2!=lastKey)&&(Key_2!=0x0))
		{
			lastKey=Key_2;

			if(Key_2==KEY_OK)
			{
				TuneClock_Flag++;
				if(TuneClock_Flag>3)
				{
					TuneClock_Flag=0;
				}

				if(TuneClock_Flag==0)
				{
					//сохранить время
					if(Screen_Flag==0)//сохраняем часы, минуты, 
					{
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x0,0x0));
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x1,(display_mas[2]|(display_mas[3]<<4))));
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x2,(display_mas[4]|(display_mas[5]<<4))));
						   PT_DELAY(pt,5);
					}
					
					if(Screen_Flag==1)//сохраняем календарь
					{
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x6,(display_mas[0]|(display_mas[1]<<4))));
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x5,(display_mas[2]|(display_mas[3]<<4))));
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0x4,(display_mas[4]|(display_mas[5]<<4))));
						   PT_DELAY(pt,5);
					}

					if(Screen_Flag==2)//сохраняем яркость
					{
						   PT_DELAY(pt,5);
						   PT_SPAWN(pt, &i2c_write,Write_I2C(&i2c_write,TMR_ADDR,0xF,brightness));
						   PT_DELAY(pt,5);						
					}
					//возобновить опрос
					I2C_Stop_Flag=0;
					temp_mask=0xFF;
				}
				else
				{
					//остановить опрос i2c
					I2C_Stop_Flag=1;
					if(TuneClock_Flag==1)
					{
						temp_mask=0b11111100;
						if(Screen_Flag==0)
						{
							TuneClock_Flag=2;
						}
					}
					if(TuneClock_Flag==2)
					{
						temp_mask=0b11110011;
					}
					if(TuneClock_Flag==3)
					{
						temp_mask=0b11001111;
					}

					//--------------------
					if(Screen_Flag==2)
					{
						temp_mask=0b11111110;
						TuneClock_Flag=4;
					}
					//--------------------
				}
			}
			
			if(Key_2==KEY_UP)
			{
				if(!TuneClock_Flag)//выбираем экран
				{
					Screen_Flag++;
					if(Screen_Flag==0)
					{
						display_mas=&display_buf[0]                                    ;
					}
					
					if(Screen_Flag==1)
					{
						display_mas=&display_buf[6];
					}

					if(Screen_Flag==2)
					{
						display_mas=&display_buf[12];
						display_mas[0]=brightness;
					}

					if(Screen_Flag==3)
					{
						display_mas=&display_buf[0]; 
						Screen_Flag=0;
					}
				}
				else
				{
					unsigned char tmp=0;
					if(Screen_Flag==0)//редактируем часы, минуты
					{			
						if(TuneClock_Flag==2)
						{
							tmp=display_mas[3]*10+display_mas[2]+1;
							
							if(tmp>=60)
							{
								tmp=0;
							}
							display_mas[2]=tmp%10;
							display_mas[3]=tmp/10;
						}

						if(TuneClock_Flag==3)
						{
							tmp=display_mas[5]*10+display_mas[4]+1;
							
							if(tmp>=24)
							{
								tmp=0;
							}
							display_mas[4]=tmp%10;
							display_mas[5]=tmp/10;
						}
					}
					//-------------------------------
					if(Screen_Flag==1)//редактируем день, месяц, год
					{
						if(TuneClock_Flag==1)
						{
							tmp=display_mas[1]*10+display_mas[0]+1;
							
							if(tmp>=99)
							{
								tmp=0;
							}
							display_mas[0]=tmp%10;
							display_mas[1]=tmp/10;						
						}

						if(TuneClock_Flag==2)
						{
							tmp=display_mas[3]*10+display_mas[2]+1;
							
							if(tmp>=13)
							{
								tmp=1;
							}
							display_mas[2]=tmp%10;
							display_mas[3]=tmp/10;
						
						}

						if(TuneClock_Flag==3)
						{
							tmp=display_mas[5]*10+display_mas[4]+1;
							
							if(tmp>=32)
							{
								tmp=1;
							}
							display_mas[4]=tmp%10;
							display_mas[5]=tmp/10;
						}						
					}
					//-----------------------------------
					if(Screen_Flag==2)
					{
						brightness++;
						if(brightness>9)
						{
							brightness=0;
						}
						display_mas[0]=brightness;
						
						TCCR1B&=!((1<<CS12)|(1<<CS11)|(1<<CS10));
						OCR1AH=0xFF;
						OCR1AL=0xB*brightness+5;
						TCCR1B|=((0<<CS12)|(1<<CS11)|(1<<CS10));
					}
				}
			}

		/*	if(Key_2==KEY_DOWN)
			{
				
			}*/
		}
   }
   PT_END(pt);
}
//---------------------------------------------------------
PT_THREAD(Blink_Process(struct pt *pt))
 {
   PT_BEGIN(pt);

  while(1) 
   {
		PT_DELAY(pt,250);
		display_mask=temp_mask;
		PT_DELAY(pt,250);
		display_mask=0xFF;
   }
   PT_END(pt);
 }
