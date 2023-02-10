#ifndef _LEDarray_H
#define _LEDarray_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//function prototypes
void LEDarray_init(void);
void button_init(void);
void LEDarray_disp_bin(unsigned int number);
void delay_ms(unsigned int milliseconds);
void LEDarray_disp_dec(unsigned int number);
void power_meter(unsigned int val);
unsigned int maximum(unsigned int num1, unsigned int num2);
void LEDarray_disp_PPM(unsigned int val, unsigned int max);


#endif