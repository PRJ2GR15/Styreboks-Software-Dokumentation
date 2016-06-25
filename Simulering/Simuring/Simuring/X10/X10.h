/*
 * X10.h
 *
 * Created: 06-06-2016 15:12:15
 *  Author: Christian
 */ 


#ifndef X10_H_
#define X10_H_

#include "X10_modtager.h"
#include "X10_sender.h"

//=====================================
// CLASS  : X10
// DESCR. : Class for X10.1
//=====================================
class X10
{
public:
	X10(unsigned char houseCode, unsigned char adress);
	bool getUnitStatus(unsigned char unitID, bool& status);
	bool switchState(unsigned char unitID, bool newStatus);
	void recieveData();
	bool getSendMode();
	
	//Interupts
	void reciveSendHighInterupt(); //INT2_vect
	void reciveSendLowInterupt(); //INT3_vect
	void stop120Interupt(); //TIMER0_OVF_vect
	void resetReciverInterupt(); //TIMER3_OVF_vect
	void inputCompleteInterupt(); //TIMER2_OVF_vect

	
private:
	unsigned char houseCode_;
	bool unitStatus_;
	
	X10_modtager modtager_;
	X10_sender sender_;

	void switchLight(bool newStatus);
	void replyStatus(bool status);
	void replyOk();
};




#endif /* X10_H_ */