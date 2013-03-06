#include "timer1.h"
#define F_CPU 8000000
volatile unsigned char freq_flag=0;
//volatile unsigned int frequency,freq_cap=0;
volatile unsigned char mid_count;//счетчик усреднения



unsigned int mid_mas[MID_MAS_SIZE];//массив усреднения

#define MID_CNT 5
//--------------------------------------------------
void Timer1_Init(void)//инициализация timer1
{
	TIMSK1|=(1<<TOIE1);//overflow interrupt enable

	TCNT1H=0;
	TCNT1L=0;
	
	return;
}
//--------------------------------------------------
void Timer1_Run(void)//запуск timer1
{
	TCCR1B|=((1<<CS12)|(0<<CS11)|(0<<CS10));//FTMR_CLK=FCLK/256	
}
//--------------------------------------------------
void Timer1_Stop(void)//останов timer1
{
	TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=0	
}
//--------------------------------------------------
void Timer1_Reset(void)//сброс timer1
{
	TCNT1H=0;
	TCNT1L=0;
	TCCR1B|=((0<<CS12)|(0<<CS11)|(1<<CS10));//FTMR_CLK=FCLK	
}
//--------------------------------------------------
ISR(INT0_vect)
{
	if(freq_flag==0)
	{
		TCCR1B|=((1<<CS12)|(0<<CS11)|(0<<CS10));//FTMR_CLK=FCLK/256	//запускаем таймер
		TCNT1H=0;
	    TCNT1L=0;
		freq_flag=1;
	}
	else
	{
	 //   TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=0//stop timer
		freq_flag=0;			
		mid_mas[mid_count&(MID_MAS_SIZE-1)]=(unsigned int)TCNT1L+((unsigned int)TCNT1H<<8);
		
		TCNT1H=0;
		TCNT1L=0;
		mid_count++;
	}
//обработка прерывания
	return;
}
//----------------------------------------------------
ISR(TIMER1_OVF_vect) //обработчик прерывания таймера0 
{
	unsigned char i;
	TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=0//stop timer
//	frequency=0;
	freq_flag=0;
	TCNT1H=0;
	TCNT1L=0;
	TIFR1&=!(1<<TOV1);

	for(i=0;i<MID_MAS_SIZE;i++)
	{
		mid_mas[i]=0;	
	}	

	return;
}
