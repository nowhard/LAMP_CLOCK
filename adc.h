#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include<avr/interrupt.h>

#define F_CPU 8000000UL  // 1 MHz
    //#define F_CPU 14.7456E6
#include <util/delay.h>

void INIT_ADC(void);
unsigned int ReadADC(unsigned char adc_num);
#endif
