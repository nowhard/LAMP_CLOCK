#include "led_process.h"
#include "pt/pt.h"
#include <string.h>

//----------------------------------------------

unsigned char  spi_bit_counter;//счетчик передаваемых битов при bit-bang
unsigned int   spi_data;//данные bit-bang

unsigned char  spi_delay;//задержка между тактами
unsigned int   spi_buf[20]={0xC01,0x9FF,0xF00,0xA0E,0xB05,0x505,0x404,0x303,0x208,0x109,0x00,0x00};
unsigned char  spi_buf_length;
unsigned char  spi_buf_counter;

unsigned int  init_buf[8]={0x100,0x200,0x300,0x400,0x500,0x600,0x700,0x800};

unsigned char  num_conv_counter=0;

unsigned long  integer_num=0;

unsigned char  flag_blink=0;//позволяет запустить таймер всего один раз

//----------------------------------------------
unsigned int indicator_brightness[INDICATOR_NUM]={0xA0F,0xA0F,0xA0F,0xA0F};
unsigned int indicator_scan[INDICATOR_NUM]={0xB04,0xB07,0xB04,0xB04};
unsigned char indicator_point[INDICATOR_NUM]={0x5,0x0,0x5,0x5};
unsigned char indicator_blink[INDICATOR_NUM]={0x0,0x0,0x0,0x0};
unsigned int indicator_decode[INDICATOR_NUM]={0x9FF,0x900,0x9FF,0x9FF};

long  indicator_buf[INDICATOR_NUM];//буфер значений индикаторов

unsigned char current_indicator=INDICATOR_1;//индикатор, захвативший шину

//-------------protothreads---------------------


 unsigned char   LED_BAR_STAMP_RED[4]   ={8 ,128,4 ,16};
 unsigned char   LED_BAR_STAMP_GREEN[4] ={64,2  ,32,1 };
 unsigned char   LED_BAR_STAMP_ORANGE[4]={72,130,36,17};
//----------------------------------------------
struct led_bar_struct
{		  
	unsigned char val;
	signed char 		  point_1;
	signed char 		  point_2;
    unsigned char invert;
}  led_bar;

