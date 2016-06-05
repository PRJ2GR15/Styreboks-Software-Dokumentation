//========================================================================
// FILENAME : rtc.cpp
// CREATED : 18-05-2016
// AUTHOR : Tonni Nybo Follmann
// DESCR. : implementation of the rtc driver module for the ds3231 rtc
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 08-04-2016/Tonni Nybo Follmann created the initial working version
//========================================================================
#include "rtc.h"
//=====================================
// CLASS : rtc
// DESCR. : represents the real time clock module ds3231
//=====================================

//=============================================================
// METHOD : rtc
// DESCR. : constructor that initiates the rtc module and sets the adress to adress,
// as well as initializing the i2c to a requested frequency which must be either 100 
// or 400 kHz
//=============================================================
rtc::rtc(unsigned char adress, int clock) : i2c_obj(1, clock)
{
	 this->adress = adress;
}
//=============================================================
// METHOD : setTime
// DESCR. : sets up the RTC to the time passed through as parameters
//=============================================================
void rtc::setTime( unsigned char hours, unsigned char minuts, unsigned char seconds )
{
	// to write set the direction bit to 0 
	// module adress is hard coded to 1101000 + direction bit
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE); // masking adress to set the direction bit to 0
	i2c_obj.write(0x00);
	i2c_obj.write(this->intToBCD(seconds));
	i2c_obj.write(this->intToBCD(minuts));
	i2c_obj.write((this->intToBCD(hours) | 0x40)); // masking to set the clock to 24 hours.
	i2c_obj.stop();
}

//=============================================================
// METHOD : setTime
// DESCR. : sets up the RTC to the date info passed through as parameters.
//=============================================================
void rtc::setDate( unsigned char date, unsigned char month, unsigned char year, unsigned char dayOfWeek )
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE); // masking adress to set the direction bit to 0
	i2c_obj.write(0x03);
	i2c_obj.write(dayOfWeek);
	i2c_obj.write(this->intToBCD(date));
	i2c_obj.write(this->intToBCD(month));
	i2c_obj.write(this->intToBCD(year));
	i2c_obj.stop();
}
//=============================================================
// METHOD : getHours
// DESCR. : returns current hours from the RTC module
//=============================================================
int rtc::getHours()
{
	// to read set the direction bit to 1
	// module adress is hard coded to 1101000 + direction bit
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE); // set direction to write
	i2c_obj.write(0x02); // setting the adress pointer of the RTC to the memory adress we want to read
	i2c_obj.stop();
	
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01); // set direction to read
	unsigned char currentHours = i2c_obj.read(1); // read 1 byte of data
	i2c_obj.stop();
	
	currentHours = currentHours & 0xBF; // masking out the 24 hour bit.
	return bcdToInt(currentHours);
}
//=============================================================
// METHOD : getMinuts
// DESCR. : returns current minuts from the RTC module
//=============================================================
int rtc::getMinuts()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE); // set direction to write
	i2c_obj.write(0x01); // set the adress pointer on RTC to the memory adress we want to read
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01); // set to read
	unsigned char currentMinuts = i2c_obj.read(1); // read 1 byte
	i2c_obj.stop();
		
	return bcdToInt(currentMinuts);
}
//=============================================================
// METHOD : getSeconds
// DESCR. : returns current seconds from the RTC module
//=============================================================
int rtc::GetSeconds()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE);
	i2c_obj.write(0x00);
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01);
	unsigned char currentSeconds = i2c_obj.read(1);
	i2c_obj.stop();
		
	return bcdToInt(currentSeconds);
}
//=============================================================
// METHOD : getDayOfWeek
// DESCR. : returns current Day of the week from the RTC module
//=============================================================
int rtc::getDayOfWeek()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE);
	i2c_obj.write(0x03);
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01);
	unsigned char currentDay = i2c_obj.read(1);
	i2c_obj.stop();
		
	return currentDay;
}
//=============================================================
// METHOD : getDate
// DESCR. : returns current minuts from the RTC module
//=============================================================
int rtc::getDate()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE);
	i2c_obj.write(0x04);
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01);
	unsigned char currentDate = i2c_obj.read(1);
	i2c_obj.stop();
		
	return bcdToInt(currentDate);
}
//=============================================================
// METHOD : getMonth
// DESCR. : returns current month from the RTC module
//=============================================================
int rtc::getMonth()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE);
	i2c_obj.write(0x05);
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01);
	unsigned char currentMonth = i2c_obj.read(1);
	i2c_obj.stop();
		
	return bcdToInt(currentMonth);
}
//=============================================================
// METHOD : getYear
// DESCR. : returns current Year from the RTC module
//=============================================================
int rtc::getYear()
{
	i2c_obj.start();
	i2c_obj.write(this->adress & 0xFE);
	i2c_obj.write(0x06);
	i2c_obj.stop();
		
	i2c_obj.start();
	i2c_obj.write(this->adress | 0x01);
	unsigned char currentYear = i2c_obj.read(1);
	i2c_obj.stop();
		
	return bcdToInt(currentYear);
}
//=============================================================
// METHOD : intToBCD
// DESCR. : converts an 8 bit unsigned value to the binary coded decimal format
//=============================================================
unsigned char rtc::intToBCD( unsigned char val )
{
	unsigned char tens = val/10 << 4; // divide by 10 bitshift for places.
	unsigned char ones = val % 10; // get the reminder of a devision by 10
	return tens + ones; // add the numbers to gain the BCD value
}
//=============================================================
// METHOD : bcdToInt
// DESCR. : converts from 8 bit binary coded decimal to int.
//=============================================================
int rtc::bcdToInt( unsigned char BCDval )
{
	unsigned char ones = (BCDval & 0x0F); // mask out the tens
	unsigned char tens = ((BCDval & 0xF0) >> 4)*10; // mask out the ones and bitshift 4 places and multiplicate by 10
	return ones + tens; // add the numbers to get the integer value.
}












 