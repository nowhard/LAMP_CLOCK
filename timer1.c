#include "timer1.h"
#define F_CPU 8000000

//--------------------------------------------------
void Timer1_Init(void)//инициализация timer1
{
	

	TCNT1H=0xFF;
	TCNT1L=0x01;

	TIFR1&=!(1<<ICF1);

	TCCR1B|=((0<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=FCLK

	TIMSK1|=(1<<OCIE1A);//compare interrupt enable

	OCR1AH=0xFF;
	OCR1AL=0x10;

	PORTB&=!(1<<PB6);
	PORTB&=!(1<<PB7);

	TIMSK1|=(1<<TOIE1);//overflow interrupt enable
	return;
}
//----------------------------------------------------
ISR(TIMER1_OVF_vect) //обработчик прерывания таймера0 
{
	TCNT1H=0xFF;
	TCNT1L=0x1;
	PORTB|=(1<<PB6);
	TIFR1&=!(1<<TOV1);
return;
}
//---------------------------------------------------
ISR (TIMER1_COMPA_vect)
{
   PORTB&=!(1<<PB6);
   TIFR1&=!(1<<OCF1A);
   return;
}
//--------------------------------------------------
