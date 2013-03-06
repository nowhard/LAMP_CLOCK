//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include "core.h"
#include "timer1.h"
#include "adc.h"
#include <avr/pgmspace.h>
#include "i2c.h"
#include "led_process.h"
#include "ext_int.h"
 #include <avr/wdt.h> 

#define VREF 	5//опора
#define ATTEN	5052/1000 //ослабление сигнала на входе(делитель)

typedef struct PROGMEM{
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	uint8_t     Select;
	const char  Text[];
} menuItem;

menuItem* selectedMenuItem; // текущий пункт меню

volatile unsigned char time_tune_state=0, date_tune_state=0;//метки состояния настройки(настройка минута-час, день-месяц)
//extern volatile unsigned int frequency;
extern unsigned int mid_mas[];
volatile unsigned long freq=0;
//menuItem* menuStack[10];
//volatile uint8_t menuStackTop;


#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (uint8_t)Select, { Text }}

#define PREVIOUS   ((menuItem*)pgm_read_word(&selectedMenuItem->Previous))
#define NEXT       ((menuItem*)pgm_read_word(&selectedMenuItem->Next))

#define PARENT     ((menuItem*)pgm_read_word(&selectedMenuItem->Parent))
#define CHILD      ((menuItem*)pgm_read_word(&selectedMenuItem->Child))
#define SELECT		(pgm_read_byte(&selectedMenuItem->Select))

char strNULL[] PROGMEM = "";

#define NULL_ENTRY Null_Menu
menuItem        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0, {0x00}};

//                 NEXT,      PREVIOUS     PARENT,     CHILD

MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  NULL_ENTRY, m_s2i1,       MENU_TIME, 			/*time*/"");
MAKE_MENU(m_s1i2,  m_s1i3,    m_s1i1,      NULL_ENTRY, m_s3i1,       MENU_DATE, 			/*data*/"");
MAKE_MENU(m_s1i3,  m_s1i4,	  m_s1i2,      NULL_ENTRY, m_s4i1,       MENU_YEAR, 			/*year*/"");
MAKE_MENU(m_s1i4,  m_s1i5,    m_s1i3,      NULL_ENTRY, NULL_ENTRY,   MENU_TUNE_BRIGHTNESS,  "");
MAKE_MENU(m_s1i5,  m_s1i6,    m_s1i4,      NULL_ENTRY, NULL_ENTRY,   MENU_TEMPERATURE_1, 	"");
MAKE_MENU(m_s1i6,  m_s1i7,    m_s1i5,      NULL_ENTRY, NULL_ENTRY,   MENU_FREQUENCY,	 	"");
MAKE_MENU(m_s1i7,  NULL_ENTRY,m_s1i6,      NULL_ENTRY, NULL_ENTRY,   MENU_VOLTAGE, 			"");
// подменю Настройка времени
MAKE_MENU(m_s2i1,  NULL_ENTRY,NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_TUNE_TIME, 		"");
// подменю Настройка даты
MAKE_MENU(m_s3i1,  NULL_ENTRY,NULL_ENTRY,  m_s1i2,     NULL_ENTRY,   MENU_TUNE_DATE, 		"");
// подменю Настройка года
MAKE_MENU(m_s4i1,  NULL_ENTRY,NULL_ENTRY,  m_s1i3,     NULL_ENTRY,   MENU_TUNE_YEAR, 		"");


volatile unsigned char disp_buf[6]={0};//буфер дисплея
unsigned char mode=0;//режим клавиатуры(режим меню, режим правки)
Clock clk;
extern volatile unsigned char brightness;
uint8_t selectMenu(msg_par par);

