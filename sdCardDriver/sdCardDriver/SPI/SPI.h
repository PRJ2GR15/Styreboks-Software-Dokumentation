//========================================================================
// FILENAME : SPI.h
// CREATED : 22-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : Header file for the ardiono SPI driver.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 22-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================

#ifndef SPI_H_
#define SPI_H_
#include <avr/io.h>
// using port B for SPI
#define SCK 1		 //SPI Clock (pin52)
#define MOSI 2		 //Master out - Slave in (pin51)
#define MISO 3		 //Master in - Slave out (pin50)

//=====================================
// CLASS : SPI
// DESCR. : represents the SPI module in master mode on arduino mega2560, the SS pin is handled outside of this driver.
//=====================================

class SPI{
	
	public:
	SPI(int freq); // available freq must be one of the following and is in kHz: 125 kHz, 250 kHz 1000 kHz or 4000 kHz
	unsigned char writeByte(unsigned char data); // returns the response from the slave.
	unsigned char recieveByte(); // returns the unsigned char recieved from the slave
	void setFreq(int freq); // same freq limitations as the constructor.
	private:
};



#endif /* SPI_H_ */