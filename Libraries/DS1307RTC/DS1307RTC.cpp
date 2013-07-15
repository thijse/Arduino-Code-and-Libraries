/*
 * DS1307RTC.h - library for DS1307 RTC
  
  
  Copyright (c) Michael Margolis 2009
  Update Thijs Elenbaas 2013
  
  This library is intended to be uses with Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  30 Dec 2009 - Initial release
  5 Sep 2011  - updated for Arduino 1.0
  9 Mar 2013  - Added memory access, start & stop
 */
 
 // Based on code by Michael Margolis and Oliver Kraus
 // http://code.google.com/p/ds1307new/
 
#include <Wire.h>
#include <MemIO.h>
#include "DS1307RTC.h"

#define _DS1307RTC_VERSION 1_1_0 // software version of this library

#define DS1307_CTRL_ID 0x68 

// **** Start Functions for writing to RAM using MemIO class ***

// Writes next byte at position int pos, with value uint8_t val. 
void writeRAM(int address, uint8_t value)
{	
  Wire.write(value);
}

// Read next byte at position int pos, returning uint8_t value. 
uint8_t readRAM(int address)
{
  uint8_t value = Wire.read();
  return value;      
}

// Called at start of writing a sequence. 
void startWriteRAM(int address, int bytes) 
{
  uint8_t rtc_addr = address;
  uint8_t bts = bytes;
  Wire.beginTransmission(DS1307_CTRL_ID);
  rtc_addr &= 63;                       // avoid wrong adressing. Adress 0x08 is now address 0x00...
  rtc_addr += 8;                        // ... and address 0x3f is now 0x38
  Wire.write(rtc_addr); 
}

// Called at start of reading a sequence.
void startReadRAM(int address, int bytes) 
{
  uint8_t rtc_addr = address;
  uint8_t rtc_quantity = bytes;
  
  Wire.beginTransmission(DS1307_CTRL_ID);
  rtc_addr &= 63;                       // avoid wrong adressing. Adress 0x08 is now address 0x00...
  rtc_addr += 8;                        // ... and address 0x3f is now 0x38
  Wire.write(rtc_addr);                  // set CTRL Register Address
  if ( Wire.endTransmission() != 0 )
    return;
  Wire.requestFrom(DS1307_CTRL_ID, (int)rtc_quantity);
  while(!Wire.available())
  {
    // waiting
  }
}
// Called at end of reading a sequence. 
void stopReadRAM() 
{
  Wire.endTransmission();
}

// Called at end of writing a sequence. 
void stopWriteRAM() 
{
  Wire.endTransmission();
}
// **** End Functions for writing to RAM using MemIO class ***

/**
 * Constructor
 */
DS1307RTC::DS1307RTC()
{
  Wire.begin();
  RAM = new MemIO(writeRAM,readRAM,startWriteRAM,stopWriteRAM,startReadRAM, stopReadRAM, NULL, NULL);
  RAM->setMemPool(0,56); //0x3f - 0x8h + 1 = 56 bytes, see http://datasheets.maximintegrated.com/en/ds/DS1307.pdf, page 7    
  //RAM->setMaxAllowedWrites(0);
}
  
/**
 * Get time/date
 */
time_t DS1307RTC::get()   // Aquire data from buffer and convert to time_t
{
  tmElements_t tm;
  if (read(tm) == false) return 0;
  return(makeTime(tm));
}

/**
 * Set time/date
 */
bool DS1307RTC::set(time_t t)
{
  tmElements_t tm;
  breakTime(t, tm);
  tm.Second |= 0x80;  // stop the clock 
  write(tm); 
  tm.Second &= 0x7f;  // start the clock
  write(tm); 
}

/**
 * Read time elements from the RTC chip in BCD format
 */
bool DS1307RTC::read(tmElements_t &tm)
{
  uint8_t sec;
  Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); 
#else
  Wire.send(0x00);
#endif  
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(DS1307_CTRL_ID, tmNbrFields);
  if (Wire.available() < tmNbrFields) return false;
#if ARDUINO >= 100
  sec = Wire.read();
  tm.Second = bcd2dec(sec & 0x7f);   
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.read() );
  tm.Day = bcd2dec(Wire.read() );
  tm.Month = bcd2dec(Wire.read() );
  tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
#else
  sec = Wire.receive();
  tm.Second = bcd2dec(sec & 0x7f);   
  tm.Minute = bcd2dec(Wire.receive() );
  tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.receive() );
  tm.Day = bcd2dec(Wire.receive() );
  tm.Month = bcd2dec(Wire.receive() );
  tm.Year = y2kYearToTm((bcd2dec(Wire.receive())));
#endif
  if (sec & 0x80) return false; // clock is halted
  return true;
}

/**
 * Write time elements from the RTC chip in BCD format
 */
bool DS1307RTC::write(tmElements_t &tm)
{
  Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); // reset register pointer  
  Wire.write(dec2bcd(tm.Second)) ;   
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
  Wire.write(dec2bcd(tm.Wday));   
  Wire.write(dec2bcd(tm.Day));
  Wire.write(dec2bcd(tm.Month));
  Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 
#else  
  Wire.send(0x00); // reset register pointer  
  Wire.send(dec2bcd(tm.Second)) ;   
  Wire.send(dec2bcd(tm.Minute));
  Wire.send(dec2bcd(tm.Hour));      // sets 24 hour format
  Wire.send(dec2bcd(tm.Wday));   
  Wire.send(dec2bcd(tm.Day));
  Wire.send(dec2bcd(tm.Month));
  Wire.send(dec2bcd(tmYearToY2k(tm.Year)));   
#endif
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}

/**
 * Stop the clock
 */
void DS1307RTC::stopClock()         
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00);                    // Register 0x00 holds the start/stop bit
  Wire.endTransmission();
  Wire.requestFrom(DS1307_CTRL_ID, 1);
  uint8_t second = Wire.read() | 0x80;       	// save actual seconds and OR sec with bit 7 (start/stop bit) = clock stopped
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)second);                 // write seconds back and stop the clock
  Wire.endTransmission();
}

/**
 * Start the clock
 */
void DS1307RTC::startClock()        		
{
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00);                   // Register 0x00 holds the start/stop bit
  Wire.endTransmission();
  Wire.requestFrom(DS1307_CTRL_ID, 1);
  uint8_t second = Wire.read() & 0x7f;         // save actual seconds and AND sec with bit 7 (start/stop bit) = clock started
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)second);                 // write seconds back and start the clock
  Wire.endTransmission();
}

/**
 * Convert Decimal to Binary Coded Decimal (BCD)
 */
uint8_t DS1307RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

/**
 * Convert Binary Coded Decimal (BCD) to Decimal
 */
uint8_t DS1307RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

bool DS1307RTC::exists = false;

DS1307RTC RTC = DS1307RTC(); // create an instance for the user

