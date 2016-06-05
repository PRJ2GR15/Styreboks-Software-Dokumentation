/*
 * PCInterface.cpp
 *
 * Created: 26-05-2016 13:38:50
 *  Author: Tonni
 */ 


#include <avr/io.h>
#include "Uart/uartDriver.h"
#include "SD/sdCard.h"
#include "UnitHandler/UnitHandler.h"
#include <avr/interrupt.h>
#include "PCinterface/PCinterface.h"
#define F_CPU 16000000

UART uart_obj(57600, 8, 'E');
sdCard SD_obj(4000);
UnitHandler handler_obj(&SD_obj);
PCinterface PCIF_obj(&handler_obj ,&uart_obj);


int main(void)
{
	unsigned char Array[512] = { 0 };
	for (int i = 0; i < 101; i++){
		SD_obj.writeBlock(i, Array);
	}	
    while(1)
    {
		sei();
    }
}

ISR (USART0_RX_vect)
{
	PCIF_obj.handleCMD();
	//uart_obj.sendChar(0xAB);
}