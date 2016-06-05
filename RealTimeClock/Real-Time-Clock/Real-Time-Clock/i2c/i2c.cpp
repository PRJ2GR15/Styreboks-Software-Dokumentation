//========================================================================
// FILENAME : i2c.cpp
// CREATED : 08-04-2016
// AUTHOR : Stefan Nielsen
// DESCR. : Implementation of the i2c/TWI driver class
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.1 18-05-2016/Tonni Nybo Follmann added support for setting the clock frequency to 100 or 400 kHz
// 1.0 08-04-2016/Stefan Nielsen created the initial working version
//========================================================================
#include <avr/io.h>
#include "i2c.h"
//=====================================
// CLASS : i2c
// DESCR. : Represents the i2c/TWI interface of the arduino mega2560
//=====================================

//=============================================================
// METHOD : i2c
// DESCR. : constructor that initiates the i2c module as master with
// a chosen clock frequency of 100 kHz or 400 kHz.
//=============================================================
i2c::i2c(bool isMaster, int clock)
{
	if(isMaster == true)
	{
		if(clock == 400)
		{
			TWSR = 0b00000001; // set prescaler
			TWBR = 0x03; // sets SCL to 400 kHz
		}
		else if(clock == 100){
			TWSR = 0b00000001; // set prescaler
			TWBR = 0x12; // sets SCL to 100 kHz
		}
		else
		{
			TWSR = 0b00000011; // set prescaler
			TWBR = 0x14; // sets SCL to 6,211 kHz
		}
		TWCR = 0x04; // enable the TWI module in mega2560.
	}
}
//=============================================================
// METHOD : start
// DESCR. : sends the start command on the i2c interface.
//=============================================================
void i2c::start()
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1<<TWINT)));
}
//=============================================================
// METHOD : stop
// DESCR. : sends the stop command on the i2c interface.
//=============================================================
void i2c::stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN ) | (1<<TWSTO);
};
//=============================================================
// METHOD : write
// DESCR. : writes a single byte of data to the i2c interface.
//=============================================================
void i2c::write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<< TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	
};
//=============================================================
// METHOD : read
// DESCR. : reads 1 byte of data from the i2c interface
// and transmits the ack, if it is not the last byte to be read.
//=============================================================
unsigned char i2c::read(bool isLast)
{
	if ( isLast == 0)
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	else
		TWCR = (1<<TWINT) | (1<<TWEN);
	while ((TWCR & (1 <<TWINT)) == 0);
	return TWDR;
};