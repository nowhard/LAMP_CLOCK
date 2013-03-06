#include "timer1.h"
#define F_CPU 8000000

volatile unsigned char brightness=0x9;

//--------------------------------------------------
void Timer1_Init(void)//инициализация timer1
{	
	brightness=0x9;
	TCNT1H=0xFF;
	TCNT1L=0x0;

//	TIFR1&=!(1<<ICF1);

	



	OCR1AH=0xFF;
	OCR1AL=0xB*brightness+5;

	PORTB&=!(1<<PB6);
	PORTB&=!(1<<PB7);

	TIMSK1|=(1<<OCIE1A);//compare interrupt enable]
	TCCR1B|=((0<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=FCLK
//	TCCR1B|=((1<<CS12)|(0<<CS11)|(1<<CS10));//FTMR_CLK=FCLK
	TIMSK1|=(1<<TOIE1);//overflow interrupt enable
	return;
}
//----------------------------------------------------
ISR(TIMER1_OVF_vect) //обработчик прерывания таймера0 
{
	TCNT1H=0xFF;
	TCNT1L=0x0;
	PORTB|=0b01000000;//PORTB|=(1<<PB6);
	TIFR1&=0b11111110;//TIFR1&=!(1<<TOV1);
}
//---------------------------------------------------
ISR (TIMER1_COMPA_vect)
{
   PORTB&=0b10111111;//PORTB&=!(1<<PB6);
   TIFR1&=0b11111101;//TIFR1&=!(1<<OCF1A);
}
//--------------------------------------------------
