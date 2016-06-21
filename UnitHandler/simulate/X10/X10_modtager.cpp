//========================================================================
// FILENAME : X10_modtager.cpp
// CREATED : 25-05-2016
// AUTHOR : Christian Bøeg-Jensen
// DESCR. : Source file for driver to receiving X10.1 communication
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 <rev. date/author> <Change description>
//========================================================================

#include <avr/io.h>
#include "X10_modtager.h"

//=============================================================
// METHOD : Explicit constructor
// DESCR. : Receives the unit housecode and adress
//=============================================================
X10_modtager::X10_modtager(unsigned char houseCode, unsigned char adress)
{
	houseCode_ = houseCode;
	adress_ = adress;
	
	
	//timeout timer
	// interupt
	TIMSK3 |= (1 << TOIE3);
	TCNT2 = 0;

	
	resetReceiver();
}

//=============================================================
// METHOD : resetReceiver
// DESCR. : resets the receiver
//=============================================================
void X10_modtager::resetReceiver()
{
	packet1_ = 0;
	packet2_ = 0;
	currentPos1_ = 0;
	currentPos2_ = 0;
	currentPacket_ = 1;
	numberOfData_ = 0;
	notReciving_ = false;
	dataReady_ = false;
	type_ = 0;
	data_ = 0;
	numberOfZero_ = 0;
	packetValidity_ = false;
	TCCR3B &= ~(1 << CS31);
}

//=============================================================
// METHOD : startTimout
// DESCR. : starts the countdown to time out
//=============================================================
void X10_modtager::startTimout()
{
	TCCR3B |= (1 << CS31);
	TCNT3 = 5000;
}

//=============================================================
// METHOD : resetTimout
// DESCR. : resets the time for the time out
//=============================================================
void X10_modtager::resetTimout()
{
	TCNT3 = 5000;
}

//=============================================================
// METHOD : writeDataToPacket1
// DESCR. : writes the given data to packet 1
//=============================================================
void X10_modtager::writeDataToPacket1(unsigned char data)
{
	if(currentPos1_ > 0)
		packet1_ = (packet1_ << 1) | data;
	else
		packet1_ |= data;
		
	if(currentPos1_ < 15)
	{
		++currentPos1_;
	}		
	else
	{
		++currentPos1_;
	}		
		
}

//=============================================================
// METHOD : writeDataToPacket2
// DESCR. : writes the given data to packet 2
//=============================================================
void X10_modtager::writeDataToPacket2(unsigned char data)
{
	if(currentPos2_ > 0)
		packet2_ = (packet2_ << 1) | data;
	else
		packet2_ |= data;
	
	if(currentPos2_ < (8 + numberOfData_))
		++currentPos2_;
	else
	{
		++currentPos2_;
		currentPacket_ = 0;
	}		
}

//=============================================================
// METHOD : isPacketForMe
// DESCR. : checks if the adress in the packet match up with the units adress
//=============================================================
bool X10_modtager::isPacketForMe()
{
	if(houseCode_ == ((packet1_ >> 8) & 0xF))
		{
			if(adress_ == (packet1_ & 0xFF))
				return true;
			else
				return false;
		}
	else
		return false;
}

//=============================================================
// METHOD : getPacketType
// DESCR. : Retrieve the packet type from the packet
//=============================================================
unsigned char X10_modtager::getPacketType()
{
	return (packet2_ & 0b111);
}

//=============================================================
// METHOD : getPacketData
// DESCR. : Retrieve the packet data from the packet
//=============================================================
unsigned char X10_modtager::getPacketData()
{
	if(numberOfData_ == 1)
		return ((packet2_ >> 7) & 1);
	else if(numberOfData_ == 2)
		return ((packet2_ >> 7) & 3);
	else
		return 0;
}

//=============================================================
// METHOD : isValidStart
// DESCR. : checks if the first 4 bits are a valid start
//=============================================================
bool X10_modtager::isValidStart()
{
	if(packet1_ == 0b1110)
		return true;
	else
		return false;
}

//=============================================================
// METHOD : checkForZero
// DESCR. : check if last received data was a zero
//=============================================================
bool X10_modtager::checkForZero(unsigned char data)
{
	if(data ^ 1)
		return true;
	else
		return false;
}

