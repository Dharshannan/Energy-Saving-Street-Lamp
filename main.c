#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "LEDarray.h"
#include "ADC.h"
#include "interrupts.h"
#include "timers.h"
#include "datetime.h"
#include "LCD.h"
#include "LEDconfig.h"
#include "comparator.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) 
{   date_time curr; // We will call our structure variable curr short for current
    // Initiate date and time variables
    curr.year = 2023;
    curr.month = 10;
    curr.date = 26;
    curr.day = 6;
    curr.hour = 16;
    curr.minute = 0;
    curr.second = 0;
    curr.daysave = 1;
    curr.sunset_hour = 17;
    curr.sunset_minute = 0;
    curr.sunset_second = 0;
    curr.sunrise_hour = 7;
    curr.sunrise_minute = 0;
    curr.sunrise_second = 0;
    
    // Initialize functions
    LCD_init(curr); // Initialize LCD
    LED_init(); // Initialize LED as lamp
    LEDarray_init(); // Initialize LED array
    ADC_init(); // Initiate the ADC converter for pin RA3
    Comp1_init(); // Enable Comparator
    Timer0_init(); // Initiate timer
    Interrupts_init(curr); // Initiate interrupt

    while(1) {
        // Configure LED (street lamp)
        configure_LED_lamp(curr);
        // Increment time
        curr = incre_time2(curr);
        // Update time based on sun-synchronocity
        curr = set_sunsynchronocity(curr);
    }
}
