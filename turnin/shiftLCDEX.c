#include <avr/io.h>

#include "scheduler.h"
//#include "../header/mtimer.h"
#include "keypad.h"
#include "io.h"
#include "timer.h"
#include "shift.h"
#include "custChar.h"
//#include "../header/pwm.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

////////////////////////DISPLAYS CUSTOM CHAR WITH SHIFT REG///////////////////////////

int main(void) {
    //output of columns
   DDRD = 0xFF;	PORTD = 0x00;
    
    //set port A as output columns
    //DDRC = 0xFF;
    //PORTC = 0x00;

//    unsigned short data;
//    unsigned short i;
    //set period timer
    //TimerSet(1000);
    //initialize timer
    //TimerOn();
   // shiftInit();
    LCD_init();
    WriteCust(pattern1,0);
	LCD_Cursor(17);
	LCD_WriteData(0);
   // LCD_DisplayString(1,"Hello World");
    //initialize
    //shiftInit();
    while (1)
    {
        //while(!TimerFlag);
        //TimerFlag = 0;
    }
}
