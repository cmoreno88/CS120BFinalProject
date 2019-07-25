#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "keypad.h"
#include "scheduler.h"
//#include "mtimer.h"
#include "io.h"
#include "custChar.h"
//#define pat

/* Example for using the WriteCust() function*/

int main(void) {
	//DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//Declare the array of the patterns you want to print
	//unsigned char pattern1 [8] = { 0x0E, 0x0E, 0x04, 0x04, 0x1F, 0x04, 0x0A, 0x0A };
	//initialize the LCD
	LCD_init();
	//pass the array and the location 0-7 of the newly addressed character
	WriteCust(pattern1, 0);
	//set the cursor postition for where you want your custom char printed
	LCD_Cursor(1);
	//pass the location of the custom char that you want printed
	LCD_WriteData(0);
    while(1){
	
		}
    return 0;
}
