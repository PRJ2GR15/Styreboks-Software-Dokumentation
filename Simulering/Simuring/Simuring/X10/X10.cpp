/*
 * X10.cpp
 *
 * Created: 06-06-2016 15:12:01
 *  Author: Christian
 */ 


#include "X10.h"
#include <avr/io.h>
#include <avr/interrupt.h>


//=============================================================
// METHOD :
// DESCR. :
//=============================================================
X10::X10(unsigned char houseCode, unsigned char adress) : modtager_(houseCode, adress)
{
	houseCode_ = houseCode;
	unitStatus_ = true;
	//timer
	TCNT4 = 34286;
	
	
	//int 2
	EIMSK |= (1 << INT2);
	EICRA |= (1 << ISC21);
	EICRA |= (1 << ISC20);
	
	//int 3
	EIMSK |= (1 << INT3);
	EICRA |= (1 << ISC31);
	
	//if(adress != 0)
	//	switchLight(unitStatus_);
	
	
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
bool X10::getUnitStatus(unsigned char unitID, bool& status)
{
	sender_.sendCommand(houseCode_, unitID, 0b000);
	
	while(getSendMode() == true){}
	
	TCCR4B |= (1 << CS42) | (1 << CS40); //start timer
	
	while(modtager_.isDataReady() != true)
	{
		if(TIFR4 & (1 << TOV4))
		{
			TIFR4 |= (1 << TOV4); //stop timer
			TCCR4B &= ~((1 << CS42) | (1 << CS40));
			TCNT4 = 34286;
			//PORTB |= (1 << PINB6);
			return false;
		}
	}
	TCCR4B &= ~((1 << CS42) | (1 << CS40)); //stop timer
	TCNT4 = 34286;
	
	unsigned char type;
	unsigned char data;
	bool vali;
	modtager_.fetchData(type, data, vali);
	if(vali == false)
		return false;
	else
	{
		if(type == 0b001)
		{
			if(data == 0b0)
			{
				status = false;
				return true;
			}					
			else if(data == 0b1)
			{
				status = true;
				return true;
			}		
			else
				return false;				
		}		
		else
			return false;		
	}
}	

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
bool X10::switchState(unsigned char unitID, bool newStatus)
{
	if(newStatus == true)
	{
		sender_.sendCommand(houseCode_, unitID, 0b010, 0b1);
	}
	else
	{
		sender_.sendCommand(houseCode_, unitID, 0b010, 0b0);
	}
	
	while(getSendMode() == true){}
	
	TCCR4B |= (1 << CS42) | (1 << CS40); //start timer
	
	while(modtager_.isDataReady() != true)
	{
		if(TIFR4 & (1 << TOV4))
		{
			TIFR4 |= (1 << TOV4); //stop timer
			TCCR4B &= ~((1 << CS42) | (1 << CS40));
			TCNT4 = 34286;
			//PORTB |= (1 << PINB6);
			return false;
		}
	}
	TCCR4B &= ~((1 << CS42) | (1 << CS40)); //stop timer
	TCNT4 = 34286;
	
	unsigned char type;
	unsigned char data;
	bool vali;
	modtager_.fetchData(type, data, vali);
	if(vali == false)
		return false;
		
	else
	{
		if(type == 0b011)
			return true;	
							
		else
			return false;		
	}
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::replyStatus(bool status)
{
	if (status == true)
		sender_.sendCommand(houseCode_, 0, 0b001, 0b1);
		
	else
		sender_.sendCommand(houseCode_, 0, 0b001, 0b0);
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::replyOk()
{
	sender_.sendCommand(houseCode_, 0, 0b011);
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::recieveData()
{
	if(modtager_.isDataReady())
	{
		unsigned char type;
		unsigned char data;
		bool vali;
		modtager_.fetchData(type, data, vali);
		if(vali == false)
		{
			//sender_.sendCommand(houseCode_, 0, 0b111, 0b00);
		}			
		else
		{
			if(type == 0b000)
			{
				replyStatus(unitStatus_);
			}
			else if(type == 0b010)
			{
				if(data == 0)
				{
					unitStatus_ = false;
				}
				else
				{
					unitStatus_ = true;
				}
				switchLight(unitStatus_);
				replyOk();
			}
			else
			{
				//sender_.sendCommand(houseCode_, 0, 0b111, 0b01);
			}
		}
	}
	else{}
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::switchLight(bool newState)
{
	if(newState == true)
	{
			PORTB |= (1 << PINB3);
			PORTB &= ~(1 << PINB2);
	}
	else
	{
			PORTB |= (1 << PINB2);
			PORTB &= ~(1 << PINB3);
	}
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
bool X10::getSendMode()
{
	return sender_.getSendMode();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::reciveSendHighInterupt()
{
	if(getSendMode() == true)
		sender_.sendHigh();
	else
		modtager_.receiveHigh();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::reciveSendLowInterupt()
{
	if(sender_.getSendMode() == true)
		sender_.sendLow();
	else
		modtager_.receiveLow();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::stop120Interupt()
{
	sender_.stop120();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::resetReciverInterupt()
{
	modtager_.resetReceiver();
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::inputCompleteInterupt()
{
	modtager_.inputComplete();
}