/*
 * X10.cpp
 *
 * Created: 06-06-2016 15:12:01
 *  Author: Christian
 */ 


#include "X10.h"
#include <avr/io.h>


//=============================================================
// METHOD :
// DESCR. :
//=============================================================
X10::X10(unsigned char houseCode, unsigned char adress) : modtager_(houseCode, adress)
{
	houseCode_ = houseCode;
	unitStatus_ = false;
	//timer
	TCNT4 = 34286;
	
	
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
bool X10::getUnitStatus(unsigned char unitID, bool& status)
{
	sender_.sendCommand(houseCode_, unitID, 0b000);
	
	TCCR4B |= (1 << CS42) | (1 << CS40); //start timer
	
	while(modtager_.isDataReady() != false)
	{
		if(TIFR4 & (1 << TOV4))
		{
			TIFR4 |= (1 << TOV4); //stop timer
			TCCR4B &= ~((1 << CS42) | (1 << CS40));
			TCNT4 = 34286;
			return false;
		}
	}
	TCCR4B &= ~((1 << CS42) | (1 << CS40)); //stop timer
	TCNT4 = 34286;
	
	unsigned char type, data;
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
	sender_.sendCommand(houseCode_, unitID, 0b010, (newStatus == true ? 0b1 : 0b0));
	
	TCCR4B |= (1 << CS42) | (1 << CS40); //start timer
	
	while(modtager_.isDataReady() != false)
	{
		if(TIFR4 & (1 << TOV4))
		{
			TIFR4 |= (1 << TOV4); //stop timer
			TCCR4B &= ~((1 << CS42) | (1 << CS40));
			TCNT4 = 34286;
			return false;
		}
	}
	TCCR4B &= ~((1 << CS42) | (1 << CS40)); //stop timer
	TCNT4 = 34286;
	
	unsigned char type, data;
	bool vali;
	modtager_.fetchData(type, data, vali);
	if(vali == false)
		return false;
		
	else
	{
		if(type == 0b101)
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
	sender_.sendCommand(houseCode_, 0, 0b001, (status == true ? 0b1 : 0b0));
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::replyOk()
{
	sender_.sendCommand(houseCode_, 0, 0b101);
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
			sender_.sendCommand(houseCode_, 0, 111, 0b00);
		}			
		else
		{
			if(type == 0b000)
			{
				replyStatus(unitStatus_);
			}
			else if(type == 0b010)
			{
				switchLight((data == 0b0 ? false : true));
				replyOk();
			}
			else
				sender_.sendCommand(houseCode_, 0, 111, 0b01);
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
	//dummy
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::reciveSendHighInterupt()
{
	if(sender_.getSendMode())
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
	if(sender_.getSendMode())
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
	PORTB |= 1;
}

//=============================================================
// METHOD :
// DESCR. :
//=============================================================
void X10::inputCompleteInterupt()
{
	modtager_.inputComplete();
}