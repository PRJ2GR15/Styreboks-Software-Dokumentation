//========================================================================
// FILENAME  : UnitHandler.h
// CREATED   : 11:07 24-05-2+16
// AUTHOR   : Stefan Nielsen
// DESCR.   : Class to handle all unit commands
//
//------------------------------------------------------------------------
//
// REV - DATE - AUTHOR - CHANGE DESCRIPTION
// 1.0 24-05-2016/Stefan Nielsen Created project.
//========================================================================

#ifndef UNITHANDLER_H_
#define UNITHANDLER_H_

#include "sd/sdCard.h"

#include "UART/uart.h"
//=====================================
// CLASS : UnitHandler
// DESCR : handles units, rooms, and unit-timetables.
//=====================================
class UnitHandler
{
public:
	UnitHandler(sdCard *SDObj);
	
	//unit handling
	bool AddUnit(unsigned char unitID, unsigned char roomID);
	bool RemoveUnit(unsigned char unitID);
	
	unsigned char getUnitCount();
	
	//Unit-list operations.
	void getUnitList(unsigned char Array[]);
	bool addUnitToList(unsigned char unitID, unsigned char address);
	bool removeUnitFromList(unsigned char unitID);
	
	//Room-list operations.
	void getRoomList(unsigned char Array[]);
	bool createNewRoom(unsigned char RoomID);
	bool deleteRoom(unsigned char RoomID);
	bool addUnitToRoom(unsigned char roomID);
	bool removeUnitFromRoom(unsigned char unitID, unsigned char roomID);
	
	//Timetable handling
	bool UpdateTime(unsigned char unitID, unsigned char schedule[]);
	bool getTimeTable(unsigned char day, unsigned char UnitID, unsigned char schedule[]);
	bool editUnit(unsigned char previusUnitID, unsigned char newUnitID, unsigned char roomID);

private:
	unsigned char searchFreeBlocks();
	bool checkStdAssignment();
	
	
	sdCard *SD;
	unsigned char numberOfUnits; // used to count units (up too 255) 
};

#endif /* UNITHANDLER_H_ */