void menuChange(menuItem* NewMenu)
{
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}
//------------------------------------
unsigned char dispMenu(msg_par par) 
{
//	menuItem* tempMenu;
//	tempMenu = (menuItem*)pgm_read_word(&selectedMenuItem);
//	//printf(" %c  ",SELECT+0x30);
	static unsigned char blink=100,blink_flag=0;
//	static unsigned char blink_mask=0xFF;
wdt_reset();	
	if(blink<2)
	{
		blink++;
	}	
	else
	{
		blink_flag=!blink_flag;
		blink=0;
	}

	switch(SELECT)
	{
		case MENU_TIME:
		{
		//	ReadTime(disp_buf);
			ReadTime(&clk,0);
			
			disp_buf[4]=clk.hour/10;
			disp_buf[3]=clk.hour%10;
			disp_buf[2]=0xA;
			disp_buf[1]=clk.minute/10;
			disp_buf[0]=clk.minute%10;
		//	SetPoint(3);
		//	LED_Out_Integer(clk.hour*100+clk.minute);
			LED_Out_Buf(disp_buf,5,0xFF);
		}
		break;
		
		case MENU_DATE:
		{
			ReadTime(&clk,1);

			disp_buf[4]=0xF;
			disp_buf[1]=clk.month/10;
			disp_buf[0]=(clk.month%10);
			disp_buf[3]=clk.day/10;
			disp_buf[2]=(clk.day%10)|0x80;

			LED_Out_Buf(disp_buf,5,0xFF);
		}
		break;

		case MENU_YEAR:
		{
			ReadTime(&clk,2);

			disp_buf[4]=0xF;
			disp_buf[3]=2;
			disp_buf[2]=0;
			disp_buf[1]=clk.year/10;
			disp_buf[0]=clk.year%10;

			LED_Out_Buf(disp_buf,5,0xFF);
		}
		break;

		case MENU_TUNE_BRIGHTNESS:
		{
			disp_buf[4]=0xF;
			disp_buf[3]=0xF;
			disp_buf[2]=0xF;
			if(brightness>9)
			{
				disp_buf[1]=brightness/10;
				disp_buf[0]=brightness%10;
			}
			else
			{
				disp_buf[1]=0xF;
				disp_buf[0]=brightness;
			}
			

			LED_Out_Buf(disp_buf,5,0xFF);
		}
		break;

		case MENU_TEMPERATURE_1:
		{
			static  int cap_temp,temperature;
			static unsigned char cap_temp_count=0;
				
			if(cap_temp_count<5)
			{
				cap_temp+=(int)(( long)((int)ReadADC(1)-(int)ReadADC(0))*VREF*100/1023);	
				cap_temp_count++;
			}
			else
			{
				temperature=cap_temp/5;
				cap_temp_count=0;
				cap_temp=0;
			}
			SetPoint(0);
			LED_Out_Integer(temperature);
		}
		break;

		case MENU_FREQUENCY:
		{
			unsigned char i;
			freq=0;
			for(i=0;i<MID_MAS_SIZE;i++)
			{
				freq+=mid_mas[i];
			}
			if(freq!=0)
			{
				freq=1875000*MID_MAS_SIZE/freq;
			}
			SetPoint(0);
			LED_Out_Integer((unsigned int)freq);
		}
		break;

		case MENU_VOLTAGE:
		{
			SetPoint(2);
			LED_Out_Integer((unsigned int)((unsigned long)ReadADC(2)*VREF*10*ATTEN/1023));
		}
		break;

		case MENU_TUNE_TIME:
		{
			disp_buf[4]=clk.hour/10;
			disp_buf[3]=clk.hour%10;
			disp_buf[2]=0xA;
			disp_buf[1]=clk.minute/10;
			disp_buf[0]=clk.minute%10;

			if(blink_flag)
			{
				if(time_tune_state)
				{
					LED_Out_Buf(disp_buf,5,0xFC);
				}
				else
				{
					LED_Out_Buf(disp_buf,5,0xE7);
				}
			}
			else
			{
				LED_Out_Buf(disp_buf,5,0xFF);
			}
		}
		break;

		case MENU_TUNE_DATE:
		{
			disp_buf[4]=0xF;
			disp_buf[1]=clk.month/10;
			disp_buf[0]=(clk.month%10);
			disp_buf[3]=clk.day/10;
			disp_buf[2]=(clk.day%10)|0x80;

			if(blink_flag)
			{
				if(date_tune_state)
				{
					LED_Out_Buf(disp_buf,5,0xE7);
				}
				else
				{
					LED_Out_Buf(disp_buf,5,0xF9);
				}
			}
			else
			{
				LED_Out_Buf(disp_buf,5,0xFF);
			}
		}
		break;

		case MENU_TUNE_YEAR:
		{
			disp_buf[4]=0xF;
			disp_buf[3]=2;
			disp_buf[2]=0;
			disp_buf[1]=clk.year/10;
			disp_buf[0]=clk.year%10;

			if(blink_flag)
			{
				LED_Out_Buf(disp_buf,5,0xE7);
			}
			else
			{
				LED_Out_Buf(disp_buf,5,0xFF);
			}
		}
		break;

		default:
		{
		}
		break;
	}

	setTimer(MSG_DISP_REFRESH, 0, 250);
	return (1);
}

