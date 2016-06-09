//========================================================================
// FILENAME : SPI.h
// CREATED : 22-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : Implementation file for the ardiono SPI driver.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 22-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================
#include "SPI.h"
//=====================================
// CLASS : SPI
// DESCR. : represents the SPI module in master mode on arduino mega2560, the SS pin is Handled outside of this driver.
//=====================================


//=============================================================
// METHOD : SPI
// DESCR. : constructor, initalizes SPI and sets mode to master,
// sets the SCK and MOSI Pins to output and sets the SCK frequenzy
// to freq.
//=============================================================
 SPI::SPI( int freq )
{
	// only using master mode in the project so slave mode is not part of this SPI driver.
	DDRB = 0b00000111; // sets MOSI as output and SCK as output rest as input on port B.
	SPCR = 0b01010000; // enable SPI and set mode to master
	setFreq(freq);
}
//=============================================================
// METHOD : setFreq
// DESCR. : Sets the frequency of the SCK, takes a freq in Khz
// as a parameter, 4000, 1000, 250, 125 are available options,
// defaults to 4000 if invalid parameter.
//=============================================================
void SPI::setFreq( int freq ) // sets prescaler based on requested freq in kHz, calculations assume a 16MHz clock speed.
{
	switch(freq)
	{
	case 4000:
		SPCR = SPCR & 0b11111100; // prescaler = 4
		break;
	case 1000:
		SPCR = (SPCR & 0b11111100) | 0b00000001; // prescaler = 16
		break;
	case 250:
		SPCR = (SPCR & 0b11111100) | 0b00000010; // prescaler = 64
		break;
	case 125:
		SPCR = SPCR | 0b00000011; // prescaler = 128
		break;
	default:
		SPCR = SPCR & 0b11111100; // defaults to 4000 kHz
		break;
	}
}
//=============================================================
// METHOD : writeByte
// DESCR. : sends 1 byte of data out on MOSI,
// returns the response recieved from the reciever
//=============================================================
unsigned char SPI::writeByte( unsigned char data )
{
	SPDR = data; // starting the transmission
	while(!(SPSR &(1<<SPIF)));
	return SPDR;	
}
//=============================================================
// METHOD : recieveByte
// DESCR. : Recieves 1 byte of data, has to send out dummy data to MOSI to recieve.
// returns the response recieved from the reciever
//=============================================================
unsigned char SPI::recieveByte()
{
	SPDR = 0xFF; // sends don't care byte to transmit clocks.
	while(!(SPSR &(1<<SPIF)));
	return SPDR; // read data and return it.
}
