/*
 * uartDriver.h
 *
 * Created: 26-05-2016 09:13:54
 *  Author: Dennis Hornbæk Poulsen
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#include <avr/io.h>
#include <avr/interrupt.h> 

class UART
{
public:
	UART(unsigned long baudRate = 9600, unsigned char dataBit = 8, char parity = 'N');
	unsigned char charReady();
	char readChar();
	void sendChar(char character);
	void sendString(char* textptr);
	void sendInteger(int number);

private:
	char character_;
};




#endif /* INCFILE1_H_ */