#include <xc.h>
#include "ADC.h"
#include "datetime.h"

// We will now configure the LED RD7 as our street lamp

void LED_init(void){ // Initialize the LED
    LATDbits.LATD7 = 0; // Set LAT register
    LATHbits.LATH3 = 0; // Configure LED2 to debug comparator flag
    TRISDbits.TRISD7 = 0; // Set TRIS register
    TRISHbits.TRISH3 = 0; // Configure LED2 to debug comparator flag
}

void configure_LED_lamp(date_time curr){
    unsigned char datum; // Initialize datum variable
    datum = 40; // Set arbitraty datum value (40 for now)
    if(ADC_getval()<datum && (curr.hour<1 || curr.hour>=5)){
        LATDbits.LATD7 = 1; // Turn ON LED if light level falls below datum but OFF between 1am and 5am
    }
    else LATDbits.LATD7 = 0;
    
    /*
    if(ADC_getval()<40){ // If time has passed sunset but before sunrise (only set comp to detect sunrise)
        CM1CON1bits.INTP=1; 	//set interrupt flag on positive going edge (light to dark 'sunset')
        CM1CON1bits.INTN=1;     //set interrupt flag on negative going edge (dark to light 'sunrise')
    }
    if(ADC_getval()>=40){ // If time has passed sunrise but before sunset (only set comp to detect sunset)
        CM1CON1bits.INTP=1; 	//set interrupt flag on positive going edge (light to dark 'sunset')
        CM1CON1bits.INTN=1;     //set interrupt flag on negative going edge (dark to light 'sunrise')
    }
    */
    if(curr.hour>=1 && curr.hour<5){ // Turn OFF C1 comparator interrupt between 1am and 5am
        PIE2bits.C1IE = 0;
    }
    else{
        PIE2bits.C1IE = 1;
    }
}