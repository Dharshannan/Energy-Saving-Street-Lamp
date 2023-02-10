// This is a guard condition so that contents of this file are not included more than once.  
#ifndef _LCD_H
#define	_LCD_H

#include <xc.h>
#include "datetime.h"

#define _XTAL_FREQ 64000000

// This section defines pins used with the LCD screen
#define LCD_RS LATCbits.LATC6   // H: Data, L: Instruction code
#define LCD_E LATCbits.LATC2    // Chip enable signal (LCD enable bit)
#define LCD_DB4 LATBbits.LATB3  // Data bus line
#define LCD_DB5 LATBbits.LATB2  // Data bus line
#define LCD_DB6 LATEbits.LATE3  // Data bus line
#define LCD_DB7 LATEbits.LATE1  // Data bus line

// Function prototypes
void LCD_E_TOG(void);
void LCD_sendnibble(unsigned char number);
void LCD_sendbyte(unsigned char Byte, char type);
void LCD_init(date_time curr);
void LCD_setline (char line);
void LCD_sendstring(char *string);

#endif	/* _LCD_H */