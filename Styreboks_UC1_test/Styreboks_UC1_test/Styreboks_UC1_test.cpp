/*
 * Styreboks_UC1_test.cpp
 *
 * Created: 24-05-2016 10:40:44
 *  Author: denni
 */ 


#include <avr/io.h>
#include "StartUpSystem.h"

int main(void)
{
	
	
    while(1)
    {
		StartUpSystem startUp;
		
        if (startUp.requestPIN()==true)
        {
			PORTB = (PINB | 0b0000001);
        }
		
    }
}