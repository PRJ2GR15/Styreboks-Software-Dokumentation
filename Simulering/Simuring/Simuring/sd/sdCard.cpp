//========================================================================
// FILENAME : sdCard.cpp
// CREATED : 22-05-2016
// AUTHOR : Tonni Follmann
// DESCR. : Implementation file for the ardiono SD Card driver.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 22-05-2016/Tonni Nybo Follmann created the initial header file.
//========================================================================
 #include "sdCard.h"
 
 //=============================================================
 // METHOD : sdCard Constructor
 // DESCR. : runs the initiation sequence of the sd card and sets 
 // SPI to the supplied speed
 //=============================================================
 
 sdCard::sdCard( int speed ) : spi_obj(125)
 {
	 this->speed = speed; 

 }


//=============================================================
// METHOD : sendCommand
// DESCR. : takes a cmd index, an argument and a CRC and creates
// and sends the command to the sd card.
//=============================================================
void sdCard::sendCommand( unsigned char cmdindex, unsigned long argument, unsigned char CRC )
{
	unsigned char *argument_byte_pointer = (unsigned char*)&argument; // creating a pointer to extract bytes to be send.
	spi_obj.writeByte(0xFF); // prepare the sd card by sending 8 clock pulses.
	spi_obj.writeByte(0b01000000 | cmdindex);
	spi_obj.writeByte(argument_byte_pointer[0]);
	spi_obj.writeByte(argument_byte_pointer[1]);
	spi_obj.writeByte(argument_byte_pointer[3]);
	spi_obj.writeByte(argument_byte_pointer[4]);
	spi_obj.writeByte(CRC);
}



unsigned char sdCard::getResponeByte()
{
	return spi_obj.recieveByte();
}

void sdCard::writeByte( unsigned char bla)
{
	spi_obj.writeByte(bla);
}

bool sdCard::init()
{
	_delay_ms(2);
	PORTB = PINB | 0b00000001; // sets the SS pin ghigh
	spi_obj.setFreq(125); // frequency during initiation must be lower than 400 kHz.
	_delay_ms(2); // let the clock settle in.
	
	// dummy cycles need to be moved to sdCard class.
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	spi_obj.writeByte(0xFF);
	
	unsigned char result = 0x00; // variable to store the result for error checking.
	do
	{
		PORTB = PINB & 0b11111110; // set the SS pin low
		// CMD0 needs to be moved to SD card class
		spi_obj.writeByte(0xFF); // dummy byte to let clock sync.
		spi_obj.writeByte(0x40); // cmd
		spi_obj.writeByte(0x00); // no argument
		spi_obj.writeByte(0x00); // no argument
		spi_obj.writeByte(0x00); // no argument
		spi_obj.writeByte(0x00); // no argument
		spi_obj.writeByte(0x95); // CRC
		spi_obj.recieveByte(); // grab blank return before the real value is extracted
		result = spi_obj.recieveByte(); // grab R1 return value
	} while (result == 0xFF);
	
	if(result == 0x01)
	{
		// CMD8 needs to be moved to SD card class
		spi_obj.writeByte(0xFF); // Dummy byte to let clock sync
		spi_obj.writeByte(0x48); // CMD
		spi_obj.writeByte(0x00); // argument data 4 bytes
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x01);
		spi_obj.writeByte(0xAA);
		spi_obj.writeByte(0x87); // CRC
		//0x48 00 00 01 AA 87
	}
	else
	{
		return false;
	}
	unsigned char cmd8result[5];
	spi_obj.recieveByte(); // grab useless data from pause before the real response.
	for(int i = 0; i < 5; i++)
	{
		cmd8result[i] = spi_obj.recieveByte(); // fill in the response array
	}

	if( cmd8result[0] == 0x01 && cmd8result[1] == 0x00 && cmd8result[2] == 0x00 && cmd8result[3] == 0x01 && cmd8result[4] == 0xAA ) // validate that the card is sd v2
	{
		// if cmd8 successfull start initiating the card
		do // loop fra ACMD41 untill card goes busy.
		{ 
		// CMD55 part of ACMD41
		
		spi_obj.writeByte(0xFF); // sync byte
		spi_obj.writeByte(0x77); // CMD 55
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0xFF); // dummy CRC since CRC should be off now;
		spi_obj.recieveByte();
		
		result = spi_obj.recieveByte();
	
		//rest of ACMD41
		spi_obj.writeByte(0xFF);
		spi_obj.writeByte(0x69); // acmd41
		spi_obj.writeByte(0x40); // HCS bit set high.
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0xFF); // dummy CRC
		spi_obj.recieveByte();
		result = spi_obj.recieveByte();
		
		//SendChar(result);
		} while(result != 0x00); // loop untill card goes busy, indicating the cmd was accepted.
	
		// sending command 58 to request OCR register
		spi_obj.writeByte(0xFF);
		spi_obj.writeByte(0x7A); // cmd 58
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0xFF); // dummy CRC;
	
		unsigned char OCR[4]; // used to store the OCR register
	
		spi_obj.recieveByte();
		spi_obj.recieveByte(); // R1 byte, not using the content of it here, as i'm keeping it simplified.
		OCR[0] = spi_obj.recieveByte(); // getting the 4 bytes of OCR
		OCR[1] = spi_obj.recieveByte();
		OCR[2] = spi_obj.recieveByte();
		OCR[3] = spi_obj.recieveByte();
	
		unsigned char isvalid = OCR[0] & 0b1000000; // getting the validate pin first needs to be high for HC pin to be valid;
		unsigned char isHC = OCR[0] & 0b01000000; // getting HC pin value.
		if(isHC && isvalid) 
		{
			spi_obj.setFreq(4000);
			return true;
		}
		else {
			return false;
		}	
	} 
	else 
	{
		return false;
	}		
			
	
}

