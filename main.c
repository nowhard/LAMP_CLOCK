
#include<avr/interrupt.h>
#include "timer1.h"
#include "timer0.h"
#include "i2c.h"

//#include <ctype.h>
//#include <stdint.h>
//#include <stdio.h>
#include <avr/io.h>
//#include <avr/pgmspace.h>

#include <util/delay.h>

//#include <string.h>

 #include <avr/wdt.h> 
#include "pt/pt.h"
#include "keyboard.h"



volatile struct pt pt1, pt2,pt_key,pt_blink;
extern unsigned char display_mask;


void Port_Init(void);


//FILE uart_str = FDEV_SETUP_STREAM(&uart_putchar, &uart_getchar, _FDEV_SETUP_RW);
PT_THREAD(Display_Out_Process(struct pt *pt));
//PT_THREAD(Clock_Read_Process(struct pt *pt));
//unsigned char display_mas[16]={1,2,3,4,5,6};
extern unsigned char *display_mas,*i2c_mas;//={1,2,3,4,5,6};
//-----------------------------------------------
int main(void)
{

Timer0_Init();
Port_Init();
INIT_I2C();
KBD_init();
Timer1_Init();

wdt_enable(WDTO_1S);
	PT_INIT(&pt1);

sei();

/*	while(1)//init loop
	{
		unsigned char state=0;
		state=ClockInit(&pt1);
		if(state==PT_ENDED)
		{
			break;
		}
	}*/
	PT_INIT(&pt1);
    PT_INIT(&pt2);
	PT_INIT(&pt_key);
	PT_INIT(&pt_blink);
	while(1)
	{
		Display_Out_Process(&pt1);
		ReadTime(&pt2,i2c_mas);
		Keyboard_Process(&pt_key);
		Blink_Process(&pt_blink);
		wdt_reset();
	}
}
//-----------------------------------------------
void Port_Init(void)
{
	DDRB=0xFF;
	DDRD=0x2;//RX-in TX-out
	DDRD|=0x10;
	PORTD|=0x14;

	DDRC=0xF;
	PORTC=0x0;

	PORTB=0x0;
	PORTB |= _BV(PB0);
}
//-----------------------------------------------
ISR(TIMER0_OVF_vect) //обработчик прерывания таймера0 
{
cli();
	pt1.pt_time++;
	pt2.pt_time++;
	pt_key.pt_time++;
	pt_blink.pt_time++;

//	PORTB^=0x40;

	TCNT0=65;
	TIFR0&=!(1<<TOV0);
	
sei();
}
//-----------------------------------------------------------------------------
PT_THREAD(Display_Out_Process(struct pt *pt))
 {
   static unsigned char  tempb=0,tempc=0;
   static signed char count=5;
   PT_BEGIN(pt);

   while(1) 
   {
   	//	TIMSK1&=!(1<<TOIE1);//overflow interrupt enable
	//	TIMSK1&=!(1<<OCIE1A);//compare interrupt enable
		count--;
		if(count==-1)
		{
			count=5;
		}

	    tempb=PORTB;
		tempc=PORTC;

		tempb&=0xC0;
		tempb|=(1<<count);

		tempc&=0xF0;
		if(tempb&display_mask)
		{
			tempc|=display_mas[count];
		}
		else
		{
			tempc|=0xF;
		}

   		//TIMSK1&=!(1<<TOIE1);//overflow interrupt enable
		//TIMSK1&=!(1<<OCIE1A);//compare interrupt enable
		TCCR1B|=((0<<CS12)|(0<<CS11)|(0<<CS10));//FTMR_CLK=FCLK
		PORTB&=!(1<<PB6);

	    PORTB=(tempb&display_mask);
		PORTC=tempc;
	

		PT_DELAY(pt,2);

		PORTB&=0xC0;
		//TIMSK1|=(1<<TOIE1);//overflow interrupt enable
		//TIMSK1|=(1<<OCIE1A);//compare interrupt enable
		TCCR1B|=((0<<CS12)|(1<<CS11)|(1<<CS10));//FTMR_CLK=FCLK
		_delay_us(400);
//		TIMSK1|=(1<<TOIE1);//overflow interrupt enable
//		TIMSK1|=(1<<OCIE1A);//compare interrupt enable
			
   }
   PT_END(pt);
 }
//-----------------------------------------------------------------------------