uint8_t menuKey(msg_par par) {
	switch (par) 
	{
		case 0: 
		{
			return 1;
		}
		//------------------------
		case KEY_LEFT: 
		{
			switch(SELECT)//пункт меню
			{			
				case MENU_TUNE_TIME://определяем поведение кнопок в этом меню
				{
					time_tune_state=0;
				}
				break;

				case MENU_TUNE_DATE:
				{
					date_tune_state=0;
				}
				break;
				
				default:
				{
					menuChange(PREVIOUS);
				}
				break;				
			}
		}
		break;

		//------------------------
		case KEY_RIGHT: 
		{
			switch(SELECT)//пункт меню
			{			
				case MENU_TUNE_TIME:
				{
					time_tune_state=1;
				}
				break;

				case MENU_TUNE_DATE:
				{
					date_tune_state=1;
				}
				break;
				
				default:
				{
					menuChange(NEXT);
				}
				break;				
			}

		}
		break;
		//------------------------
		case KEY_UP:
		{ 
			switch(SELECT)//пункт меню
			{			
				case MENU_TUNE_BRIGHTNESS:
				{	
					if(brightness>=0xF)
					{
						brightness=0xF;	
					}
					else
					{
						brightness=(brightness+1)&0xF;	
						I2C_Write_Byte(SLA_ADDR,0x0,brightness);
					}
				}
				break;

				case MENU_TUNE_TIME:
				{
					if(time_tune_state)
					{
						if(clk.hour<23)
						{
							clk.hour++;
						}
						else
						{
							clk.hour=23;	
						}
					}
					else
					{
						if(clk.minute<59)
						{
							clk.minute++;
						}
						else
						{
							clk.minute=59;
						}
					}
				}
				break;

				case MENU_TUNE_DATE:
				{
					if(date_tune_state)
					{
						if(clk.month<12)
						{
							clk.month++;
						}
					}
					else
					{
						if(clk.day<31)
						{
							clk.day++;
						}
					}
				}
				break;

				case MENU_TUNE_YEAR:
				{
					if(clk.year>=99)
					{
						clk.year=99;
					}
					else
					{
						clk.year++;	
					}
				}
				break;
				
				default:
				{
					//menuChange(NEXT);
				}
				break;				
			}			
		}
		break;
		//------------------------
		case KEY_DOWN:
		{
			switch(SELECT)//пункт меню
			{			
				case MENU_TUNE_BRIGHTNESS:
				{

					if(brightness<=0x1)
					{
						brightness=0x1;	
					}
					else
					{
						brightness=(brightness-1)&0xF;	
						I2C_Write_Byte(SLA_ADDR,0x0,brightness);					
					}
				}
				break;

				case MENU_TUNE_TIME:
				{
					if(time_tune_state)
					{
						if(clk.hour>0)
						{
							clk.hour--;
						}
					}
					else
					{
						if(clk.minute>0)
						{
							clk.minute--;
						}
					}				
				}
				break;

				case MENU_TUNE_DATE:
				{
					if(date_tune_state)
					{
						if(clk.month>1)
						{
							clk.month--;
						}
					}
					else
					{
						if(clk.day>1)
						{
							clk.day--;
						}
					}
				}
				break;

				case MENU_TUNE_YEAR:
				{	
					if(clk.year<=0)
					{
						clk.year=0;
					}
					else
					{
						clk.year--;	
					}
				}
				break;
				
				default:
				{
					
				}
				break;				
			}
		}
		break;
			
		//------------------------
		case KEY_OK:
		{ // выбор пункта
			switch(SELECT)//пункт меню
			{			
				case MENU_TUNE_TIME:
				{
					StoreTime(&clk,0);
					menuChange(PARENT);
				}
				break;

				case MENU_TUNE_DATE:
				{
					StoreTime(&clk,1);
					menuChange(PARENT);
				}
				break;

				case MENU_TUNE_YEAR:
				{	
					StoreTime(&clk,2);
					menuChange(PARENT);
				}
				break;
				
				default:
				{
					menuChange(CHILD);	
				}
				break;				
			}		
		}
		break;
		//------------------------
		case KEY_GND:// отмена выбора (возврат)
		{
			menuChange(PARENT);
		}
		break;
		//------------------------
		default:
		{
		}
		break;
	}
	//dispMenu(0);
	return (1);
}
//---------------------------------------------------
unsigned char startMenu() 
{
	selectedMenuItem = (menuItem*)&m_s1i1;
	//printf("ERROR");
//	dispMenu(0);
	setHandler(MSG_KEY_PRESS, &menuKey);
	setHandler(MSG_DISP_REFRESH, &dispMenu);
	setTimer(MSG_DISP_REFRESH, 0, 250);
//	setHandler(MSG_MENU_SELECT, &selectMenu);
	return (0);
}
//--------------------------------------------------
/*uint8_t selectMenu(msg_par par) {

//printf("RECIEVED %c ",par+0x30);

	switch (par) {
	case MENU_TUNE_TIME: 
	{ 
//		printf("1_MENU_TUNE_TIME  ");
		
	}
	break;

	case MENU_TUNE_DATE: 
	{ 
//		printf("2_MENU_TUNE_DATE  ");
		
	}
	break;

	case MENU_TUNE_YEAR: 
	{ 
//		printf("3_MENU_TUNE_YEAR  ");
		
	}
	break;

	case MENU_TUNE_BRIGHTNESS: 
	{ 
//		printf("4_MENU_TUNE_BRIGHTNESS  ");
		
	}
	break;
	
	}


//	startMenu();
	return 1;
}*/
