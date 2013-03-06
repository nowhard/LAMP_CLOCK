#include "timer0.h"
//--------------------------------------------------
void Timer0_Init(void)//инициализация timer1
{
	TIMSK0|=(1<<TOIE0);//overflow interrupt enable

	TCNT0=0x82;
	TCCR0B|=((0<<CS02)|(1<<CS01)|(1<<CS00));//FTMR_CLK=FCLK
	
	return;
}
//--------------------------------------------------
/*void Timer0_Run(void)//запуск timer1
{
	TCCR0|=((0<<CS02)|(1<<CS01)|(1<<CS00));//FTMR_CLK=FCLK	
}
//--------------------------------------------------
void Timer0_Stop(void)//останов timer1
{
	TCCR0&=~((1<<CS02)|(1<<CS01)|(1<<CS00));//FTMR_CLK=0	
}
//--------------------------------------------------
void Timer0_Reset(void)//сброс timer1
{
	TCNT0=0xFE;

	TCCR0|=((0<<CS02)|(0<<CS01)|(1<<CS00));//FTMR_CLK=FCLK	
}*/
//--------------------------------------------------
