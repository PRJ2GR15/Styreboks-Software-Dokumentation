/*
 * uartDriver.cpp
 *
 * Created: 26-05-2016 09:13:40
 *  Author: Dennis Hornbæk Poulsen
 */ 


#include <avr/io.h>
#include "uartDriver.h"

ISR (USART0_RX_vect)
{
	UART uart2;
	uart2.sendChar('f');
}

int main(void)
{	
	UART uart1;
	
    while(1)
    {
		uart1.sendChar('D');
		uart1.sendString(" Fuck ");
		uart1.readChar();
	}
}