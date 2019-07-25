#include <avr/io.h>

//#include "../header/scheduler.h"
//#include "../header/mtimer.h"
//#include "../header/keypad.h"
//#include "../header/io.h"
#include "timer.h"
#include "shift.h"
//#include "../header/pwm.h"
#ifdef _SIMULATE_
//#include "simAVRHeader.h"
#endif

//#define CLOCK (PINC & 0x01)
//#define DATA (PORTC & 0x02)

int main(void) {
    //output of columns
//    DDRC = 0xFF;
//    PORTC = 0x00;
    
    //set port A as output columns
//    DDRA = 0xFF;
//    PORTA = 0x00;

//    unsigned short data;
//    unsigned short i;
    //set period timer
    TimerSet(1000);
    //initialize timer
    TimerOn();
    //enable interupts
//    sei();
    //enable keyboard
//    Keyboard_Init();
    //gets the key code from the keyboard
//    unsigned char row = 0;
//    unsigned char column = 0;
    uint8_t led[2] = {
                        0b11110000,
                        0b00001111
                    };
    uint8_t i = 0;
    //initialize
    shiftInit();
    while (1)
    {
//        buttons = GET_BUTTON;
//        ToneSM();
//        data = DATA;
//        PORTB = data;
//        if(char_waiting)
//        {
//            key_code = read_char();
//            PORTA = key_code;
//        }
//        if(row < 7)
//        {
//            if(column < 7)
//            {
//                column++;
//            }
//            else
//            {
//                row++;
//                column = 0;
//            }
//        }
//        else
//        {
//            if(column < 7)
//            {
//                column++;
//            }
//            else
//            {
//                row = 0;
//                column = 0;
//            }
//        }
//       PORTA = (0x00 | (1 << column));
//       PORTC = (0xFF & ~(1 << row));
//        PORTA = 0xFF;
//        PORTC = 0x00;
        //PORTB = 0xFF;
        if(i == 0)
        {
            shiftWrite(0x00);
            i = 1;
        }
        else
        {
            shiftWrite(0xFF);
            i = 0;
        }
//        if(i < 2)
//        {
//            ShiftReg_Write(led[i]);
//            i++;
//            Wait();
//        }
//        else
//        {
//            i = 0;
//        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
}
