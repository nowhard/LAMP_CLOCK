

#include "protocol.h"

#define VREF 3.486

//extern struct PID pid[PID_CHANNELS];
volatile //extern unsigned int counter;
volatile //extern unsigned int out_counter;
volatile //unsigned int freqency;
volatile //unsigned char flag=0;
volatile unsigned char transf_count=0x1;
//extern unsigned char led_brightness;

  unsigned char  Crc8Table[256]PROGMEM ={0x00, 0xBC, 0x01, 0xBD, 0x02, 0xBE, 0x03, 0xBF, 
 									 0x04, 0xB8, 0x05, 0xB9, 0x06, 0xBA, 0x07, 0xBB, 
									 0x08, 0xB4, 0x09, 0xB5, 0x0A, 0xB6, 0x0B, 0xB7, 
									 0x0C, 0xB0, 0x0D, 0xB1, 0x0E, 0xB2, 0x0F, 0xB3, 
									 0x10, 0xAC, 0x11, 0xAD, 0x12, 0xAE, 0x13, 0xAF, 
									 0x14, 0xA8, 0x15, 0xA9, 0x16, 0xAA, 0x17, 0xAB, 
									 0x18, 0xA4, 0x19, 0xA5, 0x1A, 0xA6, 0x1B, 0xA7, 
									 0x1C, 0xA0, 0x1D, 0xA1, 0x1E, 0xA2, 0x1F, 0xA3, 
									 0x20, 0x9C, 0x21, 0x9D, 0x22, 0x9E, 0x23, 0x9F, 
									 0x24, 0x98, 0x25, 0x99, 0x26, 0x9A, 0x27, 0x9B, 
									 0x28, 0x94, 0x29, 0x95, 0x2A, 0x96, 0x2B, 0x97, 
									 0x2C, 0x90, 0x2D, 0x91, 0x2E, 0x92, 0x2F, 0x93, 
									 0x30, 0x8C, 0x31, 0x8D, 0x32, 0x8E, 0x33, 0x8F, 
									 0x34, 0x88, 0x35, 0x89, 0x36, 0x8A, 0x37, 0x8B, 
									 0x38, 0x84, 0x39, 0x85, 0x3A, 0x86, 0x3B, 0x87, 
									 0x3C, 0x80, 0x3D, 0x81, 0x3E, 0x82, 0x3F, 0x83, 
									 0x40, 0xFC, 0x41, 0xFD, 0x42, 0xFE, 0x43, 0xFF, 
									 0x44, 0xF8, 0x45, 0xF9, 0x46, 0xFA, 0x47, 0xFB, 
									 0x48, 0xF4, 0x49, 0xF5, 0x4A, 0xF6, 0x4B, 0xF7, 
									 0x4C, 0xF0, 0x4D, 0xF1, 0x4E, 0xF2, 0x4F, 0xF3, 
									 0x50, 0xEC, 0x51, 0xED, 0x52, 0xEE, 0x53, 0xEF, 
									 0x54, 0xE8, 0x55, 0xE9, 0x56, 0xEA, 0x57, 0xEB, 
									 0x58, 0xE4, 0x59, 0xE5, 0x5A, 0xE6, 0x5B, 0xE7, 
									 0x5C, 0xE0, 0x5D, 0xE1, 0x5E, 0xE2, 0x5F, 0xE3, 
									 0x60, 0xDC, 0x61, 0xDD, 0x62, 0xDE, 0x63, 0xDF, 
									 0x64, 0xD8, 0x65, 0xD9, 0x66, 0xDA, 0x67, 0xDB, 
									 0x68, 0xD4, 0x69, 0xD5, 0x6A, 0xD6, 0x6B, 0xD7, 
									 0x6C, 0xD0, 0x6D, 0xD1, 0x6E, 0xD2, 0x6F, 0xD3, 
									 0x70, 0xCC, 0x71, 0xCD, 0x72, 0xCE, 0x73, 0xCF, 
									 0x74, 0xC8, 0x75, 0xC9, 0x76, 0xCA, 0x77, 0xCB, 
									 0x78, 0xC4, 0x79, 0xC5, 0x7A, 0xC6, 0x7B, 0xC7, 
									 0x7C, 0xC0, 0x7D, 0xC1, 0x7E, 0xC2, 0x7F, 0xC3};
//-----------------------------------------------
#define INDICATORS	20//
#define FRAME_LEN	134

#define UST_1	0x0
#define	UST_2	0x1F

#define UST_FORM1 0
#define UST_FORM2 1

#define HYST_MAX 32

#define BLINKING	12  

#define MAX_MESS_FREQ 16
//---------------------------------------------------------
 //unsigned int sec_counter=0;
 unsigned char  hyst_counter=0,point_counter_3=1,point_counter_4=1,point_counter_5=1;
 unsigned char num=0x30;//'0'
const unsigned char  indicator_list[20]={3,3,3,3,3,3,3,3,5,0,4,3,4,32,32,32,32,0,32,32};	 //?????? ? ?????????????????? ???????????
unsigned char hyst_napr=0;//



volatile unsigned char  brightness=0xF;//
//unsigned char  freq=100;//


volatile unsigned char  hyst_blink_counter=0;//
volatile unsigned char  ust_form=0;//

//unsigned char  button_mask=0;//

unsigned char  index=0, index_buf=0;


unsigned char  proto_type=0x1;//

unsigned char beep_type=0x0;//

//unsigned char transfer_count=0;

unsigned char   buffer[0xFF]={0};

//unsigned char proto_cycle=0;

//unsigned char cycle_timing=1;
//unsigned char cycle_count=0;



unsigned char  CRC_Check( unsigned char *Spool,unsigned int Count ) ;

//-----------------------------------------------

