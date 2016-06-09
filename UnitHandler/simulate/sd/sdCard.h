//========================================================================
// FILENAME : sdCard.h
// CREATED : 22-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : Header file for the arduino SD Card driver.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 22-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================
#include <avr/io.h>
#include <util/delay.h>
#include "../SPI/SPI.h"

#ifndef SDCARD_H_
#define SDCARD_H_
#define F_CPU 16000000
#define PORT_SD PORTB // set output PORT for the sd card
#define PIN_SD PINB  // used to read to port for handling the SS pin when needed.
// helper type to make it easier to handle commands.
// might show up later

//=====================================
// CLASS : sdCard
// DESCR. : represends the SD card controlled using SPI in master mode
// on arduino mega2560, the sd card i used in raw block access mode.
//=====================================

class sdCard{
	public:
		sdCard(int speed); // speed in kHz (the speed you wish to communicate with the sd card in after initiation is complete.
		unsigned char getResponeByte();
		void writeByte(unsigned char);
		bool init(); // initializes the SD card.
		bool writeBlock(unsigned long adress, const unsigned char data[]); // data_length must be less than 512.
		unsigned char readBlock(unsigned long adress, unsigned char outputdata[]); // will fill the supplied array with the content of the block on the sd card.
	private:
		SPI spi_obj;
		int speed;
		void sendCommand(unsigned char cmdindex, unsigned long argument, unsigned char CRC); // used internally to send a sd command.

//		unsigned char getResponeByte();
};



#endif /* SDCARD_H_ */