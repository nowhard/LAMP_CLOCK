#ifndef TIMER1_H
#define TIMER1_H

#include<avr/io.h>
#include<avr/interrupt.h>

void Timer1_Init(void);//инициализация timer1
void Timer1_Run(void);//запуск timer1
void Timer1_Stop(void);//останов timer1
void Timer1_Reset(void);//сброс timer1


#endif
