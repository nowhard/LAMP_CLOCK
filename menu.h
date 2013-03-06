#ifndef _MENU_H_
#define _MENU_H_
#include <ctype.h>
enum {
    MENU_CANCEL=1,
    MENU_TIME,
    MENU_DATE,
    MENU_YEAR,
    MENU_TUNE_TIME,
    MENU_TUNE_DATE,
    MENU_TUNE_YEAR,
    MENU_TUNE_BRIGHTNESS,
    MENU_TEMPERATURE_1,
	MENU_FREQUENCY,
	MENU_VOLTAGE
};

unsigned char startMenu();
void initMenu();

#endif
