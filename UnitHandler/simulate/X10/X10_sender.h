//========================================================================
// FILENAME : X10_sender.h
// CREATED : 25-05-2016
// AUTHOR : Christian Bøeg-Jensen
// DESCR. : Header for the driver to send data via X10.1
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.0 <rev. date/author> <Change description>
//========================================================================


#ifndef X10_SENDER_H_
#define X10_SENDER_H_


//=====================================
// CLASS  : X10_sender
// DESCR. : Class for sending data over X10.1
//=====================================
class X10_sender
{
public:
	X10_sender();
	void start120();
	void stop120();
	
	void setSendMode(bool);
	bool getSendMode();
	
	void sendCommand(unsigned char houseCode, unsigned char adress, unsigned char type, unsigned char data = 0);
	
	unsigned int getPacket1();
	unsigned int getPacket2();
	unsigned char getCurrentPacket();
	void setCurrentPacket(unsigned char);
	
	void decrementCurrentPos1();
	unsigned char getCurrentPos1();
	void decrementCurrentPos2();
	unsigned char getCurrentPos2();
	
	unsigned char generateParity(unsigned int packet1, unsigned int packet2);
	
	void sendHigh();
	void sendLow();
	
private:
	unsigned int outGoingPacket1_;
	unsigned int outGoingPacket2_;
	unsigned char currentPos1_;
	unsigned char currentPos2_;
	unsigned char currentPacket_;
	bool sendMode_;
	
	unsigned char startBit_;
};

#endif /* X10_SENDER_H_ */