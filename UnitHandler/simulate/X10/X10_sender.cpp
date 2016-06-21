//========================================================================
// FILENAME : X10_sender.cpp
// CREATED : 25-05-2016
// AUTHOR : Christian Bøeg-Jensen
// DESCR. : Source file for the driver to send data via X10.1
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 <rev. date/author> <Change description>
//========================================================================

#include <avr/io.h>
#include "X10_sender.h"


//=============================================================
// METHOD : Default constructor
// DESCR. :
//=============================================================
X10_sender::X10_sender()
{
	outGoingPacket1_ = 0;
	outGoingPacket2_ = 0;
	currentPacket_ = 0;
	currentPos1_ = 0;
	currentPos2_ = 0;
	sendMode_ = 0;
	startBit_ = 0b1110;
	sendMode_ = false;
	
	
	// MODE 8: PWM freq og fase corret
	TCCR1B |= (1 << WGM13);
	TCCR1B &= ~(1 << WGM12);
	TCCR1A &= ~(1 << WGM11);
	TCCR1A &= ~(1 << WGM10);
	
	// No prescaler
	TCCR1B &= ~(1 << CS12);
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS10);
	
	// Set TOP, for rigtig frekvens (120kHz)
	ICR1 = 67;

	// Set det til A systemmet, PB5
	TCCR1A |= (1 << COM1A1);
	TCCR1A &= ~(1 << COM1A0);
	
	// duty 0%
	OCR1A = 0;
	
	// PB5 Output
	DDRB |= (1 << 5);
	
	
	//MODE 0: Normal tmer
	TCCR0B &= ~(1 << WGM02);
	TCCR0A &= ~(1 << WGM01);
	TCCR0A &= ~(1 << WGM00);
	
	//no timer
	TCCR0B &= ~(1 << CS02);
	TCCR0B &= ~(1 << CS01);
	TCCR0B &= ~(1 << CS00);
	
	// interupt
	TIMSK0 |= (1 << TOIE0);
	TCNT0 = 6;
}

//=============================================================
// METHOD : start120
// DESCR. : produce a 120 kHz PWM signal and starts a timer to stop it
//=============================================================
void X10_sender::start120()
{
	// duty 50%
	OCR1A = 33;
		
	//64 prescaler
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= (1 << CS01);
	TCCR0B |= (1 << CS00);
}

//=============================================================
// METHOD : stop120
// DESCR. : stops the 120 kHz PWM and stops the timer
//=============================================================
void X10_sender::stop120()
{
	TCCR0B &= ~(1 << CS02);
	TCCR0B &= ~(1 << CS01);
	TCCR0B &= ~(1 << CS00);
	TCNT0 = 6;
	
	OCR1A = 0;	
}

//=============================================================
// METHOD : setSendMode
// DESCR. : sets the send mode to true or false
//=============================================================
void X10_sender::setSendMode(bool newSendMode)
{
	sendMode_ = newSendMode;
}

//=============================================================
// METHOD : getSendMode
// DESCR. : retrive the current send mode
//=============================================================
bool X10_sender::getSendMode()
{
	return sendMode_;
}

//=============================================================
// METHOD : sendCommand
// DESCR. : Handles compiling a packet to send, and sets the unit to send the packet
//=============================================================
void X10_sender::sendCommand(unsigned char houseCode, unsigned char adress, unsigned char type, unsigned char data)
{
	volatile unsigned int tempPacketData1 = 0;
	volatile unsigned int tempPacketData2 = 0;
	unsigned char tempPacketSize = 0;
	
	
	tempPacketData1 |= startBit_;
	
	tempPacketData1 = tempPacketData1 << 4;
	tempPacketData1 |= houseCode;
	
	tempPacketData1 = tempPacketData1 << 8;
	tempPacketData1 |= adress;
	
	tempPacketData2 |= type;
	
	if(type == 0b000 || type == 0b011 || type == 0b100)
	{
		tempPacketSize = 9;
	}
	
	else if(type == 0b001 || type == 0b010)
	{
		tempPacketData2 = tempPacketData2 << 1;
		tempPacketData2 |= data;
				
		tempPacketSize = 10;
	}
	
	else if(type == 0b110 || type == 0b101)
	{
		tempPacketData2 = tempPacketData2 << 2;
		tempPacketData2 |= data;
	
		tempPacketSize = 11;
	}
	
	else
	{
		return;
	}
		
	tempPacketData2 = tempPacketData2 << 1;
	tempPacketData2 |= generateParity(tempPacketData1, tempPacketData2);
	
	tempPacketData2 = tempPacketData2 << 6;
		
	outGoingPacket1_ = tempPacketData1;
	outGoingPacket2_ = tempPacketData2;
	currentPos1_ = 15;
	currentPos2_ = tempPacketSize;
	sendMode_ = true;
	currentPacket_ = 1;
}

