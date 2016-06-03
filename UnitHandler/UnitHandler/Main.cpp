#define F_CPU 16000000

#include <avr/io.h>

#include "UART/uart.h"
#include "sd/sdCard.h"
#include "UnitHandler.h"
#include <util/delay.h>

int main()
{
	unsigned char input[512] = {0x00};
		
	InitUART(9600, 8, 0);
	sdCard SD_obj(4000);
	_delay_ms(10);
	SD_obj.init();
	UnitHandler unitHandling(&SD_obj);
	
	//Used to clear first 100 blocks before write operations begin..
	for (unsigned long i = 0; i<= 99; i++)
	{
		SD_obj.writeBlock(i,input);
	}
	
	//Start of test!!!
	
	//Create room!
	unitHandling.createNewRoom(0xAA);
	unitHandling.createNewRoom(0xBB);
	unitHandling.createNewRoom(0xCC);
	unitHandling.createNewRoom(0xDD);
	unitHandling.createNewRoom(0xEE);
	unitHandling.createNewRoom(0xFE);
	
	//Add units	
	unitHandling.AddUnit(0x01, 0xBB);
	unitHandling.AddUnit(0x02, 0xBB);
	unitHandling.AddUnit(0x03, 0xBB);
	unitHandling.AddUnit(0x04, 0xBB);
	unitHandling.AddUnit(0x05, 0xBB);
	unitHandling.AddUnit(0x06, 0xBB);
	
	//Remove a unit
 	unitHandling.RemoveUnit(0x03);
	 
	 //Add a new unit too see if tha data is filled in too the seven blocks from the unit deleted.
  	unitHandling.AddUnit(0x33, 0xAA);
	  
	//Remove unit from room (test if room list is updated)
	unitHandling.removeUnitFromRoom(0x04, 0xBB);
	
	//Delete a room (test if room list is updated, and all blocks for the units in that room is updated (remove roomID from the saved data to those units))
 	unitHandling.deleteRoom(0xAA);
	 
	//Create new room, to see if the room is placed in the space of the roomlist that is free from deleting the room beforehand.
 	unitHandling.createNewRoom(0x0E);
	 
	unitHandling.editUnit(0x02, 0x55, 0x66);
 
	//Test function UpdateTime()
	//First we fill an array with dummy data
	unsigned char timetable[512];
	
	for (int i = 0; i <= 511; i++)
	{
		timetable[i] = 0x44;
	}
	
	//Then we set unit specific data
	timetable[0] = 0x01;
	timetable[2] = 0xAA;
	timetable[3] = 0x01;
	
	//and write the block to the SD-card.
	unitHandling.UpdateTime(0x01, timetable);
	
	//Test function getTimeTable()
	//First we empty the array.
	for (int i = 0; i <= 511; i++)
	{
		timetable[i] = 0x00;
	}
	
	//Then fill the array with the specified unit, and day.
	unitHandling.getTimeTable(0x01, 0x01, timetable);
	
	//Output data via. UART, too make sure getTimeTable worked as expected.
	//Function worked, commented section to make sure arduino waste time tring to UART.
// 	for (int i = 0; i <= 511; i++)
// 	{
// 		SendChar(timetable[i]);
// 	}
}