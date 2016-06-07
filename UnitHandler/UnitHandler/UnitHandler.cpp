//========================================================================
// FILENAME  : UnitHandler.cpp
// CREATED   : 11:07 24-05-2+16
// AUTHOR   : Stefan Nielsen
// DESCR.   : Class to handle all unit commands
//
//------------------------------------------------------------------------
//
// REV - DATE - AUTHOR - CHANGE DESCRIPTION
// 1.0 24-05-2016/Stefan Nielsen Created project.
//========================================================================

#include "UnitHandler.h"

//=============================================================
// METHOD : UnitHandler() Constructer
// DESCR. : Initializes the UnitHandler object, and sets the SDCard object.
//=============================================================
UnitHandler::UnitHandler(sdCard *SDObj)
{
	SD = SDObj;
	numberOfUnits = 0;
}

//=============================================================
// METHOD : AddUnit()
// DESCR. : Saves the Unit-info (with room) to the next free block on the SD card.
//=============================================================
bool UnitHandler::AddUnit(unsigned char unitID, unsigned char Room = 0x00)
{
	numberOfUnits++;
	//We calculate the standard address assignment from the parameter "numberOfUnits"
	//if a unit has been deleted, the number has been used before. So we check to see if
	//the standard assignment for this "numberOfUnits" is already in use.
	
	//If true, standard assignment hasn't been used.
	if (checkStdAssignment())
	{
		unsigned char data[512] = {0x00};
		
		// Set data specific to the unit.
		data[0] = unitID;
		data[1] = numberOfUnits;
		data[2] = Room;
		
		// For-loop makes sure to create 7 blocks for each unit.
		// One block represents one day of the week.
		for (unsigned char i = 1; i <= 7; i++)
		{
			// Set weekday (representated by a number, Monday to Sunday is 1 - 7)
			data[3] = i;
			
			//Calculate the starting point address (Standard assignment)
			unsigned long temp_address = (((2 + ((numberOfUnits - 1) * 7)) + i) - 1);
			
			//Loop to give each write operation 9 tries to succeed.
			for (int y = 1; y <= 9; y++)
			{
				if (SD->writeBlock(temp_address, data))
				{
					y = 10;			// if write attempt succeeds, leave loop...
				}
				else if (y = 9)
				{
					numberOfUnits--;
					return false;
				}
			}
		}
		//Add unit to the room and unit list.
		if (addUnitToRoom(Room))
		{
			return addUnitToList(unitID, numberOfUnits);
		}		
		else return false;
	}
	
	
	// If the standard assignment is already in use, we search to find out what unit has been deleted.
	// We then use temp to calculate the starting point address of the unit, and set byte1 in the block equal to temp
	else
	{
		unsigned char temp = searchFreeBlocks();
		unsigned char data[512] = {0x00};
		
		// Set data specific to the unit.
		data[0] = unitID;
		data[1] = temp;
		data[2] = Room;
		
		// For-loop makes sure to create 7 blocks for each unit.
		// One block represents one day of the week.
		for (unsigned char i = 1; i <= 7; i++)
		{
			// Set weekday (representated by a number, Monday to Sunday is 1 - 7)
			data[3] = i;
			
			//Calculate the starting point address (Standard assignment)
			unsigned long temp_address = (((2 + ((temp - 1) * 7)) + i) - 1);
			
			
			//Loop to give each write operation 9 tries to succeed.
			for (int y = 1; y <= 9; y++)
			{
				if (SD->writeBlock(temp_address, data))
				{
					y = 10;			// if write attempt succeeds, leave loop...
				}
				else if (y = 9)
				{
					numberOfUnits--;
					return false;
				}
			}
		}
		//Add unit to the unit list.
		if (addUnitToRoom(Room))
		{
			return addUnitToList(unitID, temp);
		}
		else return false;
	}
}

//=============================================================
// METHOD : RemoveUnit()
// DESCR. : Delete a Unit, by erasing data in the blocks that represents that unit.
//=============================================================
bool UnitHandler::RemoveUnit(unsigned char unitID)
{
	unsigned char empty_block[512] = {0x00};
	unsigned char start_block;
	unsigned char next_block;
	unsigned char spot;
	unsigned char temp[512] = {0x00};
	unsigned char temp2[512] = {0x00};
	getUnitList(temp);
	
	//Search unitList for the position of the specified unit.
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 != 0)
		{
			if (unitID == temp[i])
			{
				int y = temp[i] - 1;
				start_block = (2 + (y * 7));		
			}
		}
	}
	
	SD->readBlock(start_block, temp2);
	
	if (temp2[2] != 0x00)
	{
		unsigned char roomList[512] = {0x00};
		getRoomList(roomList);
		
		for (int i = 0; i <= 511; i++)
		{
			if (i % 2 == 0 && roomList[i] == temp2[2])
			{
				spot = i;
				unsigned char count = spot + 1;
				roomList[count] -= 1;
				SD->writeBlock(1, roomList);
			}
		}
	}
	
	//Loop to overwrite all 7 blocks that represent the unit.
	for (int i = 0; i <= 6; i++)
	{
		next_block = start_block + i;
		
		//Loop to give each write operation 3 tries to succeed.
		for (int y = 1; y <= 3; y++)
		{
			if (SD->writeBlock(next_block, empty_block))
			{
				y = 4;			// if write attempt succeeds, leave loop...
			}
			else if (y = 3)
			{
				return false;
			}
		}
	}
	
	numberOfUnits--;
	
	return removeUnitFromList(unitID);
}