char  i=0;
//----------------------------------------------
PT_THREAD(LED_Process(struct pt *pt))
 {

  PT_BEGIN(pt);

  while(1) 
  {
  		//PT_WAIT_UNTIL(pt,LED_START); //ждем команды на старт
		//LED_START=0;

	   	PT_DELAY(pt,1);

		//-----------------предварительная настройка----------
		 spi_buf[1]=indicator_decode[current_indicator];//устанавливаем decode mode
		 spi_buf[4]=indicator_scan[current_indicator];//установим scan_limit
		 spi_buf[3]=indicator_brightness[current_indicator];//установим яркость

		//----------------------------------------------------
			integer_num=fabs(indicator_buf[current_indicator]);	  //берем абсолютное
			PT_YIELD(pt);
			
			num_conv_counter=0;
		//------------------int to bcd------------------------
		if(indicator_decode[current_indicator]==0x9FF)	 //если декодирование включено
		{
			while(num_conv_counter<=(indicator_scan[current_indicator]&0xF)) //переводим в bcd
			{
				spi_buf[num_conv_counter+5]=integer_num%10|(0x100*(((indicator_scan[current_indicator]&0xF)+1)-num_conv_counter));
				integer_num=integer_num/10;	
				num_conv_counter++;
				PT_YIELD(pt);
			}
				//-----------------------------поставим точку-----------------------------------
//							
			if(indicator_point[current_indicator])
			{
				spi_buf[5+indicator_point[current_indicator]-1]|=	0x80;  //
				
				for(i=/*((unsigned char)(spi_buf[4]&0x7))*/4;i>((unsigned char)(spi_buf[4]&0x7)+indicator_point[current_indicator]-5);i--)
				{
				   if((spi_buf[5+i]&0xf)==0x0)
				   {
				   		spi_buf[5+i]|=0xf;	
				   }
				   else
				   {
				   		break;
				   }
					
				}

			}
			//----------------------------поставим минус-------------------------------------			
			if(indicator_buf[current_indicator]<0)  //по возможности автоматизировать подстройку debug
			{
				spi_buf[9]=(spi_buf[9]&0xFFF0)|0xA;//ставим минус, если число отрицательное
			}
//				//-------------------------------------------------------------------------------
//				if(indicator_blink[current_indicator])
//				{
//					for(i=0;i<(indicator_scan[current_indicator]+1);i++)
//					{
//						 spi_buf[5+i]&=0xff00;
//						 spi_buf[5+i]|=0x000f;
//					}
//				}
		}
		else//SET BARGRAPH
		{
			 	memcpy(&spi_buf[5],&init_buf,8*sizeof(unsigned int));//обнулим буфер
				PT_YIELD(pt);

				if(led_bar.point_1>=0)
				{
					spi_buf[(led_bar.point_1/4)+5]|=((led_bar.point_1/4+1)<<8)|LED_BAR_STAMP_ORANGE[led_bar.point_1%4];
				}

				if(led_bar.point_2>=0)
				{				
					spi_buf[(led_bar.point_2/4)+5]|=((led_bar.point_2/4+1)<<8)|LED_BAR_STAMP_RED[led_bar.point_2%4];
				}
				PT_YIELD(pt);
		
			
				for(i=0;i<led_bar.val;i++)
				{
					PT_YIELD(pt);
					if(i<led_bar.point_1)
					{
						spi_buf[(i/4)+5]|=((i/4+1)<<8)|LED_BAR_STAMP_GREEN[i%4];
					}
					else
					{
						if(i<led_bar.point_2)
						{
							spi_buf[(i/4)+5]|=((i/4+1)<<8)|LED_BAR_STAMP_ORANGE[i%4];
						}
						else
						{
							spi_buf[(i/4)+5]|=((i/4+1)<<8)|LED_BAR_STAMP_RED[i%4];	
						}
					}	
				}
		}


//				//-------------------------------------------------------------------------------			
				spi_buf_counter=0;
				spi_data=spi_buf[spi_buf_counter];
				spi_buf_counter++;
				spi_buf_length=5+(indicator_scan[current_indicator]&0xF);
			
				switch(current_indicator)
				{
					case INDICATOR_1:
					{
						CS_0;
					}
					break;

					case INDICATOR_2:
					{
						CS_10;
					}
					break;

					case INDICATOR_3:
					{
					//	CS_3;
					}
					break;

					case INDICATOR_4:
					{
					//	CS_4;
					}
					break;
				}

				
			//--------------передача--------------------------------------
			while(spi_buf_counter<(spi_buf_length+2))
			{
				for(spi_bit_counter=0;spi_bit_counter<16;spi_bit_counter++)
				{
					if((spi_data & 0x8000))
					{
						WR_DATA_1;
						WR_DATA_11;
					}
					else
					{
						WR_DATA_0;
						WR_DATA_10;
					}
					
					CLK_1;
					CLK_11;
					spi_data=spi_data<<1;//
						
					CLK_0;
					CLK_10;
				}
				PT_YIELD(pt);
			
				switch(current_indicator)
				{
					case INDICATOR_1:
					{
						CS_1;
					}
					break;

					case INDICATOR_2:
					{
						CS_11;//	CS_2=1;
					}
					break;

					case INDICATOR_3:
					{
					//	CS_3=1;
					}
					break;

					case INDICATOR_4:
					{
					//	CS_4=1;
					}
					break;
				}


				PT_YIELD(pt);

				switch(current_indicator)
				{
					case INDICATOR_1:
					{
						CS_0;
					}
					break;

					case INDICATOR_2:
					{
						CS_10;
					}
					break;

					case INDICATOR_3:
					{
					//	CS_3=0;
					}
					break;

					case INDICATOR_4:
					{
					//	CS_4=0;
					}
					break;
				}

				spi_data=spi_buf[spi_buf_counter];
				spi_buf_counter++;
				//spi_bit_counter=0;	
			}
			//--------отослали-сбросим поток
			current_indicator++;
			current_indicator&=0x3;
			PT_RESTART(pt);
			//----------------------------------------------------
  }
   PT_END(pt);

 }
//----------------------------------------------

