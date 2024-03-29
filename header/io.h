#ifndef __io_h__
#define __io_h__
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include "shift.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))
          
/*-------------------------------------------------------------------------*/

//Changed port assignments for shift register use
//#define DATA_BUS PORTC	// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTD	// port connected to pins 4 and 6 of LCD disp.
#define RS 6				// pin number of uC connected to pin 4 of LCD disp. register select
#define E 7					// pin number of uC connected to pin 6 of LCD disp. enable line

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

////////////////////Created for Final Project//////////////////////////////////////
void WriteCust(unsigned char *pattern, unsigned char location){
	if(location < 8)
	{
		LCD_WriteCommand (0x40 + (location * 8));
		unsigned char i;
		for(i = 0; i < 8; i++){
			LCD_WriteData(pattern[i]);
			}
		}
}
//////////////////////////////////////////////////////////////////////////////////
void LCD_init(void) {

    //wait for 100 ms.
    shiftInit();
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0C);//LCD_WriteCommand(0x0f); Changed to stop cursor blinking
	LCD_WriteCommand(0x01);
	delay_ms(10);						 
}

void LCD_WriteCommand (unsigned char Command) {
   CLR_BIT(CONTROL_BUS,RS);
   //DATA_BUS = Command; 	//set value from PORTC
   shiftWrite(Command);		//set value to the shift register output
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(CONTROL_BUS,RS);
   //////////////////////////////////////////FINAL PROJECT/////////////////////////////////
   //DATA_BUS = Data;		//set to PORTC output
   shiftWrite(Data);		//set to the shift register data
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
   //LCD_ClearScreen();
   unsigned char c = column;
   while(*string) {
      LCD_Cursor(c++);
      LCD_WriteData(*string++);
   }
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
											// 16x2 LCD: column - 9
   }
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
}

#endif

