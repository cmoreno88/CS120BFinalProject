#ifndef CUSTCHAR_H
#define CUSTCHAR_H
#include "io.h"

#define FIRSTPLAYER 0


unsigned char pattern1 [] = { 0x0E, 0x0E, 0x04, 0x04, 0x1F, 0x04, 0x0A, 0x0A };	//Address 0
/*
void WriteCust(unsigned char *pattern, const char location){
	unsigned char i;
	if(location < 8)
	{
		LCD_WriteCommand (0x40 + (location*8));
		for(i=0; i < 8; i++){
			LCD_WriteData(pattern[i]);
		}
}*/
	
	
	
#endif
