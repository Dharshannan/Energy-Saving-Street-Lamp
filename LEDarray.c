#include <xc.h>
#include "LEDarray.h"

void LEDarray_init(void) {
    // LAT bits
    LATA = 0; // Assign LAT before TRIS
    LATB = 0;
    LATF = 0;
    LATG = 0;
    // TRIS bits
    TRISA = 0; //set up TRIS registers for pins connected to LED array
	TRISB = 0;
    TRISFbits.TRISF0 = 0; // Set F0 and F6 separately as we need to use the RF2 switch
    TRISFbits.TRISF6 = 0;
    TRISG = 0;
}

void button_init(void) {
    TRISFbits.TRISF2 = 1; //set the switch RF2 to an input
    ANSELFbits.ANSELF2 = 0; //turn off analogue 
}

void LEDarray_disp_bin(unsigned int number)
{
    //code to turn on/off the pins connected to the LED array
	if (number & 1)        {LATGbits.LATG0 = 1;} else {LATGbits.LATG0 = 0;} //test bit 0 against bit mask and set LED
    if (number & (1 << 1)) {LATGbits.LATG1 = 1;} else {LATGbits.LATG1 = 0;} //test bit 1
    if (number & (1 << 2)) {LATAbits.LATA2 = 1;} else {LATAbits.LATA2 = 0;} //...
    if (number & (1 << 3)) {LATFbits.LATF6 = 1;} else {LATFbits.LATF6 = 0;}
    if (number & (1 << 4)) {LATAbits.LATA4 = 1;} else {LATAbits.LATA4 = 0;}
    if (number & (1 << 5)) {LATAbits.LATA5 = 1;} else {LATAbits.LATA5 = 0;}
    if (number & (1 << 6)) {LATFbits.LATF0 = 1;} else {LATFbits.LATF0 = 0;}
    if (number & (1 << 7)) {LATBbits.LATB0 = 1;} else {LATBbits.LATB0 = 0;}
    if (number & (1 << 8)) {LATBbits.LATB1 = 1;} else {LATBbits.LATB1 = 0;} //test bit 8
}

void LEDarray_disp_dec(unsigned int number) {
    
    unsigned int dec_value, i; // dec_value assigned the value of number compared to i
    for (i=0;i<=520;i+=10) { // use for loop to check stop at 520 (limit)
        if (number%i == 0 && i!= 0) { // If the number is a factor of 10 and i!=0
            dec_value = 1 << ((i-10)/10); // left shift to light up LED in sequence 
            LEDarray_disp_bin(dec_value); // call previous function to light up LED
        }
    }
}

 void delay_ms(unsigned int milliseconds) // delay function own defined to enable variable delay input
 {
   while(milliseconds > 0)
   {
       __delay_ms(1);
      milliseconds--;
    }
 }

// Now we will define a function to turn on the LED like a power meter
 void power_meter(unsigned int val){ // similar to dec function above but slightly different
     
    unsigned int dec_value, i; // dec_value assigned the value of number compared to i
    for (i=0;i<=520;i+=10) { // use for loop to check stop at 520 (limit)
        unsigned int temp1, temp2, val2; // use some temporary values to change val into a multiple of 10
        temp1 = val;
        temp2 = temp1/10; // divide 10 to round off
        val2 = temp2*10; // multiply rounded int by ten
        if (val2%i == 0 && i!= 0 && val2!=0) { // If the number is a factor of 10 and i!=0
            dec_value = 1 << ((i-10)/10); // left shift to light up LED in sequence 
            unsigned int j;
            for (j=0;j<((i-10)/10);j++) { // add the values below (i-10)/10 to light up the LED array like a meter
                dec_value += 1 << j;
            }
            LEDarray_disp_bin(dec_value); // call previous function to light up LED
        }
        if (val2==0) { // to not light up LED if very low light
            dec_value = 0;
            LEDarray_disp_bin(dec_value);
        }
    }
 }
 
 /* Exercise 3 (8), NOTE FOR SELF LATER, can possibly be achieved using a 
  * while loop and delay function within the loop */
 
 unsigned int maximum(unsigned int num1, unsigned int num2){ // function to return maximum of 2 numbers
     if (num1>num2){
         return num1;
     }
     else {return num2;}
 }
 
 void LEDarray_disp_PPM(unsigned int val, unsigned int max){ // similar to power_meter and LEDarray_disp_dec (combined) specifically for the last task 8
    // This section takes care of the max value
     
    unsigned int dec_value1, j; // dec_value assigned the value of number compared to i
    for (j=0;j<=520;j+=10) { // use for loop to check stop at 520 (limit)
        if (max%j == 0 && j!= 0) { // If the number is a factor of 10 and j!=0
            dec_value1 = 1 << ((j-10)/10); // left shift to light up LED in sequence 
        
        }
    }
    
    // This section takes care of the current value
     
    unsigned int dec_value2, i; // dec_value assigned the value of number compared to i
    for (i=0;i<=520;i+=10) { // use for loop to check stop at 520 (limit)
        unsigned int temp1, temp2, val2; // use some temporary values to change val into a multiple of 10
        temp1 = val;
        temp2 = temp1/10; // divide 10 to round off
        val2 = temp2*10; // multiply rounded int by ten
        if (val2%i == 0 && i!= 0 && val2!=0) { // If the number is a factor of 10 and i!=0
            dec_value2 = 1 << ((i-10)/10); // left shift to light up LED in sequence 
            unsigned int k;
            for (k=0;k<((i-10)/10);k++) { // add the values below (i-10)/10 to light up the LED array like a meter
                dec_value2 += 1 << k;
            }
            LEDarray_disp_bin(dec_value1 | dec_value2); // call previous function to light up LED (use bit-wise OR operator to add signals of max and current values)
        }
        if (val2==0) { // to not light up LED if very low light
            dec_value2 = 0;
            LEDarray_disp_bin(dec_value1 | dec_value2); // As above
        }
    }
 }