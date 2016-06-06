/*
 * uartDriver.cpp
 *
 * Created: 26-05-2016 09:18:42
 *  Author: Dennis Hornbæk Poulsen
 */ 

#include "uartDriver.h"
#include <stdlib.h>
#define F_CPU 16000000


UART::UART(unsigned long baudRate, unsigned char dataBit, char parity)
{
	if ((baudRate >= 110) && (baudRate <= 115200) && (dataBit <= 8) && (dataBit >= 5))
	{
		UCSR0A = 0b00100000;
		UCSR0B = 0b10011000;
		UCSR0C = (dataBit-5)<<1;
		
		unsigned long tempUBRR;
		
		tempUBRR = F_CPU/(16*baudRate)-1;
		
		UBRR0L = tempUBRR;
		UBRR0H = tempUBRR >> 8;
	}
	
	if (parity == 'E')
	{
		UCSR0C |= 0b00100000;
	}
	else if (parity == 'O')
	{
		UCSR0C |= 0b00110000;
	}	
	
}

unsigned char UART::charReady()
{
	return UCSR0A & (1<<7);
}

char UART::readChar()
{
	while ((UCSR0A & 0b10000000) == 0)
	{}
	return UDR0;
}

void UART::sendChar(char character)
{
	while ((UCSR0A & 0b00100000) == 0)
	{}
	UDR0 = character;
}


void UART::sendString(char* textptr)
{
	while (*textptr != 0)
	{
		sendChar(*textptr);
		textptr++;
	}
	
}

void UART::sendInteger(int number)
{
	char myArray[7];
	itoa(number, myArray, 10);
	sendString(myArray);
}