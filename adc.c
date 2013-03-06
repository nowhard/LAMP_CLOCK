#include "adc.h"

volatile unsigned int adc_result_1,adc_result_2;
void INIT_ADC(void)
{
	ADMUX|=(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(0<<MUX0);//REF internal
	ADCSRA|=(1<<ADEN)|(1<<ADIE)|(1<<ADSC);//enable, free running cycle, interrupt enable
	ADCSRA|=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);//prescaler ADC=128
}

//-----------------------------------------------------------------------
/*unsigned int ReadADC(unsigned char adc_num)
{
		unsigned int adc_result;
//
		ADMUX&=0xF0;//11110000
		ADMUX|=adc_num;
	//	delay(100);
	_delay_us(10);
		
		ADCSRA|=(1<<ADSC);

	//	while(ADCSRA&(1<<ADSC)){}
 while ((ADCSRA&_BV(ADIF))==0x00)  ; //Дождатся окончания преобразования



		if(!(ADMUX&(1<<ADLAR)))
		{
			adc_result=ADCL;
			adc_result|=ADCH<<8;
		}
		else
		{
			adc_result=ADCL>>6;
			adc_result|=ADCH<<2;
		}

		return adc_result;
}*/
//-----------------------------------------------------------------------
ISR(ADC_vect)
{
	if(!(ADMUX&(1<<ADLAR)))
	{
		if((ADMUX&0x1)==0)
		{
			adc_result_1=ADCL;
			adc_result_1|=ADCH<<8;
			ADMUX|=0x1;
		}
		else
		{
			adc_result_2=ADCL;
			adc_result_2|=ADCH<<8;
			ADMUX&=0xFE;
		}	
	}
	else
	{
		if((ADMUX&0x1)==0)
		{
			adc_result_1=ADCL>>6;
			adc_result_1|=ADCH<<2;
			ADMUX|=0x1;
		}
		else
		{
			adc_result_2=ADCL>>6;
			adc_result_2|=ADCH<<2;
			ADMUX&=0xFE;
		}
	}
	ADCSRA|=0x40;
}
