/*
 * codelock.cpp
 *
 * Created: 22-05-2016 18:33:57
 *  Author: Dennis Hornbæk Poulsen
 */ 

#include "codelock.h"

codelock::codelock()
{
	DDRA = 0;										// PORTA set as input
}

bool codelock::validatePIN()
{	
	return ((PINA & 0b00000001) == 1);				// When input signal = logic '1', return true	
}