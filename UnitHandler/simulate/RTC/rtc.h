//========================================================================
// FILENAME : rtc.h
// CREATED : 18-05-2016
// AUTHOR : Tonni Nybo Follmann
// DESCR. : header for the rtc driver module for the ds3231 rtc
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 08-04-2016/Tonni Nybo Follmann created the initial working version
//========================================================================
#include <avr/io.h>
#include "../i2c/i2c.h"

#ifndef RTC_H_
#define RTC_H_

//=====================================
// CLASS : rtc
// DESCR. : represents the real time clock module ds3231
//=====================================

class rtc
{
public:
	rtc(unsigned char adress, int clock);
	void setTime(unsigned char hours, unsigned char minuts, unsigned char seconds);
	void setDate(unsigned char date, unsigned char month, unsigned char year, unsigned char dayOfWeek);
// functions for getting current time
	int getHours();
	int getMinuts();
	int GetSeconds();

// functions for getting current date infomation
	int getDayOfWeek();
	int getDate();
	int getMonth();
	int getYear();

private:
unsigned char adress;
i2c i2c_obj;	
// helper functions for handling binary coded decimal
unsigned char intToBCD(unsigned char val);
int bcdToInt(unsigned char BCDval);
};


#endif /* RTC_H_ */