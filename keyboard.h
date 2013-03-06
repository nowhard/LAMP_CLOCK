#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#include "pt/pt.h"

#define KEY_UP				1
#define KEY_DOWN			2
#define KEY_RIGHT			3
#define KEY_LEFT			4
#define KEY_OK				5
#define KEY_GND				6

void  KBD_init();
void readKey();
PT_THREAD(Keyboard_Process(struct pt *pt));
#endif
