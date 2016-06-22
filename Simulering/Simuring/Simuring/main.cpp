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

bool handling;
int unitCount;
unsigned char day;


int main()
{
	
	sei();
	SD_obj.init();
	
	//Bruges som test program.
	RTC_obj.setDate(22, 06, 16, 3);
	RTC_obj.setTime(12, 00, 00);
	DDRF = 0xFF;
	
	
	while (1)
	{

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
					if (Array[i] % 2 != 0)
					{
						handler_obj.getTimeTable(day, Array[i - 1], schedule);
					
						
						for (int j = 5; j<= 511; j += 4)
						{
							if (schedule[j] == RTC_obj.getHours())
							{
							
								if (schedule[j + 1] == RTC_obj.getMinuts())
								{
							
									handling = schedule[j+2];			
								
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
									// men skal bruges i demo.
									//X10_obj.switchState(Array[i-1], handling);							
								}
							}
						}
					}				
				}			
				break;
		}
	}	
}




ISR (USART0_RX_vect) // interrupt based uart
{
	PCIF_obj.handleCMD();
	sei();
}

// interrupts used by x10.h
ISR(INT2_vect) // used for X10.h
{
	X10_obj.reciveSendHighInterupt();
	sei();
}
ISR(INT3_vect) // used for x10.h
{
	X10_obj.reciveSendLowInterupt();
	sei();
}
ISR(TIMER0_OVF_vect)
{
	X10_obj.stop120Interupt();
	sei();
}
ISR(TIMER3_OVF_vect)
{
	X10_obj.resetReciverInterupt();
	sei();
}
ISR(TIMER2_OVF_vect)
{
	X10_obj.inputCompleteInterupt();
	sei();
}

