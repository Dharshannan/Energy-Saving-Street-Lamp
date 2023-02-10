#include <xc.h>
#include <stdio.h>
#include "LCD.h"
#include "datetime.h"

/*******************************************************************
 * LCD_E_TOG
 * Function to toggle LCD enable bit on then off
 * When this function is called, the LCD screen reads the data lines
 *******************************************************************/
void LCD_E_TOG(void) {
	LCD_E = 1;      // Turn the LCD enable bit on
	__delay_us(2);  // Wait a short delay
	LCD_E = 0;      // Turn the LCD enable bit off again
}

/********************************************************
 * LCD_sendnibble
 * Function to set the 4-bit data line levels for the LCD
 ********************************************************/
void LCD_sendnibble(unsigned char number) {
	// Set the data lines here (think back to LED array output)
    if (number & 0b0001) {LCD_DB4 = 1;} else {LCD_DB4 = 0;} // Configure 1st data bus line
    if (number & 0b0010) {LCD_DB5 = 1;} else {LCD_DB5 = 0;} // Configure 2nd data bus line
    if (number & 0b0100) {LCD_DB6 = 1;} else {LCD_DB6 = 0;} // Configure 3rd data bus line
    if (number & 0b1000) {LCD_DB7 = 1;} else {LCD_DB7 = 0;} // Configure 4th data bus line
    
    LCD_E_TOG();    // Toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);  // Wait a short delay
}

/****************************************************************************
 * LCD_sendbyte
 * Function to send full 8-bit commands/data over the 4-bit interface
 * High nibble (4 most significant bits) are sent first, then low nibble sent
 ****************************************************************************/
void LCD_sendbyte(unsigned char Byte, char type) {
    // Set RS pin whether it is a Data/Char (1) or Command (0) using type argument
    if (type) {LCD_RS = 1;} else {LCD_RS = 0;}
    
    // Send high bits of Byte using LCD_sendnibble function
    LCD_sendnibble(Byte>>4);
    
    // Send low bits of Byte using LCD_sendnibble function
    LCD_sendnibble(Byte&0b00001111);
	
    // Delay 50us (minimum for command to execute)
    __delay_us(50);
}

/*************************************************************************
 * LCD_init
 * Function to initialise the LCD after power on
 * This function also displays the initial inputted date on the LCD screen
 *************************************************************************/
void LCD_init(date_time curr) {
    // Define LCD Pins as Outputs
    TRISCbits.TRISC6 =0;
    TRISCbits.TRISC2=0;
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB2=0;
    TRISEbits.TRISE3=0;
    TRISEbits.TRISE1=0;
    
    // Set all pins low (might be random values on start up, fixes lots of issues)
    LCD_RS = 0;
    LCD_E = 0;
    LCD_DB4 = 0;
    LCD_DB5 = 0;
    LCD_DB6 = 0;
    LCD_DB7 = 0;
    
    // Initialisation sequence code
	// Follow the sequence in the LCD datasheet for 4-bit interface
	// First function set should be sent with LCD_sendnibble (the LCD is in 8-bit mode at start up)
    // After this use LCD_sendbyte to operate in 4-bit mode
    __delay_ms(45);             // Wait for more than 40ms after V_DD rises to 4.5V
    LCD_sendnibble(0b0010);     // Function set (0, 0, 1, DL:8-bit/4-bit)
    __delay_us(45);             // Wait for more than 39us
    LCD_sendbyte(0b00101000,0); // Function set (0, 0, 1, 0, N:2-line/1-line, F:5x11-dots/5x8-dots, *, *)
    __delay_us(45);             // Wait for more than 39us
    LCD_sendbyte(0b00101000,0); // Function set (0, 0, 1, 0, N:2-line/1-line, F:5x11-dots/5x8-dots, *, *)
    __delay_us(45);             // Wait for more than 37us
    
    // Turn the LCD display off
    LCD_sendbyte(0b00001000,0); // Display ON/OFF control (0, 0, 0, 0, 1, D:display-on/display-off, C:cursor-on/cursor-off, B:blinking-of-cursor-on/blinking-of-cursor-off)
    __delay_us(45);             // Wait for more than 37us
    LCD_sendbyte(0b00000001,0); // Display clear (0, 0, 0, 0, 0, 0, 0, 1)
    __delay_ms(2);              // Wait for more than 1.53ms
    LCD_sendbyte(0b00000110,0); // Entry mode set (0, 0, 0, 0, 0, 1, I/D:cursor-moving-direction-right/cursor-moving-direction-left, SH:shift-of-entire-display-on/shift-of-entire-display-off)
    __delay_us(50);             // Delay 50us (minimum for command to execute)
    
	// Remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
    LCD_sendbyte(0b00001100,0); // Display ON/OFF control (0, 0, 0, 0, 1, D:display-on/display-off, C:cursor-on/cursor-off, B:blinking-of-cursor-on/blinking-of-cursor-off)
    __delay_us(50);             // Delay 50us (minimum for command to execute)
    
    // Display the initial inputed date on the LCD screen
    char buf[40];                                                           // Define a buffer variable
    LCD_setline(1);                                                         // Set to the first line of the LCD
    sprintf(buf,"%02d-%02d-%04d",curr.date,curr.month,curr.year);  // Format the initial date as a string
    LCD_sendstring(buf);                                                    // Send the string to the LCD screen
}

/*****************************************************************************************************
 * LCD_setline
 * Function to set the cursor to beginning of line 1 or 2
 * The Display Data RAM (DDRAM) is used to store the display data represented in 8-bit character codes
 * The Character Generator RAM (CGRAM) allows the user to rewrite character by program
 * The Address Counter (AC) assigns addresses to both DDRAM and CGRAM
 *****************************************************************************************************/
void LCD_setline (char line) {
    // Send 0x80 to set line to 1 (0x00 DDRAM address)
    if (line==1) {LCD_sendbyte(0x80,0);}    // Set DDRAM address (1, AC6, AC5, AC4, AC3, AC2, AC1, AC0)
    __delay_us(50);                         // Delay 50us (minimum for command to execute)
    
    // Send 0xC0 to set line to 2 (0x40 DDRAM address)
    if (line==2) {LCD_sendbyte(0xC0,0);}    // Set DDRAM address (1, AC6, AC5, AC4, AC3, AC2, AC1, AC0)
    __delay_us(50);                         // Delay 50us (minimum for command to execute)
}

/***************************************
 * LCD_sendstring
 * Function to send string to LCD screen
 ***************************************/
void LCD_sendstring(char *string) {
	// Code here to send a string to LCD using pointers and LCD_sendbyte function
    while (*string != 0) {          // While the data pointed to isn't a 0x00 do below (strings in C must end with a NULL byte)
        LCD_sendbyte(*string++,1);  // Send out the current byte pointed to and increment the pointer
    }
}