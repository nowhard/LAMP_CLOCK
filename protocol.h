#ifndef PROTOCOL_H
#define PROTOCOL_H

#include<avr/interrupt.h>


#include <ctype.h>
#include <stdint.h>
//#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#include "adc.h"
#include "led_process.h"

void Protocol_Handler(void);
#endif
