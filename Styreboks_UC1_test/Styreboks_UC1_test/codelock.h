/*
 * codelock.h
 *
 * Created: 22-05-2016 18:31:00
 *  Author: Dennis Hornbæk Poulsen
 */ 

#include <avr/io.h>


#ifndef CODELOCK_H_
#define CODELOCK_H_

class codelock
{
public:
	codelock();
	bool validatePIN();
};




#endif /* CODELOCK_H_ */