//=============================================================
// METHOD : chechStdAssignment()
// DESCR. : Checks to see if the standard address assignment is already in use.
//			Returns false if stdAssignment is already in use, true if stdAssignment is free....
//=============================================================
bool UnitHandler::checkStdAssignment()
{
	unsigned char unitList[512];
	getUnitList(unitList);
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 != 0 && numberOfUnits  == unitList[i])
		{
			return false;
		}
	}
	
	return true;
}

//=============================================================
// METHOD : getUnitList()
// DESCR. : Fill array with the unit list (block 0) from the SD card.
//=============================================================
void UnitHandler::getUnitList(unsigned char Array[])
{	
	SD->readBlock(0, Array);
}

//=============================================================
// METHOD : addUnitToList()
// DESCR. : Finds the next free spot on the list, and adds the new unit to the list.
//=============================================================
bool UnitHandler::addUnitToList(unsigned char unitID, unsigned char address)
{
	unsigned char temp_list[512];
	int spot = 0;
	
	getUnitList(temp_list);
	
	for (int i = 0; i <= 511; i++)
	{
		if (temp_list[i] == 0)
		{
			spot = i;
			i = 512;
		}
	}

	if (spot == 0 && temp_list[spot] != 0)
	{
		return false;
	}
	
	temp_list[spot] = unitID;
	spot++;
	temp_list[spot] = address;
		
	//Write new list to SD
	for (int y = 1; y <= 3; y++)
	{
		if (SD->writeBlock(0, temp_list))
		{
			y = 4;			// if write attempt succeeds, leave loop...
		}
		else if (y = 3)
		{
			return false;
		}
	}
	return true;	
}

//=============================================================
// METHOD : removeUnitFromList()
// DESCR. : Finds the unit specified, deletes the info and compresses the block.
//=============================================================
bool UnitHandler::removeUnitFromList(unsigned char unitID)
{
	unsigned char temp_list[512] = {0x00};
	unsigned char spot = 0x00;
	
	getUnitList(temp_list);
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0)
		{
			if (temp_list[i] == unitID)
			{
				spot = i;
				i = 512;
			}
		}
	}
	unsigned char newList[512] = {0x00};
		
		
	for (int i = 0; i <= 511; i++)
	{
		if (i + 2 < 511)
		{
			if (i == spot | i > spot)
			{
				newList[i] = temp_list[i + 2];
			}
			else
			{
				newList[i] = temp_list[i];
			}
		}
		else {
			temp_list[i] = 0x00;
		}
	}
	//Write new list to SD
	for (int y = 1; y <= 9; y++)
	{
		if (SD->writeBlock(0, newList))
		{
			y = 10;			// if write attempt succeeds, leave loop...
		}
		else if (y = 9)
		{
			return false;
		}
	}
	return true;
}

//=============================================================
// METHOD : searchFreeBlocks()
// DESCR. : Searches for the blocks made free from deleting a unit,
//          returns a value that is used to calculate starting address for free blocks.
//=============================================================
unsigned char UnitHandler::searchFreeBlocks()
{
	unsigned char unitList[512];
	//Get list of known units, and their starting point (representation) number.
	getUnitList(unitList);
	
	for (unsigned char x = 1; x <= numberOfUnits; x++)
	{
		for (int i = 1; i <= 511; i++)
		{
			if (i%2 != 0)
			{
				if (unitList[i] == x)
				{
					i = 512;
				}
				else if (i == 511)
				{
					return x;	
				}
			}
		}
	}
}

//=============================================================
// METHOD : getRoomList()
// DESCR. : Fill array with the roomlist (block 1) from the SD-card.
//=============================================================
void UnitHandler::getRoomList(unsigned char Array[])
{
	SD->readBlock(1, Array);
}

