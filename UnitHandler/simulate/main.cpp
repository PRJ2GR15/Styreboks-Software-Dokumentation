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
//========================================================================

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Uart/uart.h"
#include "SD/sdCard.h"
#include "UnitHandler.h"
#include "PCinterface/PCinterface.h"
#include "RTC/rtc.h"
#include "X10_modtager.h"
#include "X10_sender.h"


int main(void)
{
	while (1) 
    {
		int error_count = 0;
		
		unsigned char Array;
		UnitHandler.getUnitList(Array);
		
		int unitCount = UnitHandler.getUnitCount();
		unsigned char day = rtc.getDayOfWeek();
		unsigned char hour = rtc.getHours();
		unsigned char minute = rtc.getMinuts();
		
		unsigned char data;
		unsigned char type;
		bool validity;
		unsigned char status;
		
		
		for (int i = 0; i <= unitCount; i++)
		{
			if (i % 2 == 1 && Array[i] != 0x00)
			{
				unsigned long blockAddress = ((2 + ((Array[i] - 1) * 7)) + day);
				sdCard.readBlock(blockAddress);
				
				
				for (int x = 5; x <= 511; x += 4)
				{
					if (Array[x] == hour)
					{
						if (Array[x + 1] == minute)
						{
							X10_sender.sendCommand(0x01, Array[1], 0x00);
							while(!X10_modtager.dataReady_)
							{}		
							
							X10_modtager.fetchData(type, data, validity);													
							
							
							if (type == 0x01 && validity == true)
							{
								status == data;
								
								if (status == Array[x + 2])
								{}
								else
								{
									X10_sender.sendCommand(0x01, Array[1], 0x02, Array[x + 2]);
									while(!X10_modtager.dataReady_)
									{}
									
									X10_modtager.fetchData(type, data, validity);
									
									if (type == 0x05 && validity == false)
									{
										error_count++;
									}
								}
							}
						}
					}
				}
			}
		}
    }
}

