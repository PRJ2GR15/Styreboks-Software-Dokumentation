///========================================================================
// FILENAME : X10_modtager.h
// CREATED : 25-05-2016
// AUTHOR : Christian Bøeg-Jensen
// DESCR. : Header for driver to receiving X10.1 communication
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 <rev. date/author> <Change description>
//========================================================================


#ifndef X10_MODTAGER_H_
#define X10_MODTAGER_H_


//=====================================
// CLASS : X10_modtager
// DESCR. : 
//=====================================
class X10_modtager
{
public:
	X10_modtager(unsigned char bit4_houseCode, unsigned char bit8_adress);
	
	void resetReceiver();
	
	void receiveData(unsigned char data);
	
	bool isDataReady();
	void fetchData(unsigned char& type, unsigned char& data, bool& validity);
	
	void startTimout();
	void resetTimout();
	void startInput();
	void inputComplete();
	
	
	void writeDataToPacket1(unsigned char data);
	void writeDataToPacket2(unsigned char data);
	
	bool checkForZero(unsigned char data);
	
	bool isPacketForMe();
	
	unsigned char getPacketType();
	unsigned char getPacketData();
		
	unsigned char getCurrentPos1();
	unsigned char getCurrentPos2();
	
	unsigned char getCurrentPacket();
	
	bool validatePackets();
	bool isValidStart();
	
	unsigned char getData();
	unsigned char getType();
		
private:
	unsigned int packet1_;
	unsigned int packet2_;
	unsigned char currentPos1_;
	unsigned char currentPos2_;
	unsigned char currentPacket_;
	bool notReciving_;
	
	// unit specific variables
	unsigned char houseCode_; 
	unsigned char adress_;
	
	// exit variables
	unsigned char type_;
	unsigned char numberOfData_;
	unsigned char data_;
	bool packetValidity_;
	bool dataReady_;
	
	unsigned char numberOfZero_;

};



#endif /* X10_MODTAGER_H_ */