/*
 * StartUpSystem.h
 *
 * Created: 22-05-2016 18:37:51
 *  Author: Dennis Hornbæk Poulsen
 */ 


#ifndef STARTUPSYSTEM_H_
#define STARTUPSYSTEM_H_

#include <avr/io.h>
#include "codelock.h"

class StartUpSystem
{
public:
	StartUpSystem();
	bool requestPIN();
	
private:
	codelock code_;
};



#endif /* STARTUPSYSTEM_H_ */