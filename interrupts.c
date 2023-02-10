#include <xc.h>
#include "interrupts.h"
#include "ADC.h"
#include "datetime.h"
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(date_time curr)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    
    // Initialize C1 comparator interrupt
    
    if(curr.hour<1 || curr.hour>=5){ // Turn on comparator outside of 1am to 5am range
        PIE2bits.C1IE = 1;
    }
    else PIE2bits.C1IE = 0;
    
    IPR2bits.C1IP = 1; // High priority
    
    // Initialize TMR0 interrupts
    PIE0bits.TMR0IE=1; 	//enable interrupt source TMR0
    IPR0bits.TMR0IP = 0; // Low priority
    
    // Other interrupts
    INTCONbits.IPEN = 1; // Enable interrupt priority
    INTCONbits.PEIE = 1; // peripheral interrupts
    INTCONbits.GIE=1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)
}

// Here is where we define sunset and sunrise flags to keep track of sunset and sunrise occurrences 
unsigned char sunrise_flag = 0;
unsigned char sunset_flag = 0;

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{   
    if(PIR2bits.C1IF){
        __delay_ms(50); // Delay to allow for ADC conversion, dunno why but without the delay its buggy
        unsigned char val = ADC_getval();
        //LATHbits.LATH3 = !LATHbits.LATH3; // Toggle LED 2 to debug
        if(val<40){ // If LED is turned ON (sunset has occurred)
            sunset_flag = 1;
        }

        if(val>=40){ // If LED is turned OFF (sunrise has occurred)
            sunrise_flag = 1;
        }
        
        PIR2bits.C1IF = 0; // Clear interrupt flag
    }
}

unsigned char tmr_ovf = 0; // Timer overflow flag
// Low priority interrupt
void __interrupt(low_priority) LowISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(PIR0bits.TMR0IF){ 	//check the interrupt source
        TMR0H = 0b00001011; // Reset timer value to 3035 (decimal) this is to accurately measure 1s intervals
        TMR0L = 0b11011011;
        tmr_ovf = 1;
        PIR0bits.TMR0IF=0; //clear the interrupt flag!     
    }
}

