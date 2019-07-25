/*	Author: Christopher Moreno
 *  Partner(s) Name: Alex Ku
 *	Lab Section: A21
 *	Assignment: Lab 11  Exercise 5
 *	Exercise Description: 
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
#include "keypad.h"
#include "scheduler.h"
#include "mtimer.h"
#include "io.h"
#include "custChar.h"



//THE TASKS
//number of tasks
#define TASK_SIZE 4

const unsigned long tasksPeriod = 50;
//number of tasks
const unsigned char tasksSize = TASK_SIZE;
//the starting of the game task
task StartGameTask;
//the srolling task;
task ScrollTask;
//moving user position task
task MovePositionTask;
//check for game end task
task GameEndTask;
//Array of tasks
task *tasks[TASK_SIZE] = {&StartGameTask, &MovePositionTask, & GameEndTask,
                            &ScrollTask};
//END OF TASKS 

//BEGIN SHARED VARIABLES
//check if game has begun
unsigned char begin;

//////////////////////////////////////////////
#define HIGHSCOREADDR 10
unsigned char highScore;
unsigned char points = 0;
unsigned char tens = 0;
unsigned char ones = 0;



//all button inputs
#define GET_BUTTONS (~PINA & 0x0F)
//#define GET_BUTTONS (~PINA & 0x07)
//player position
unsigned char position;
//message to print at top and bottom respectively
unsigned char topMessage[17];
unsigned char bottomMessage[17];
//END SHARED VARIABLES

//Enumeration of States for all 4 tasks  
enum ScrollState { START, WAIT, INIT, SCROLL, GAME_OVER };
enum StartGameState { StartGame_START, StartGame_INIT,StartGame_WAIT, StartGame_PRESS, reset };
enum MovePositionState{ MovePosition_START, MovePosition_INIT, MovePosition_WAIT, MovePosition_PRESS_UP, 
	MovePosition_PRESS_DOWN };
enum GameEndStates {GameEnd_START, GameEnd_WAIT, GameEnd_CHECK};                      


#define MESSAGE_SIZE 30
const unsigned char TopArray[MESSAGE_SIZE+1] = "     *       *       *     "
                                                "   ";
const unsigned char BottomArray[MESSAGE_SIZE+1] = "       *        *      "
                                                "  *    ";

int ScrollSM(int state)
{
    unsigned char loopIndex;
    static unsigned char messageIndex;
    static const unsigned char endMessage[] = "GAME OVER";
    static const unsigned char newHigh[] = "NEW HIGH SCORE!";
    static const unsigned char yourScore[]= "Your Score: ";
    switch(state)//transitions
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
            else
            {
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
    switch(state)//actions
    {
        case START:
            break;
        case INIT:
            messageIndex = 17;
            strncpy(topMessage, TopArray, 16);	//copy characters from TopArray into topmessage array
            strncpy(bottomMessage, BottomArray, 16);
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
         //   LCD_DisplayString(1, endMessage); "NEW HIGH SCORE!"
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
			else{//14 to print "your score: "
				LCD_DisplayString(1,endMessage);
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
    //getting button press
    unsigned char buttons = GET_BUTTONS;
    //transitions
    switch(state)
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
                begin = 0x01;			//starts the game
            }
            break;
         case reset:
			state = StartGame_INIT;
			break;
        
        default:
            state = StartGame_WAIT;
            break;
    }

    switch(state)///////actions////////
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
			break;
			
        default:
            break;
    }
    return state;
}

//move position SM
int MovePositionSM(int state)
{
    unsigned char buttons = GET_BUTTONS;
    switch(state)    //transitions
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
    
    switch(state)    //actions
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

//game end sm
int GameEndSM(int state)
{
    switch(state)    //transitions
    {
        case GameEnd_START:
            state = GameEnd_WAIT;
            break;
        case GameEnd_WAIT:
            if(begin == 0x00)
            {
                state = GameEnd_WAIT;
            }
            else
            {
                state = GameEnd_CHECK;
            }
            break;
        case GameEnd_CHECK:
            if((position == 1 && topMessage[0] == '*') ||
                (position == 17 && bottomMessage[0] == '*'))
            {
                begin = 0x00;
                state = GameEnd_WAIT;
            }
            else if((topMessage[0] == '*') || (bottomMessage[0] == '*'))
            {
				//begin == 0x01;
				points++;
				state = GameEnd_WAIT;
			}            
			else
            {
                state = GameEnd_CHECK;
            }
            break;
        default:
            break;
    }

    switch(state)    //actions
    {
        case GameEnd_START:
            break;
        case GameEnd_WAIT:
            break;
        case GameEnd_CHECK:
            break;
        default:
            break;
    }
    return state;
}

//change timer flag when one period is complete
void TimerISR()
{
    unsigned char i;
    for(i = 0; i < tasksSize; ++i)
    {
        //if task is ready to exexute
        if(tasks[i]->elapsedTime >= tasks[i]->period)
        {
            //execute tick function and update state
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            //reset elasped time
            tasks[i]->elapsedTime = 0;
        }
        //update elasped time
        tasks[i]->elapsedTime += tasksPeriod;
    }
}

int main(void) {
    //inputs
    DDRA = 0x00; PORTA = 0xFF;
    //outputs
    //output to port b
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00; // LCD control lines
    //initialize LCD displays
    LCD_init();
    /////////////////////////////////////////////////////////////////FP////////////////////
    WriteCust(pattern1,FIRSTPLAYER);
//
    //initialize timer
    TimerSet(tasksPeriod);
    //turn on timer
    TimerOn();
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

    GameEndTask.state = GameEnd_START;
    GameEndTask.period = 100;
    GameEndTask.elapsedTime = 100;
    GameEndTask.TickFct = &GameEndSM;
    while (1)
    {
	

    }
    return 0;
}
