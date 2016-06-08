//========================================================================
// FILENAME : PCinterface.cpp
// CREATED : 26-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : cpp file for the ardiono - PC interface.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 26-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================
#include "PCinterface.h"
//=====================================
// CLASS : PCinterface
// DESCR. : represents the interface between the arduino and the PC
//=====================================

//=============================================================
// METHOD : PCinterface
// DESCR. : constructor, initalizes the pc interface, takes 3 object pointers 
// to the drivers needed.
//=============================================================
 PCinterface::PCinterface( UnitHandler *Handler_obj, UART *uart_obj, rtc *rtc_obj )
{
	uartPointer = uart_obj;
	rtc_obj_pointer = rtc_obj;
	UnitHandlerPointer = Handler_obj;
	
}

//=============================================================
// METHOD : isStart
// DESCR. : tests for the start command on the uart connection
//=============================================================
bool PCinterface::isStart( unsigned char bytes[] )
{
	bytes[0] = uartPointer->readChar();
	bytes[1] = uartPointer->readChar();
	
	if (bytes[0] == 0xF0 && bytes[1] == 0xF0){
		return true;
	} 
	else
	{
		return false;
	}
}
//=============================================================
// METHOD : getCmd
// DESCR. : gets the command recieved from the PC and returns it.
//=============================================================

unsigned char PCinterface::getCmd(unsigned char bytes[2])
{
	
	if(isStart(bytes))
	{
		unsigned char cmd = uartPointer->readChar();
		if(cmd > 0 && cmd < 0x0B){
			//uartPointer->sendChar(0x0F);
		} 
		else
		{
			return 0;
		}
		return cmd;
	}
}
//=============================================================
// METHOD : getData
// DESCR. : recieves data on UART following a commmand, fills it
// into the array recieved and returns the
// number of data bytes added to the array.
//=============================================================
unsigned int PCinterface::getData( unsigned char data[] ) // array must be 512 bytes.
{
	for (int i = 0; i < 512; i++){
		data[i] = 0x00;
	}
	unsigned int databytes = 0;
	unsigned char byte = 0;
	unsigned char byte2 = 0;
	for( int i = 0; i<512; i++){
		byte = uartPointer->readChar();	
		uartPointer->sendChar(0x0F); // transmit ACK;
		if(byte == 0x0F)
		{	
			
			byte2 = uartPointer->readChar();
			uartPointer->sendChar(0x0F); // transmit ACK;
			if(byte2 == 0x0F){
				return databytes;
			} 
			else 
			{
				data[i] = byte;
				databytes++;
				i++;
				data[i] = byte2;
				databytes++;
			}
		}
		else 
		{
			data[i] = byte;
			databytes++;
		}
	}
}

//=============================================================
// METHOD : handleCMD
// DESCR. : The function doing all the hard work ;)
//=============================================================
bool PCinterface::handleCMD()
{
	DDRA = 0x00000000;
	unsigned char datablock[512];
	for ( int i = 0; i<512; i++) {
		datablock[i] = 0x00;
	}
	unsigned char unitListArray[512];
	unsigned char cmdarray[2];
	unsigned char cmd = getCmd(cmdarray);
	unsigned long blockAdress;
	unsigned char unitID = 0x00; // til at gemme unit Id
	unsigned char my_unit_count = 0;
	int numberoffields = 0;
	switch(cmd)
	{
		case 1: // PC tilsluttet 0 bytes data;
					// sæt register i main ( PC bit high)
					PCconnectionStatus = true;
					break;
		case 2: // PC frakoblet 0 bytes data;
					// sæt register i main ( PC bit low)
					PCconnectionStatus = false;
					break;
		case 3: // tjek kode 0 bytes data;
				// tjek kode pin ( low = korrekt kode) og returner til PC hvad resultatet er.
					if( (PINA & 0b00000001) == 0) { // PORTA 0
						
						uartPointer->sendChar(0x01);	
					} else {
						uartPointer->sendChar(0x00);
					}
					break;
		case 4: // anmod om fejl log // ej implementeret
					// ignorer
					break;
		case 5: // Anmod om enhedsstatus 1 byte: enhedsadresse;
					break; // ikke implementeret i pc software endnnu
		case 6:  // Hent Enhed Returner enhedsinfo for næste enhed i rækket, start enhed 1.
					//Nær ikke flere enheder:
					//Err cmd.
					my_unit_count = UnitHandlerPointer->getUnitCount(); // hent antal af enheder
					for ( unsigned char i = 1; i <= my_unit_count; i++){ // handle each unit ;)
						UnitHandlerPointer->getUnitList(unitListArray); // get unit list
						for(int j = 0; j<512; j++){
							if(j%2 != 0){
								if(unitListArray[j] == i){
									unsigned char id = j - 1;
									unitID = unitListArray[id];
								}
							}
						}
						for ( int h = 1; h < 8; h++ ){
							UnitHandlerPointer->getTimeTable(h, unitID, datablock);
						//	uartPointer->sendChar(0xF0);
						//	uartPointer->sendChar(0xF0);
							for ( int t = 0; t < 512; t++){
								uartPointer->sendChar(datablock[t]);
							}
						//	uartPointer->sendChar(0x0F);
						//	uartPointer->sendChar(0x0F);
						}
					}
					uartPointer->sendChar(0xFA);
					uartPointer->sendChar(0xFA);
					uartPointer->sendChar(0xFA);
					uartPointer->sendChar(0xFA); // 0xFAFAFAFA
					break;
		
		case 7: // Enhedsinfo fra PC til styreboks 4 til 512 bytes; ack for hver byte efter cmd
			
			if(getData(datablock) > 3)
			{
				if(datablock[1] == 0x00)
				{
					UnitHandlerPointer->AddUnit(datablock[0]);	
				} 
				else
				{
					UnitHandlerPointer->AddUnit(datablock[0], datablock[1]);
				}
				uartPointer->sendChar(0x0F);
			}		
					break;
		case 8: // slet enhed 1 byte;
			if(getData(datablock) == 1){
				UnitHandlerPointer->RemoveUnit(datablock[0]);
			}
					break;
		case 9: // Edit Unit
				if(getData(datablock) == 3)
				{
					UnitHandlerPointer->editUnit(datablock[0], datablock[1], datablock[2]);
				}					 
					break;
		case 10: // update tidsplan 512 bytes;
			numberoffields = getData(datablock);
			if(numberoffields)
			{
				unsigned char tempArray[512];
				for( int q = 0; q < 512; q++){
					tempArray[q] = 0x00;
				}
				unsigned char offset = 0;
				for( int i = 0; i < numberoffields; i++ ){
					
					if(i > 0){
						offset = i+1;
					} else {
						offset = i;
					}
					tempArray[offset] = datablock[i];
				}
				UnitHandlerPointer->UpdateTime(tempArray[0], tempArray);
			}
			break;
		case 11: // Set time 7 bytees;
			// ikke implementereret i pc software.
			break;
	}
}
//=============================================================
// METHOD : returnStatus
// DESCR. : takes the unitID and the recieved status and returns it to
// the PC software through UART.
//=============================================================
void PCinterface::returnStatus( unsigned char unitID, unsigned char status )
{
	// implementation missing.
}