////--------------------------------------------------------------------------------
void LED_Out_Float(unsigned char display_num,float num)//вывод значения с плавающей точкой  //не реализовано
{
 	unsigned int pwr=1;
	unsigned char j;

	for(j=0;j<(indicator_point[display_num]-1);j++) //возводим 10 в степень положения точки
	{
		pwr=pwr*10;
	}
	indicator_buf[display_num]=(long)(num*pwr);

	return;
}
//--------------------------------------------------------------------------------
//void LED_Out_Integer(unsigned long num)//вывод целочисленного значения
//{
////	if(spi_auto==SPI_WAIT)
////	{
//		integer_num=num;//...
//		//spi_auto=SPI_INT_TO_BCD;
//		LED_START=1;//стартуем поток
//
//		if(flag_blink==0)
//		{
//			LED_Set_Brightness(skd.brightness>>4);
//		}
//		else
//		{
//			LED_Set_Brightness(0xf);
//		}	
////	}
//	return;
//}
//--------------------------------------------------------------------------------
//void LED_Out_Buf(unsigned int out_buf[], unsigned char out_buf_len)//вывод буфера в SPI bit-bang
//{
//	unsigned char i;
//
////	if(spi_auto!=SPI_WAIT)
////	{
//	//	return;
//	//}
//	//скопируем в свой буфер
//	for(i=0;i<(out_buf_len&0xF);i++)
//	{
//		spi_buf[i]=out_buf[i];	
//	}
//	
//	spi_buf_counter=0;
//	spi_data=spi_buf[spi_buf_counter];
//	spi_buf_counter++;
//	spi_buf_length=out_buf_len;
//	CS=0;
////	spi_auto=SPI_SEND_WORD;
////	sw_auto=0;
//	return;	
//}
//--------------------------------------------------------------------------------
void LED_Set_Digit_Num(unsigned char display_num,unsigned char num)//установить количество отображаемых чисел
{
	//(spi_buf[4]&0xfff0)|((num-1)&0x7);
	
	indicator_scan[display_num]=(num-1)&0x7;
	return;	
}
//--------------------------------------------------------------------------------
void LED_Set_Brightness(unsigned char display_num,unsigned char bright)//установить яркость индикаторов
{
	//spi_buf[3]=((unsigned int)0xA<<8)|(bright&0xf);
	
	indicator_brightness[display_num]=((unsigned int)0xA<<8)|(bright&0xf);
	return;	
}
//---------------------------------------------------------------------------------
void LED_Set_Decode(unsigned char display_num,unsigned char decode)//установить яркость индикаторов
{
	//spi_buf[3]=((unsigned int)0xA<<8)|(bright&0xf);
	
	indicator_decode[display_num]=((unsigned int)0x9<<8)|(decode&0xf);
	return;	
}
//--------------------------------------------------------------------------------
void LED_SetPoint(unsigned char display_num,unsigned char pnt)//на какое знакоместо ставим точку, начиная слева
{
	//indicator_point[current_indicator]=(8-2)-pnt;
	indicator_point[display_num]=pnt;//(8-2)-pnt;
	return;
}
//--------------------------------------------------------------------------------
void LED_SetBlink(unsigned char display_num,unsigned char blink)//мигание
{
	indicator_blink[display_num]=blink;//
	return;
}
//----------------------------------------------------------------------------------

PT_THREAD(LED_BlinkTask(struct pt *pt))//процесс мигания
 {
 	unsigned char i=0;
  PT_BEGIN(pt);

    while(1) 
    {
	   PT_DELAY(pt,100);
	   for(i=0;i<INDICATOR_NUM;i++)
	   {
	   		indicator_blink[i]=!indicator_blink[i];	
	   }
	}
  PT_END(pt);
 }
 //----------------------------------------------------------------------------------
 void SetBarGraph(unsigned char val,unsigned char point_1,unsigned char point_2, unsigned char invert)
 {
 	 // unsigned char i=0;
	led_bar.val=val;
	led_bar.point_1=point_1-1;
	led_bar.point_2=point_2-1;

	if(led_bar.val>32)
	{
		led_bar.val=32;
	}

	if(point_1==0)
	{
		led_bar.point_1=-1;	
	}

	if(point_2==0)
	{
		led_bar.point_2=-1;	
	}

	if(point_1>32)
	{
		led_bar.point_1=33;	
	}

	if(point_2>32)
	{
		led_bar.point_2=33;	
	}


 }
 //----------------------------------------------------------------------------------