//-----------------------------------------------
//void USART0_Transmit(unsigned char data)
//{
//	while (!(UCSRA & (1<<UDRE)));
//	UDR = data;
//}

//-----------------------------------------------
/*void Sec_Task()
{
	

	cycle_timing=(ReadADC(4)>>7)+1;
	LED_Out_Integer(cycle_timing);
	if(cycle_timing<=cycle_count)
	{
		led_brightness=brightness=ReadADC(3)>>6;
		
		Protocol_Handler();
		
		cycle_count=0;
	}
	setTimer(SEC_TASK, 0,150);
	cycle_count++;
	return;
}*/


//------------------------------------------------
void Protocol_Handler(void)//
{
	unsigned char i=0,j=0;
//	unsigned char buf[7];	

//	if(!proto_cycle)
//		return;	

//	proto_cycle=0; 
	index=0;

	if(proto_type==0x1)
	{
		index+=9;//
		buffer[0]=0x0;
		buffer[1]=0xD7;
		buffer[2]=0x29;
		buffer[3]=0xF;
		buffer[4]=0x9;
		//buffer[5]= 
		buffer[6]=0x0;
		buffer[7]=0x8F;
		//buffer[8]=
	}
						



	buffer[index]=':';
	index++;
	index++;//

	for(i=0;i<INDICATORS;i++)
	{			
		switch(indicator_list[i])
		{
			case 3:
			{			
					buffer[index]='[';
					index++;					

					buffer[index]=i;
					index++;

					for(j=0;j<=indicator_list[i];j++)
					{
							if(j==point_counter_3)
							{
								//buf[j]=0x2E;//'.'
								buffer[index]=0x2E;	
							}
							else
							{
								//buf[j]=num;
								buffer[index]=num;
							}
							index++;
					}

					buffer[index]=']';
					index++;	
			}
			break;

			case 4:
			{
					buffer[index]='[';
					index++;

					buffer[index]=i;
					index++;

					for(j=0;j<=indicator_list[i];j++)
					{
							if(j==point_counter_4)
							{
								//buf[j]=0x2E;//'.'
								buffer[index]=0x2E;	
							}
							else
							{
								//buf[j]=num;
								buffer[index]=num;
							}
							index++;
					}
					
					buffer[index]=']';
					index++;
			}
			break;

			case 5:
			{
					buffer[index]='[';
					index++;					

					buffer[index]=i;
					index++;

					for(j=0;j<=indicator_list[i];j++)
					{
							if(j==point_counter_5)
							{
								buffer[index]=0x2E;	
							}
							else
							{
								buffer[index]=num;
							}
							index++;
					}
					
					buffer[index]=']';
					index++;		 
			}
			break;

			case 32:
			{
					buffer[index]='[';
					index++;
					
					if(i!=INDICATORS-1)
					{
						buffer[index]=i;
						index++;

						buffer[index]=ust_form;
						index++;

						buffer[index]=hyst_counter;
						index++;

						buffer[index]=UST_1;
						index++;

						buffer[index]=UST_2;
						index++;
					}
					else
					{
						buffer[index]=i;
						index++;

						buffer[index]=ust_form;
						index++;

						buffer[index]=hyst_counter;
						index++;

						buffer[index]=UST_1;
						index++;

						buffer[index]=UST_2-1;
						index++;
					}	
				  	
					buffer[index]=']';
					index++;
			}
			break;	

			default:
			{
			}
			break;
		}
	}
//-------------------------------	
//	 index+=sprintf(&buffer[index],"[*%c]",brightness);

	buffer[index]='[';
	index++;

	buffer[index]='*';
	index++;

	buffer[index]=brightness;
	index++;

	buffer[index]=']';
	index++;
//------------------------------
	if(proto_type==0x1)
	{	
//		index+=sprintf(&buffer[index],"\x1\x41%c",beep_type);

		buffer[index]=0x1;
		index++;
		
		buffer[index]=0x41;
		index++;
		
		buffer[index]=beep_type;
		index++;

//------------------------------

		buffer[8]=index-12;
		buffer[5]=index-5;//
		buffer[10]=index-14;
		buffer[index]=CRC_Check(&buffer[1],index-1);
		index++;	
	}
	else
	{
		buffer[1]=index-2;	 //???
	}  

	     index_buf=index;

		 UDR0 = buffer[0];
//организовать передачу буфера

	 point_counter_3++;
	 point_counter_4++;
	 point_counter_5++;
	
	 if(point_counter_3>3)
			point_counter_3=1;

	 if(point_counter_4>4)
			point_counter_4=1;

	 if(point_counter_5>5)
			point_counter_5=1;

	 num++;
	 if(num>0x39)//9->0
	 	num=0x30;


  if(hyst_blink_counter==0)//
  {
	if(!hyst_napr)
		hyst_counter++;
	else
		hyst_counter--;

	if(hyst_counter>=HYST_MAX)
	{
		hyst_napr=1;
		hyst_blink_counter=BLINKING;
	}
	if(hyst_counter==0)
	{
		hyst_napr=0;
	}
 }
 else
 {
 	hyst_blink_counter--;
	ust_form=~ust_form;//
 }
   
 return;		
}
//---------------------------------------------------------
unsigned char  CRC_Check( unsigned char *Spool,unsigned int Count ) 
{	
     unsigned char crc = 0x0;

     while (Count--)
         crc = pgm_read_byte(&(Crc8Table[crc ^ *Spool++]));

     return crc;
}
//-------------------------------------------------------------
ISR(USART_TX_vect)//
{
	if(transf_count<index_buf)
	{
		UDR0=buffer[transf_count];
		transf_count++;
	}
	else
	{
		transf_count=0x1;
		index_buf=0;		
	}	
}
