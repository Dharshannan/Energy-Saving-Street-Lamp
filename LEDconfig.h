#ifndef _LEDconfig_H
#define	_LEDconfig_H

#include <xc.h> // include processor files - each processor file is guarded. 

#define _XTAL_FREQ 64000000

void LED_init(void);
void configure_LED_lamp(date_time curr);

#endif