bool sdCard::readBlock( unsigned long adress, unsigned char outputdata[] )
{
	unsigned char *argument_byte_pointer = (unsigned char*)&adress;
	spi_obj.writeByte(0xFF); // clock sync
	spi_obj.writeByte(0x51);
	spi_obj.writeByte(argument_byte_pointer[3]);
	spi_obj.writeByte(argument_byte_pointer[2]);
	spi_obj.writeByte(argument_byte_pointer[1]);
	spi_obj.writeByte(argument_byte_pointer[0]);
	spi_obj.writeByte(0xFF); // dummy CRC;
	
	spi_obj.recieveByte(); // getting empty response out of the way.
	unsigned char result;
	result = spi_obj.recieveByte();
	if(result != 0x00){
		return false;
	}
	
	do 
	{
		result = spi_obj.recieveByte();
	} while (result == 0xFF);
	
	if(result != 0xFE){
		return false;
	}
	
	for( int i = 0; i < 512; i++ ){ // filling up the supplied array with the data content of the requested block.
		outputdata[i] = spi_obj.recieveByte();
	}
	spi_obj.recieveByte();
	spi_obj.recieveByte();	
	return true;
}

bool sdCard::writeBlock( unsigned long adress, const unsigned char data[] )
{
		unsigned char *argument_byte_pointer = (unsigned char*)&adress;

		//sendchar(dataout[7]);
		spi_obj.writeByte(0xff); // dummy byte
		spi_obj.writeByte(0x58); // write cmd
		spi_obj.writeByte(argument_byte_pointer[3]); // adress bytes 
		spi_obj.writeByte(argument_byte_pointer[2]); // adress bytes 
		spi_obj.writeByte(argument_byte_pointer[1]); // adress bytes 
		spi_obj.writeByte(argument_byte_pointer[0]); // adress bytes 
		spi_obj.writeByte(0xff); // CRC
		spi_obj.recieveByte(); // grab that idle byte returned
		unsigned char val = spi_obj.recieveByte(); // grab return value (R1)
		if(val != 0x00){ // fejl hvis ikke busy. 
			return false;
		}
		
		spi_obj.writeByte(0xff); // dummy byte
		spi_obj.writeByte(0xfe); // data start byte
		for(int z = 0; z < 512; z++){
			spi_obj.writeByte(data[z]); // send the content of the data array to be stored
		}
		spi_obj.writeByte(0xff); // dummy checksum 16 bit
		spi_obj.writeByte(0xff); // dummy checksom 16 bit;
		unsigned char result; 
		do
		{
			result = spi_obj.recieveByte(); // grab result byte untill card is not busy
		} while (result == 0x00);
		if(result != 0xE5){ // if data not accepted return false.
			return false;
		}
		
		spi_obj.recieveByte();
		do
		{
			result = spi_obj.recieveByte(); // looping untill card is done writing data
		} while (result != 0xff);
		
		spi_obj.writeByte(0xff); // dummy byte
		spi_obj.writeByte(0x4d); // cmd for requesting card status to make sure the write completed correctly.
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0x00);
		spi_obj.writeByte(0xff);  // CRC
		
		spi_obj.recieveByte(); // get the dummy FF byte out of the way
		unsigned char r2[2]; // array for storing the R2 reply
		r2[0] = spi_obj.recieveByte(); // grab two bytes and store them in array
		r2[1] = spi_obj.recieveByte(); 
		
		if(r2[0] != 0x00){
			return false;
		} 
		else if(r2[1] != 0x00){
			return false;
		} else {
			return true;
		}
		//sendchar(r2[0]);
		//sendchar(r2[1]);
		
}












