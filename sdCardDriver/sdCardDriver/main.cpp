/*
 * sdCard.cpp
 *
 * Created: 22-05-2016 11:08:25
 *  Author: Tonni
 */ 
#define F_CPU 16000000

#include <avr/io.h>

#include "UART/uart.h"
#include "sd/sdCard.h"
#include <util/delay.h>
int main(void)
{

	InitUART(9600, 8, 0);
	sdCard SD_obj(4000); // create SD card object for handling the sd card
	_delay_ms(10); // do a small delay to allow the SD card to wake up.
	SendChar(SD_obj.init()); // run init sequence and print the result to UART
	unsigned char inData[512]; // array for storring read data
	unsigned char dataOut[512]; // array for storing data to be written
	for ( int x = 0; x < 512; x++){ // fill the array with dummy data
		dataOut[x] = 0xEF;
	}
	SendChar(dataOut[7]); // write byte 8 of the array to uart

	if(SD_obj.writeBlock(0xFFFFFFFF, dataOut)){ // write data to SD CARd (current adress is not a legal adress, used for testing error handling
		SendChar(0xCC);
	} else {
		SendChar(0xAB);
	}

	SendChar(0xBB);
	SendChar(0xBB);

	if(SD_obj.readBlock(0xFFFFFFFF, inData)){
		for( int j = 0; j < 512; j++){
			SendChar(inData[j]);
		}
	} else {
		SendChar(0xE9);
	}
}