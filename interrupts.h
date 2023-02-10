#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>
#include "datetime.h"

#define _XTAL_FREQ 64000000

void Interrupts_init(date_time curr);
extern unsigned char sunrise_flag;
extern unsigned char sunset_flag;
extern unsigned char tmr_ovf;
void __interrupt(low_priority) LowISR();
void __interrupt(high_priority) HighISR();

#endif
