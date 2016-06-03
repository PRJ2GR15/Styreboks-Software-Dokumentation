//========================================================================
// FILENAME : PCinterface.h
// CREATED : 26-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : Header file for the ardiono - PC interface.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 26-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================

#ifndef PCINTERFACE_H_
#define PCINTERFACE_H_
#include "../Uart/uart.h";
#include "../RTC/rtc.h"
#include "../UnitHandler.h"

//=====================================
// CLASS : PCinterface
// DESCR. : represents the interface between the pc and the arduino using UART
//=====================================
class PCinterface
{
public:
	PCinterface(UnitHandler *Handler_obj, UART *uart_obj); // rtc skal tilføjes senere, er fjernet da den ikke virker.
	unsigned char getCmd(unsigned char bytes[]); // grabs the cmd char from the transmission if start byte is transmitted correctly.
	unsigned int getData(unsigned char data[]); // takes a 512 byte array as parameter.
	bool isStart( unsigned char bytes[]); // send in a 2 byte array;
	bool handleCMD();
	bool setTime();
	bool PCconnectionStatus;
private:
	rtc * rtc_obj_pointer;
	UnitHandler * UnitHandlerPointer;
	UART * uartPointer;
};



#endif /* PCINTERFACE_H_ */