#ifndef SHIFT_H
#define SHIFT_H
#include <avr/io.h>
#include <util/delay.h>

/*
 * File created using Interfacing Shift Register w/AVR Tutorial from:
 * https://embedds.com/interfacing-shift-register-with-avr/
 */

#define OUTPUT_PORT PORTC   //port to output to shift register
#define OUTPUT_DDR DDRC
#define DS_POS PC0          //data pin(DS) the 4 and 6 pins from the LCD
#define SH_CP_POS PC1       //shift clock(SH_CP) {SRCLK}
#define ST_CP_POS PC2       //store clock(ST_CP) {RCLK}
// change data (DS)lines
#define ShiftDataHigh() (OUTPUT_PORT|=(1<<DS_POS))
#define ShiftDataLow() (OUTPUT_PORT&=(~(1<<DS_POS)))

void shiftInit()
{
   //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
   OUTPUT_DDR|=((1<<SH_CP_POS)|(1<<ST_CP_POS)|(1<<DS_POS));
}

//Sends a clock pulse on SH_CP line
void shiftPulse()
{
   //Pulse the Shift Clock
   OUTPUT_PORT|=(1<<SH_CP_POS);//set shift clock to HIGH
   OUTPUT_PORT&=(~(1<<SH_CP_POS));//set shift clock to LOW
}

//Sends a clock pulse on ST_CP line
void shiftLatch()
{
   //Pulse the Store Clock
   OUTPUT_PORT|=(1<<ST_CP_POS);//set RCLK HIGH
   _delay_loop_1(1);
   OUTPUT_PORT&=(~(1<<ST_CP_POS));//set RCLK LOW
   _delay_loop_1(1);
}

void shiftWrite(uint8_t data)
{
    //send each of the 8 bits serially Most Significant Bit(MSB)
   for(uint8_t i=0;i<8;i++)
   {
      //Output the data on DS line according to the
      //Value of MSB
      if(data & 0b10000000)
      {
         //MSB is 1 so output high
         ShiftDataHigh();
      }
      else
      {
         //MSB is 0 so output low
         ShiftDataLow();
      }
      shiftPulse();  //Pulse the Clock line
      data=data<<1;  //Now bring next bit at MSB position
   }
   //Now all 8 bits have been transferred to shift register
   //Move them to output latch at one
   shiftLatch();
}

//Simple Delay function approx 0.5 seconds
void Wait()
{
   for(uint8_t i=0;i<50;i++)
   {
      _delay_loop_2(0);
   }
}
#endif
