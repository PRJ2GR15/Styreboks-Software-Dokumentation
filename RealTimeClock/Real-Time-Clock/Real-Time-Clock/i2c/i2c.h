//========================================================================
// FILENAME : i2c.h
// CREATED : 08-04-2016
// AUTHOR : Stefan Nielsen
// DESCR. : Header file for the ardiono i2c driver.
//
//------------------------------------------------------------------------
//
// REV. DATE/AUTHOR CHANGE DESCRIPTION
// 1.1 18-05-2016/Tonni Nybo Follmann added support for setting the clock frequency.
// 1.0 08-04-2016/Stefan Nielsen created the initial header file
//========================================================================
#ifndef i2c_H_
#define i2c_H_
//=====================================
// CLASS : i2c
// DESCR. : represents the i2c/TWI module on arduino mega2560
//=====================================
class i2c 
{
public:
	i2c(bool isMaster, int clock); // clock can be 100 or 400 and represents the SCL value in kHz
	void start();
	void stop();
	void write(unsigned char data);
	unsigned char read(bool isLast);

private:

};

#endif