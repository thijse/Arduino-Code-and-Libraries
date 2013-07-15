/*
 * DS1307RTC.h - library for DS1307 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef DS1307RTC_h
#define DS1307RTC_h

#include <Time.h>
#include <MemIO.h>

void writeRAM(int address, uint8_t value);
uint8_t readRAM(int address);
void startWriteRAM(int adress);
void startReadRAM(int adress);
void stopWriteRAM();

// library interface description
class DS1307RTC
{
  private:
  // private functions  
    static bool exists;
	static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num); 

  public:
  // public functions
	MemIO *RAM;
    DS1307RTC();
    static time_t get();
    static bool set(time_t t);
    static bool read(tmElements_t &tm);
    static bool write(tmElements_t &tm);
    static bool chipPresent() { return exists; }
    static void stopClock();
	static void startClock();    
};

extern DS1307RTC RTC;

#endif
 

