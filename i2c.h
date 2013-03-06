#ifndef I2C_H
#define I2C_H

#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "pt/pt.h"
//---------------------I2C-------------------------------
#define START 			0x08
#define	MT_SLA_ACK		0x18
#define	MT_SLA_READ_ACK	0x40
#define MT_DATA_ACK		0x28
#define	START_2			0x10
#define	READ_ACK		0x50
#define	READ_NACK		0x58

#define SLA_ADDR		0xD0
#define TMR_ADDR		0xD0
//-------------------------------------------------------  
void INIT_I2C(void);//инициализация
PT_THREAD(Write_I2C(struct pt *pt,unsigned char address,unsigned char address_reg, unsigned char data));
PT_THREAD( Read_I2C(struct pt *pt,unsigned char address,unsigned char address_reg,unsigned char *buf));
PT_THREAD(ClockInit(struct pt *pt));

PT_THREAD(ReadTime(struct pt *pt,unsigned char *led_buf));
//void ClockInit(unsigned char *clock_buf);//инициализация часов, установка
//void StoreTime(unsigned char *buf);
#endif
