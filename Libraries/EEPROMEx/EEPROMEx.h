/*
  EEPROMEx.h - Extended EEPROM library
  Copyright (c) 2012-2013 Thijs Elenbaas.  All right reserved.

  This library is free software; you can redistribute it and/or
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
*/

#ifndef EEPROMEX_h
#define EEPROMEX_h

#include <EEPROM.h>
#include  <MemIO.h>

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
#include <inttypes.h>
#include <avr/eeprom.h>

// Boards with ATmega328, Duemilanove, Uno, UnoSMD, Lilypad - 1024 bytes (1 kilobyte)
// Boards with ATmega1280 or 2560, Arduino Mega series – 4096 bytes (4 kilobytes)
// Boards with ATmega168, Lilypad, old Nano, Diecimila  – 512 bytes (1/2 kilobyte)

#define EEPROMSizeATmega168   512     
#define EEPROMSizeATmega328   1024     
#define EEPROMSizeATmega1280  4096     

#define EEPROMSizeUno         EEPROMSizeATmega328     
#define EEPROMSizeUnoSMD      EEPROMSizeATmega328
#define EEPROMSizeLilypad     EEPROMSizeATmega328
#define EEPROMSizeDuemilanove EEPROMSizeATmega328
#define EEPROMSizeMega        EEPROMSizeATmega1280
#define EEPROMSizeDiecimila   EEPROMSizeATmega168
#define EEPROMSizeNano        EEPROMSizeATmega168

typedef void (*memWriter)(int, uint8_t);
typedef uint8_t (*memReader)(int);

void EEPROMWriter(int address, uint8_t value);
void EEPROMWriter(uint16_t address, uint16_t no, uint8_t& value);
uint8_t EEPROMReader(int address);

class EEPROMClassEx :  public MemIO
{	 
  public:
	EEPROMClassEx();
	bool isReady();	
};

extern EEPROMClassEx EEPROM;

#endif