//=============================================================
// METHOD : createNewRoom()
// DESCR. :
//=============================================================
bool UnitHandler::createNewRoom(unsigned char RoomID)
{
	unsigned char roomList[512] = {0x00};
	getRoomList(roomList);
	
	int spot = 0;
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0 && roomList[i] == 0)
		{
			spot = i;
			i = 512;
		}
	}
	if (spot == 0 && roomList[spot] != 0)
	{
		return false;
	}
	else
	{
		roomList[spot] = RoomID;
		
		//Write new list to SD
		for (int y = 1; y <= 9; y++)
		{
			if (SD->writeBlock(1, roomList))
			{
				y = 10;			// if write attempt succeeds, leave loop...
			}
			else if (y = 9)
			{
				return false;
			}
		}
		return true;
	}
}

//=============================================================
// METHOD : deleteRoom()
// DESCR. : Deletes the specified unit and unit count for that room, and removes the roomID from the units belonging to that room
//=============================================================
bool UnitHandler::deleteRoom(unsigned char RoomID)
{
	unsigned char temp_list[512] = {0x00};
	unsigned char spot = 0x00;
	unsigned char count = 0x00;
	unsigned char unitsHandled = 0x00;
	unsigned long blockPoint;
	unsigned char data[512];
	
	getRoomList(temp_list);
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0)
		{
			spot = temp_list[i];
			
			if (RoomID == spot)
			{
				//Saves the number of units belonging to that room.
				count = temp_list[i + 1];
				//Set unit count for that room equal too 0.
				temp_list[i + 1] = 0x00;

				//shift the register (list) to overwrite the room we want to delete.
				for (int x = 2; (x + i) <= 511; x++)
				{
					temp_list[i] = temp_list[x + i];
				}
				
				//Set i out of range, too stop loop
				i = 512;
			}
		}
	}
	
	//Write new list to SD
	for (int y = 1; y <= 3; y++)
	{
		if (SD->writeBlock(1, temp_list))
		{
			y = 4;			// if write attempt succeeds, leave loop...
		}
		else if (y = 3)
		{
			return false;
		}
	}
	
	//Search through the SD-card to find units belonging to the specified room, and delete room.
	for (int i = 0; i <= numberOfUnits; i++)
	{
		blockPoint = ((2 + (i * 7)) + 1);
		
		SD->readBlock(blockPoint, data);
		
		if (data[2] == RoomID)
		{
			removeUnitFromRoom(data[0], RoomID);
			unitsHandled++;
			if (unitsHandled == count)
			{
				i = numberOfUnits + 1;
			}
		}		
	}
	return true;
}

//=============================================================
// METHOD : addUnitToRoom()
// DESCR. : increment the count for the room in the roomlist.
//=============================================================
bool UnitHandler::addUnitToRoom(unsigned char roomID)
{
	unsigned char temp_list[512];
	unsigned char spot = 0;
	
	getRoomList(temp_list);
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0 && temp_list[i] == roomID)
		{
			spot = i;
			unsigned char count = spot + 1;
			temp_list[count] += 1;
		}
	}
	
	//Write new list to SD
	for (int y = 1; y <= 9; y++)
	{
		if (SD->writeBlock(1, temp_list))
		{
			y = 10;			// if write attempt succeeds, leave loop...
		}
		else if (y = 9)
		{
			return false;
		}
	}
	return true;
}

//=============================================================
// METHOD : removeUnitFromRoom()
// DESCR. : decrement the count for the room in the roomlist.
//=============================================================
bool UnitHandler::removeUnitFromRoom(unsigned char unitID, unsigned char roomID)
{
	unsigned char temp_list[512] = {0x00};
	getRoomList(temp_list);
	
	//Search for the room, and decrement room count with 1.
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0)
		{
			if (roomID == temp_list[i])
			{
				temp_list[i + 1] = temp_list[i + 1] - 1;
				i = 512;
			}
		}
	}
	
	//Write new list to SD
	for (int y = 1; y <= 9; y++)
	{
		if (SD->writeBlock(1, temp_list))
		{
			y = 10;			// if write attempt succeeds, leave loop...
		}
		else if (y = 3)
		{
			return false;
		}
	}
	
	unsigned char temp_list2[512] = {0x00};
	getUnitList(temp_list2);
	unsigned char start_block = 0x00;
		
	//Find and calculate the starting block of the specified unit.
	for (int x = 0; x <= 511; x++)
	{
		if (x % 2 == 0 && temp_list2[x] == unitID)
		{
			start_block = (2 + ((temp_list2[x + 1] - 1) * 7));
			x = 512;
		}
	}
	
	unsigned long adress_block = 0x00000000;
	unsigned char newList[512] = {0x00};
	
	for (int z = 0; z <= 6; z++)
	{
		adress_block = start_block + z;
		
		SD->readBlock(adress_block, newList);
		
		newList[2] = 0x00;
		
		SD->writeBlock(adress_block, newList);
}


	return true;
}

