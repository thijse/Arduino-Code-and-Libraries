/*
  EEPROMEx.cpp - Extended EEPROM library
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

/******************************************************************************
 * Includes
 ******************************************************************************/
#include  "MemIO.h"
#include "EEPROMEx.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define _EEPROMEX_VERSION 0.9.0 // software version of this library

void EEPROMWriter(int address, uint8_t value)
{	
	// Always read byte first to see if update needed (to reduce wear)
	if (eeprom_read_byte((unsigned char *) address)!=value) {
		eeprom_write_byte((unsigned char *) address, value);
	}
}

uint8_t EEPROMReader(int address)
{
	return eeprom_read_byte((unsigned char *) address);
}
  
 /******************************************************************************
 * Constructors
 ******************************************************************************/

EEPROMClassEx::EEPROMClassEx() : MemIO(EEPROMWriter,EEPROMReader)   
{
  setMaxAllowedWrites(100);
}
 
bool EEPROMClassEx::isReady() {
	return eeprom_is_ready();
}

EEPROMClassEx EEPROM;
