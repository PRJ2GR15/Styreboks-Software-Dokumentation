/*
 * StartUpSystem.cpp
 *
 * Created: 22-05-2016 18:41:55
 *  Author: Dennis Hornbæk Poulsen
 */ 

#include "StartUpSystem.h"
#include "codelock.h"


StartUpSystem::StartUpSystem() : code_()
{
	DDRB = 0xFF;					// udelukkende til brug af lysdoder for test af implementering, skal være 
	PORTB = 0;						// udelukkende til brug af lysdoder for test af implementering
}

bool StartUpSystem::requestPIN()
{
	return (code_.validatePIN() == true);		// If high input on PINA send true.
	