//=============================================================
// METHOD : validatePackets
// DESCR. : checks if the whole packet is valid
//=============================================================
bool X10_modtager::validatePackets()
{
	unsigned char parity = 0;
	
	for (unsigned char i = 0; i < 16; i++)
	{
		parity = (parity ^ ((packet1_ >> i) & 1));
	}
	
	for (unsigned char i = 0; i < (12); i++)
	{
		parity = (parity ^ ((packet2_ >> i) & 1));
	}
	
	if(parity != 0)
		return false;
	else
		return true;
}

//=============================================================
// METHOD : receiveData
// DESCR. : Handles receiving data, and the compilation of type and data
//=============================================================
void X10_modtager::receiveData(unsigned char data)
{	
// 	if(currentPos1_ == 0)
// 		startTimout();
// 	else
// 		resetTimout();
		
	
	if(notReciving_ == false)
	{
		if(currentPacket_ == 1)
		{
			writeDataToPacket1(data);
			if(currentPos1_ == 4)
			{
				if(isValidStart())
				{
				}
				else
				{
					notReciving_ = true;
				}
			}
			else if (currentPos1_ == 16)
			{
				if(isPacketForMe())
				{
					currentPacket_ = 2;
				}
				else
				{
					notReciving_ = true;
				}
				
			}
		}
		
		else if(currentPacket_ == 2)
		{
			
			writeDataToPacket2(data);
			if(currentPos2_ == 3)
			{
				type_ = getPacketType();
				if(type_ == 0b000 || type_ == 0b011 || type_ == 0b100)
				{
					numberOfData_ = 0;
				}
				else if(type_ == 0b001 || type_ == 0b010)
				{
					numberOfData_ = 1;
				}
				else if(type_ == 0b110 || type_ == 0b101)
				{
					numberOfData_ = 2;
				}
				else
				{
				}
			}
		}
		else if(currentPacket_ == 0)
		{
			TCCR3B &= ~(1 << CS31); //stop timeout
			data_ = getPacketData();
			packetValidity_ = validatePackets();
			dataReady_ = true;
		}
	}
	else{}
		
	if(notReciving_)
	{
		PORTB |= (1 << 7);
		
		if(checkForZero(data))
			++numberOfZero_;
		else
			numberOfZero_ = 0;
		
		if(numberOfZero_ >= 6)
			resetReceiver();
		else{}
			
	}
	else{}	
}

//=============================================================
// METHOD : fetchData
// DESCR. : Extracts the type, data and the validity of the message received
//=============================================================
void X10_modtager::fetchData(unsigned char& type, unsigned char& data, bool& validity)
{
	if(dataReady_)
	{
		type = type_;
		data = data_;
		validity = packetValidity_;
		resetReceiver();
	}
	else
	{
		type = 0;
		data = 0;
		validity = 0;
	}
}

//=============================================================
// METHOD : isDataReady
// DESCR. : Returns true if there currently is data ready to be read
//=============================================================
bool X10_modtager::isDataReady()
{
	return dataReady_;
}

//=============================================================
// METHOD : startInput
// DESCR. : Start the timer for cheeking the input for a fixed time
//=============================================================
void X10_modtager::startInput()
{
	TCCR2B |= (1 << CS22);
}

//=============================================================
// METHOD : inputComplete
// DESCR. : Stops the timer and reset overflow flag in TIFR2
//=============================================================
void X10_modtager::inputComplete()
{
	TCCR2B &= ~(1 << CS22);
	TCCR2B &= ~(1 << CS12);
	TCCR2B &= ~(1 << CS02);
	TCNT2 = 0;
	TIFR2 |= (1 << TOV2);
}

//=============================================================
// METHOD : 
// DESCR. : 
//=============================================================
void X10_modtager::receiveHigh()
{
	startInput();
	bool recived = false;
	while((TIFR2 & (1 << TOV2)) == 0 && recived == false)
	{
		
		if((PINH & (1 << PINH6)) != 0)
		{
			recived = true;
			receiveData(1);
		}
	}
	inputComplete();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10_modtager::receiveLow()
{
	startInput();
	bool recived = false;
	while((TIFR2 & (1 << TOV2)) == 0 && recived == false)
	{
		if((PINH & (1 << PINH6)) != 0)
		{
			recived = true;
			receiveData(0);
		}
	}
	inputComplete();
}