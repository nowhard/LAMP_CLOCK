#ifndef CORE_H
#define CORE_H
#include<avr/io.h>               // SFR declarations
#include <avr/pgmspace.h>
#include<avr/interrupt.h>

#define maxHandlers 8
#define maxMessages 16
#define maxTimers  8

#define SEC_TASK    		1
#define REG_TASK    		2
#define	OUT_TASK			3
#define	MSG_KEY_PRESS 		4
#define	MSG_KEY_SCAN 		5
#define MSG_DISP_REFRESH  	6
#define MSG_MENU_SELECT		7

typedef unsigned char msg_num; // тип сообытия - мне пока хватает одного байта
typedef int msg_par; // тип параметра события
typedef unsigned char (*handler)(msg_par); // описание функции-обработчика
//---------прототипы-------------------------------------------------------
void setHandler(msg_num msg, handler hnd);
void killHandler(msg_num msg, handler hnd);
void sendMessage(msg_num msg, msg_par par);
void dispatchMessage(void);
void setTimer(msg_num msg, msg_par par, unsigned int time);
void killTimer(msg_num msg);
void dispatchTimer(void);
ISR(TIMER0_OVF_vect);

#endif
