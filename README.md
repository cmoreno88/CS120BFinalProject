# CS120BFinalProject
Repository for CS120B Summer 2019 Final Project
Exercise Description: LCD Side Scroller Game
The Objective of the game is to avoid the '*' to earn points.
There are 4 input buttons wired on A[3:0]
The inputs control player movement, game start, and game restart.
The LCD screen is wired through the shift register which is connected to the ATMEGA1284 microcontroller
using C[2:0] and D[7:6].	
A highscore is stored using functions from the <avr/eeprom.h> library. 
The ATMEGA 1284 is programmed using the AVRDUDE utility.
