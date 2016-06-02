/*
 * StartUpSystem.cpp
 *
 * Created: 22-05-2016 18:41:55
 *  Author: Dennis Hornb�k Poulsen
 */ 

#include "StartUpSystem.h"
#include "codelock.h"


StartUpSystem::StartUpSystem() : code_()
{
	DDRB = 0xFF;					// udelukkende til brug af lysdoder for test af implementering, skal v�re 
	PORTB = 0;						// udelukkende til brug af lysdoder for test af implementering
}

bool StartUpSystem::requestPIN()
{
	return (code_.validatePIN() == true);		// If high input on PINA send true.
	