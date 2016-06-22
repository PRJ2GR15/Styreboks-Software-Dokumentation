//========================================================================
// FILENAME  : main.cpp
// CREATED   : 13:04 27-05-2+16
// AUTHOR   : Stefan Nielsen
// DESCR.   : Simuleringsprogrammet for styreboks.
//
//------------------------------------------------------------------------
//
// REV - DATE - AUTHOR - CHANGE DESCRIPTION
// 1.0 27-05-2016/Stefan Nielsen Created project.
// 1.1 21/06-2016 Stefan Nielsen Updated project to switch case layout.
//========================================================================



#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART/uartdriver.h"
#include "SD/sdCard.h"
#include "UnitHandler/UnitHandler.h"
#include "PCinterface/PCinterface.h"
#include "RTC/rtc.h"
#include "X10/X10.h"


UART uart_obj(57600, 8, 'E');
sdCard SD_obj(4000);
rtc RTC_obj(0xD0, 400);
UnitHandler handler_obj(&SD_obj);
X10 X10_obj(0x00, 0x00);
PCinterface PCIF_obj(&handler_obj ,&uart_obj, &RTC_obj, &X10_obj);

int error_count = 0;
int unitCount;
unsigned char day;
unsigned char hour;
unsigned char minute;


int main()
{
	
	//Bruges som test program.
	RTC_obj.setDate(22, 06, 16, 3);
	RTC_obj.setTime(12, 00, 00);
	DDRF = 0xFF;
	
	
	
	//State-Machine implementation
	switch (PCIF_obj.PCconnectionStatus)
	{
		
		//If PC is connected.
		case true:
			PCIF_obj.returnStatus();
			break;
		
		
		
		//If PC is not connected.
		case false:
		
			unsigned char Array[512] = {0x00};
			unsigned char schedule[512] = {0x00};
		
			unitCount = handler_obj.getUnitCount();
			day = RTC_obj.getDayOfWeek();
			
				
			//Get list of units
			handler_obj.getUnitList(Array);
			
			//Search through the array for unitID´s 
			for (int i = 0; i <= 511; i++)
			{
				if (Array[1] % 2 == 0)
				{
					handler_obj.getTimeTable(day, Array[i], schedule);
					
					if (schedule[5] == RTC_obj.getHours())
					{
						if (schedule[6] == RTC_obj.getMinuts())
						{
							
							bool handling = false;
							if (schedule[7] =! 0)
							{
								handling = true;
							}
							
							
							
							//Bruges til test...
							if (handling == true)
							{
								PORTF = 0xFF;
							}
							else
							{
								PORTF = 0x00;
							}
							
							//Bruges ikke i test...
							//X10_obj.switchState(Array[i], handling);							
						}
					}					
				}				
			}			
			break;	
	}	
}




ISR (USART0_RX_vect) // interrupt based uart
{
	PCIF_obj.handleCMD();
}

// interrupts used by x10.h
ISR(INT2_vect) // used for X10.h
{
	X10_obj.reciveSendHighInterupt();
}
ISR(INT3_vect) // used for x10.h
{
	X10_obj.reciveSendLowInterupt();
}
ISR(TIMER0_OVF_vect)
{
	X10_obj.stop120Interupt();
}
ISR(TIMER3_OVF_vect)
{
	X10_obj.resetReciverInterupt();
}
ISR(TIMER2_OVF_vect)
{
	X10_obj.inputCompleteInterupt();
}

