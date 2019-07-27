/*	Author: Christopher Moreno
 *  Partner(s) Name: N/A
 *	Lab Section: A21
 *	Assignment: Final Project
 *	Exercise Description: LCD Side Scroller Game
 * 	The Objective of the game is to avoid the '*' to earn points.
 * 	There are 4 input buttons wired on A[3:0]
 *  The inputs control player movement, game start, and game restart
 * 	The LCD screen is wired through the shift register which is connected
 * 	to the ATMEGA1284 microcontroller using C[2:0] and D[7:6]
 * 	A highscore is stored using functions from the <avr/eeprom.h> library.
 *  The ATMEGA 1284 is programmed using the AVRDUDE utility.
 *  
 *  
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
 
#include <avr/io.h>
#include <avr/eeprom.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "scheduler.h"
#include "timer.h"
#include "io.h"
#include "custChar.h"

#define BUTTONS (~PINA & 0x0F)		//Input buttons
#define HIGHSCOREADDR 10

//tasks
task StartGameTask;
task ScrollTask;
task MovePositionTask;
task EndGameTask;					//endgame task
task *tasks[] = { &StartGameTask, &MovePositionTask, &EndGameTask, &ScrollTask };	//tasks Array
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);//TASK_SIZE;
const unsigned long tasksPeriod = 50;	//

//Global Variables
unsigned char highScore;			//hold copy of score from eeprom address
unsigned char points = 0;			//hold player points
unsigned char tens = 0;				//variable to calculate tens position of points
unsigned char ones = 0;				//variable to calculate ones position of points
unsigned char begin;				//Begin Flag
unsigned char start;				//Restart Flag
unsigned char position;				//Player Position variable
unsigned char topMessage[17];		//LCD top row
unsigned char bottomMessage[17];	//LCD bottom row

//Enumeration of States for all 4 tasks  
enum ScrollState { START, WAIT, INIT, SCROLL, GAME_OVER };
enum StartGameState { StartGame_START, StartGame_INIT,StartGame_WAIT, StartGame_PRESS, reset };
enum MovePositionState{
	MovePosition_START, 
	MovePosition_INIT, 
	MovePosition_WAIT, 
	MovePosition_PRESS_UP, 
	MovePosition_PRESS_DOWN };
enum EndGameStates { EndGame_START, EndGame_WAIT, EndGame_CHECK };                      


#define MESSAGE_SIZE 30
const unsigned char TopArray[MESSAGE_SIZE+1]    = "     *       *    *  *     *  ";
const unsigned char BottomArray[MESSAGE_SIZE+1] = "  *    *  *     *        *    ";

int ScrollSM(int state)
{
    unsigned char loopIndex;
    static unsigned char messageIndex;
    static const unsigned char endMessage[] = "ENDGAME";		//7 spaces
    static const unsigned char newHigh[] = "NEW HIGH SCORE!";	//15 spaces
    static const unsigned char yourScore[] = "Your Score: ";	//12 spaces
    static const unsigned char currHigh[] = " HS = ";			//6 spaces 
    static const unsigned char startScreen[] = "Press Start";
    
    switch(state)//State Transitions
    {
        case START:
            state = INIT;
            break;
            
        case INIT:
            state = WAIT;
            break;
            
        case WAIT:
            if(begin == 0x01)
            {
                state = SCROLL;
            }
            else
            {
                state = WAIT;
            }
            break;
            
        case SCROLL:
            if(begin == 0x01)
            {
                state = SCROLL;
            }
            else if(begin == 0x00 && start == 0x01)
            {
                state = INIT;
                start = 0x00;
            }
            else{
				state = GAME_OVER;
			}
            break;
            
        case GAME_OVER:
            state = WAIT;
            break;
            
        default:
            state = INIT;
            break;
    }
    switch(state)//State Actions
    {
        case START:
            break;
        case INIT:
            messageIndex = 17;
            strncpy(topMessage, TopArray, 16);	//copy characters from TopArray into topmessage array
            strncpy(bottomMessage, BottomArray, 16);
            LCD_ClearScreen();
            LCD_DisplayString(1,startScreen);
            LCD_Cursor(position);
            break;
            
        case WAIT:
            break;
            
        case SCROLL:
            for(loopIndex = 0; loopIndex < 16; ++loopIndex)
            {
                if((loopIndex+1) == position){
					LCD_Cursor(loopIndex+1);
					LCD_WriteData(FIRSTPLAYER);
				}
				else{
					LCD_Cursor(loopIndex+1);
					LCD_WriteData(topMessage[loopIndex]);
				}
				if((loopIndex+17) == position){
					LCD_Cursor(loopIndex+17);
					LCD_WriteData(FIRSTPLAYER);
				}
				else{
					LCD_Cursor(loopIndex+17);
					LCD_WriteData(bottomMessage[loopIndex]);
				}
            }
            LCD_Cursor(position);
/////////This sect will mov the string to the next set of characters in memory//////////////////
            if(messageIndex < MESSAGE_SIZE)	
            {
                memmove(topMessage, topMessage+1, 15);
                topMessage[15] = TopArray[messageIndex];
                memmove(bottomMessage, bottomMessage+1, 15);
                bottomMessage[15] = BottomArray[messageIndex];
                messageIndex++; 
            }
            else
            {
                messageIndex = 0;
            }
            break;
//////////////////////////////////FINAL PROJECT//////////////////////////////////////////////////////
        case GAME_OVER:
            LCD_ClearScreen();
			if(points > highScore){
				highScore = points;
				eeprom_update_byte((uint8_t*)HIGHSCOREADDR,points);
				LCD_DisplayString(1,newHigh);
				LCD_Cursor(24);
				tens = highScore / 10;
				if(tens <= 9){
					LCD_WriteData('0'+tens);
				}
				LCD_Cursor(25);
				ones = highScore % 10;
				LCD_WriteData('0'+ones);
			}
			else{//////////////////////////////////////////////////////
				LCD_DisplayString(1,endMessage);	//Game over message
				LCD_DisplayString(8,currHigh);		//current highscore message
				LCD_Cursor(14);
				tens = highScore / 10;
				if(tens <= 9){
					LCD_WriteData('0'+tens);
				}
				LCD_Cursor(15);
				ones = highScore % 10;
				LCD_WriteData('0'+ones);
				LCD_DisplayString(17, yourScore);
				LCD_Cursor(31);
				tens = points / 10;
				if(tens <= 9){
					LCD_WriteData('0'+tens);
				}
				LCD_Cursor(32);
				ones = points % 10;
				LCD_WriteData('0'+ones);
			}
				messageIndex = 0;
				points = 0;
				strncpy(topMessage, TopArray, 16);
				strncpy(bottomMessage, BottomArray, 16);
            break;
            
        default:
            break;
    }
    return state;
}

//start game SM
int StartGameSM(int state)
{
    unsigned char buttons = BUTTONS;	//Read button input
    switch(state)	//State Transitions
    {
        case StartGame_START:
            state = StartGame_INIT;
            break;
        case StartGame_INIT:
            state = StartGame_WAIT;
            break;
        case StartGame_WAIT:
            if(buttons == 0x01 && begin != 0x01)
            {
                state = StartGame_PRESS;
            }
            else if(buttons == 0x08 && begin == 0x01)
            {
				state = reset;
			}
            else
            {
                state = StartGame_WAIT;
            }
            break;
        case StartGame_PRESS:
            if(buttons != 0x00)
            {
                state = StartGame_PRESS;
            }
            else
            {
                state = StartGame_WAIT;
                begin = 0x01;			//set start game flag
            }
            break;
         case reset:
			state = StartGame_INIT;
			break;
        
        default:
            state = StartGame_WAIT;
            break;
    }

    switch(state)//State Actions
    {
        case StartGame_START:
            break;
            
        case StartGame_INIT:
            if(eeprom_read_byte((const uint8_t*)HIGHSCOREADDR) >= 255){
				highScore = 0;
			}
			else{
				highScore = eeprom_read_byte((const uint8_t*)HIGHSCOREADDR);
			}
            begin = 0;
            position = 1;
            points = 0;
            tens = 0;
            ones = 0;
            break;
            
        case StartGame_WAIT:
            break;
            
        case StartGame_PRESS:
            break;
            
        case reset:
			start = 0x01;
			break;
			
        default:
            break;
    }
    return state;
}

int MovePositionSM(int state)
{
    unsigned char buttons = BUTTONS;
    switch(state)    //State Transitions
    {
         case MovePosition_START:
             state = MovePosition_INIT;
             break;
             
         case MovePosition_INIT:
             state = MovePosition_WAIT;
             break;
             
         case MovePosition_WAIT:
             if(buttons == 0x02 && begin == 0x01)
             {
                 state = MovePosition_PRESS_DOWN;
             }
             else if(buttons == 0x04 && begin == 0x01)
             {
                 state = MovePosition_PRESS_UP;
             }
             else
             {
                 state = MovePosition_WAIT;
             }
             break;
             
         case MovePosition_PRESS_UP:
             if(buttons == 0x00)
             {
                 state = MovePosition_WAIT;
             }
             else
             {
                 state = MovePosition_PRESS_UP;
             }
             break;
             
         case MovePosition_PRESS_DOWN:
             if(buttons == 0x00)
             {
                 state = MovePosition_WAIT;
             }
             else
             {
                 state = MovePosition_PRESS_DOWN;
             }
             break;
             
         default:
             state = MovePosition_WAIT;
             break;
    }
    
    switch(state)    //State Actions
    {
         case MovePosition_START:
             break;
             
         case MovePosition_INIT:
             position = 1;				//sets the player position to the 1st row 1st column initially
             break;
             
         case MovePosition_WAIT:
             break;
             
         case MovePosition_PRESS_UP:
             position = 1;				//sets the player position to the 1st row 1st column
             break;
             
         case MovePosition_PRESS_DOWN:
             position = 17;				//sets the player position to the 2nd row 1st column
             break;
             
         default:
             break;
    }
    return state;
}

int EndGameSM(int state)
{
    switch(state)    //State Transitions
    {
        case EndGame_START:
            state = EndGame_WAIT;
            break;
            
        case EndGame_WAIT:
            if(begin == 0x00)
            {
                state = EndGame_WAIT;
            }
            else
            {
                state = EndGame_CHECK;
            }
            break;
            
        case EndGame_CHECK:
            if((position == 1 && topMessage[0] == '*') ||
                (position == 17 && bottomMessage[0] == '*'))
            {
                begin = 0x00;
                state = EndGame_WAIT;
            }
            else if((topMessage[0] == '*') || (bottomMessage[0] == '*'))
            {
				points++;
				state = EndGame_WAIT;
			}            
			else
            {
                state = EndGame_CHECK;
            }
            break;
            
        default:
            break;
    }

    switch(state)    //State Actions
    {
        case EndGame_START:
            break;
            
        case EndGame_WAIT:
            break;
            
        case EndGame_CHECK:
            break;
            
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;  //set PORTA for input
    DDRD = 0xFF; PORTD = 0x00;  // LCD control lines
    LCD_init();					//initialize LCD displays
    /////////////////////////////////////////////////////////////////FP////////////////////
    WriteCust(pattern1,FIRSTPLAYER);	//creates custom playable character
    TimerSet(tasksPeriod);	//Initializes timer
    TimerOn();	//turns on timer
    //initialize tasks
    ScrollTask.state = START;
    ScrollTask.period = 200;
    ScrollTask.elapsedTime = 200;//lengthen period time so the LCD can project faster
    ScrollTask.TickFct = &ScrollSM;
    
    StartGameTask.state = StartGame_START;
    StartGameTask.period = 50;
    StartGameTask.elapsedTime = 50;
    StartGameTask.TickFct = &StartGameSM;

    MovePositionTask.state = MovePosition_START;
    MovePositionTask.period = 50;
    MovePositionTask.elapsedTime = 50;
    MovePositionTask.TickFct = &MovePositionSM;

    EndGameTask.state = EndGame_START;
    EndGameTask.period = 100;
    EndGameTask.elapsedTime = 100;
    EndGameTask.TickFct = &EndGameSM;
    while (1)
    {
		unsigned char i;
		for(i = 0; i < numTasks; ++i){
        if(tasks[i]->elapsedTime >= tasks[i]->period){
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += tasksPeriod;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