//=============================================================
// METHOD : getPacket1
// DESCR. : retrieve packet 1
//=============================================================
unsigned int X10_sender::getPacket1()
{
	return outGoingPacket1_;
}

//=============================================================
// METHOD : getPacket2
// DESCR. : retrieve packet 1
//=============================================================
unsigned int X10_sender::getPacket2()
{
	return outGoingPacket2_;
}

//=============================================================
// METHOD : getCurrentPacket
// DESCR. : retrieve what packet part is being send
//=============================================================
unsigned char X10_sender::getCurrentPacket()
{
	return currentPacket_;
}

//=============================================================
// METHOD : setCurrentPacket
// DESCR. : changes what packet part to send
//=============================================================
void X10_sender::setCurrentPacket(unsigned char packet)
{
	currentPacket_ = packet;
}

//=============================================================
// METHOD : decrementCurrentPos1
// DESCR. : decrement the current pos for the current bit to send on packet 1
//=============================================================
void X10_sender::decrementCurrentPos1()
{
	--currentPos1_;
}

//=============================================================
// METHOD : getCurrentPos1
// DESCR. : retrieve the current pos of the bit to send
//=============================================================
unsigned char X10_sender::getCurrentPos1()
{
	return currentPos1_;
}

//=============================================================
// METHOD : decrementCurrentPos2
// DESCR. : decrement the current pos for the current bit to send on packet 2
//=============================================================
void X10_sender::decrementCurrentPos2()
{
	--currentPos2_;
}

//=============================================================
// METHOD : getCurrentPos2
// DESCR. : retrieve the current pos of the bit to send
//=============================================================
unsigned char X10_sender::getCurrentPos2()
{
	return currentPos2_;
}

//=============================================================
// METHOD : generateParity
// DESCR. : generates a parity bit for the whole packet
//=============================================================
unsigned char X10_sender::generateParity(unsigned int packet1, unsigned int packet2)
{
	unsigned char parity = 0;
	
	for (unsigned char i = 0; i < 16; i++)
	{
		parity = parity ^ ((packet1 >> i) & 1);
	}
	
	for (unsigned char i = 0; i < 4; i++)
	{
		parity = parity ^ ((packet2 >> i) & 1);
	}
	
	return parity;
		
}

//=============================================================
// METHOD : sendHigh
// DESCR. : 
//=============================================================
void X10_sender::sendHigh()
{
	if(getSendMode() == true)
	{
		//PORTB |= 1 << 1;
		if(getCurrentPacket() == 1)
		{
			if(getPacket1() & (1 << getCurrentPos1()))
			{
				start120();
				
				if(getCurrentPos1() > 0)
				decrementCurrentPos1();
				else
				setCurrentPacket(2);
				
				//PORTB |= 1 << 2;
			}
		else{}
	}
	
	else if(getCurrentPacket() == 2)
	{
		if(getPacket2() & (1 << getCurrentPos2()))
		{
			start120();
			
			if(getCurrentPos2() > 0)
			{
				decrementCurrentPos2();
			}
			else
			{
				setSendMode(false);
				setCurrentPacket(0);
			}
			
			//PORTB |= 1 << 2;
		}
	else{}
}
		else{}
	}
}

//=============================================================
// METHOD : sendLow
// DESCR. : 
//=============================================================
void X10_sender::sendLow()
{
	//PORTB &= ~(3 << 1);
	if(getSendMode() == true)
	{
		if(getCurrentPacket() == 1)
		{
			//PORTB |= 1 << 3;
			if(~getPacket1() & (1 << getCurrentPos1()))
			{
				start120();
				
				if(getCurrentPos1() > 0)
				decrementCurrentPos1();
				else
				setCurrentPacket(2);
				//PORTB |= 1 << 4;
			}
		else{}
	}
	
	else if(getCurrentPacket() == 2)
	{
		if(~getPacket2() & (1 << getCurrentPos2()))
		{
			start120();
			
			if(getCurrentPos2() > 0)
			{
				decrementCurrentPos2();
			}
			else
			{
				setSendMode(false);
				setCurrentPacket(0);
			}
			
			//PORTB |= 1 << 4;
		}
	else{}
}
		else{}	
	}
}