// =============================================================
// METHOD : UpdateTime()
// DESCR. : Sends updated timetable for one specified unit, on one specified day
// =============================================================
bool UnitHandler::UpdateTime(unsigned char unitID, unsigned char schedule[])
{
	unsigned char tempList[512] = {0x00};
	getUnitList(tempList); 
	
	unsigned long unit_start_block = 0x00000000;
	unsigned char block_representation;
	
	// looping to find storage location
	for (int i = 0; i <= 511; i++) 
	{
		if (i % 2 == 0)
		{
			if (tempList[i] == unitID)
			{
				// set j equal to the starting block representation for the specified unit.
				int j = i + 1;
				
				// extracting unit count for later use;
				block_representation = tempList[j];  
				
				// calculate start block
				int y = tempList[j] - 1;
				unit_start_block = (2 + (y * 7)); 
				i = 512;
			}
		}
	}
	
	// extract day from schedule array
	unsigned char day = schedule[3];
	
	unsigned long daylocation = 0x00000000;
	
	// testing if day is within legal range
	if (day < 8 && day > 0) 
	{
		// calculate the block location of the block we use.
		daylocation = unit_start_block + (day - 1); 
	}
	else
	{
		// if not within range of 1 to 7 return false
		return false;
	}
	
	// add correct starting block representation, to the new array.
	schedule[1] = block_representation;
	
	//Loop to give each write operation 9 tries to succeed.
	for (int i = 0; i < 9; i++) 
	{
		if (SD->writeBlock(daylocation, schedule))
		{
			i = 10;		//If write attempt succeeds, leave loop.
			return true;
		}
	}
	// write failed more than 9 times.
	return false; 
}

// =============================================================
// METHOD : getTimeTable()
// DESCR. : fills a 512 byte array with the data in the block for the requested day and unit.
// =============================================================
bool UnitHandler::getTimeTable(unsigned char day, unsigned char UnitID, unsigned char schedule[]) 
{
	unsigned char tempList[512] = {0x00};
	getUnitList(tempList);
	
	unsigned long unit_start_block = 0x00000000; // address storage
	
	// loop to find the value used for calculating the storage adress;
	for (int i = 0; i < 512; i++) 
	{
		if (i % 2 == 0)
		{
			if (tempList[i] == UnitID)
			{
				int j = i + 1;
				int y = tempList[j] - 1;
				unit_start_block = (2 + (y * 7));
				i = 512;
			}
		}
	}
	
	unsigned long daylocation = 0x00000000;
	if (day < 8 && day > 0)
	{
		daylocation = unit_start_block + (day - 1);
	}
	else
	{
		return false;
	}
	for (int i = 0; i < 9; i++)
	{
		if (SD->readBlock(daylocation, schedule))
		{
			i = 10;
			return true;
		}
	}
	return false;
}

// =============================================================
// METHOD : editUnit()
// DESCR. : fills a 512 byte array with the data in the block for the requested day and unit.
// =============================================================
bool UnitHandler::editUnit(unsigned char previusUnitID, unsigned char newUnitID, unsigned char roomID)
{
	unsigned char data[512];
	unsigned char data2[512];
	
	unsigned char point = 0x01;
	
	
	getUnitList(data);
	
	//Fint the starting point representation of the unit we want too update.
	for (int i = 0; i <= 511; i++)
	{
		if (data[i] == previusUnitID)
		{
			point = data[i + 1];
			data[i] = newUnitID;
			SD->writeBlock(0x00, data);
			i = 512;
		}
	}
	
	
	
	unsigned long address = (2 + ((point - 1) * 7));
	
	SD->readBlock(address, data2);
	unsigned char oldRoom = data2[2];
	
	getRoomList(data);
	
	for (int i = 0; i <= 511; i++)
	{
		if (i % 2 == 0)
		{
			if (data[i] == oldRoom)
			{
				int j = i + 1;
				
					if (data[j] != 0x00)
					{
						data[j]--;
					}
				i = 512;
			}
		}
	}
	
	bool roomExists = false;
	for (int i = 0; i<= 511; i++) // looping to see if new room allready exists
	{
		if(i % 2 == 0)
		{
			if(data[i] == roomID)
			{
				data[i + 1] = data[i + 1] + 1; // adds 1 to the unit count in new room
				roomExists = true;
				i = 512; // exit loop
			}
		}
	}
	
	SD->writeBlock(0x01, data); 
	
	// create room and add unit if room does not exits
	if(roomExists == false)
	{ 
		createNewRoom(roomID);
		addUnitToRoom(roomID);
	}
	

	for (int i = 0; i <= 6; i++)
	{
		SD->readBlock(address, data2);
		data2[0] = newUnitID;
		data2[2] = roomID;
		SD->writeBlock(address, data2);
		address++;
	}

}

// =============================================================
// METHOD : getUnitCount()
// DESCR. : returns numbers of units.
// =============================================================
unsigned char UnitHandler::getUnitCount()
{
	return numberOfUnits;
}