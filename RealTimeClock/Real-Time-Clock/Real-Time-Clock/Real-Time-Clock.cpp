/*
 * Real_Time_Clock.cpp
 *
 * Created: 18-05-2016 09:24:02
 *  Author: Tonni
 */ 

#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include "i2c/i2c.h"
#include "RTC/rtc.h"
#include "UART/uart.h"
int main(void)
{
	InitUART(9600, 8, 0);
    rtc RTC_obj(0xD0, 400);
    RTC_obj.setTime(14, 37, 0);
    RTC_obj.setDate(18, 5, 16, 3);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	while(1)
    {
		SendChar(RTC_obj.getYear());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.getMonth());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.getDate());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.getDayOfWeek());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.getHours());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.getMinuts());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		SendChar(RTC_obj.GetSeconds());
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
        //TODO:: Please write your application code 
    }
}