/*
  MemIO.h - Generic Memory Access library
  Copyright (c) 2012 Thijs Elenbaas.  All right reserved.

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
#include <MemIO.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/

 #define _MEMIO_VERSION 0.1   // software version of this library
// #define _MEMIO_DEBUG         // Enables logging of maximum of writes and out-of-memory

 /******************************************************************************
 * Constructors
 ******************************************************************************/

MemIO::MemIO(memWriteNextByte writer, 
             memReadNextByte reader, 
             startWriteByteSequence startWriteSequence, 
			 endWriteByteSequence endWriteSequence,
			 startReadByteSequence startReadSequence, 
			 endReadByteSequence endReadSequence,
			 initializeDevice initializeDev,
			 closeDevice closeDev)
  :  _allowedWrites(0), 
     _writer(writer), 
	 _reader(reader),
     _startWriteSequence(startWriteSequence), 
	 _endWriteSequence(endWriteSequence),
	 _startReadSequence(startReadSequence), 
	 _endReadSequence(endReadSequence),
	 _initializeDev(initializeDev), 
	 _closeDev(closeDev)     
{
   if (_initializeDev!=NULL) _initializeDev();
   
   if (_writer==NULL)  Serial.println("Error: No Memory writer function defined.");
   if (_reader==NULL)  Serial.println("Error: No Memory reader function defined.");
   
}

MemIO::~MemIO() {
	if (_closeDev!=NULL) 	_closeDev();
}
/******************************************************************************
 * User API
 ******************************************************************************/

void MemIO::setMemPool(int base, int memSize) {
	//Base can only be adjusted if no addresses have already been issued
	if (_nextAvailableaddress == _base) 
		_base = base;
		_nextAvailableaddress=_base;
	
	//Ceiling can only be adjusted if not below issued addresses
	if (memSize >= _nextAvailableaddress ) 
		_memSize = memSize;

	#ifdef _MEMIO_DEBUG  
	if (_nextAvailableaddress != _base) 
		Serial.println("Cannot change base, addresses have been issued");

	if (memSize < _nextAvailableaddress )  
		Serial.println("Cannot change ceiling, below issued addresses");
	#endif	
	
}

void MemIO::setMaxAllowedWrites(int allowedWrites) {
#ifdef _MEMIO_DEBUG
	_allowedWrites = allowedWrites;
#endif			
}

int MemIO::getAddress(int noOfBytes){
	int availableaddress   = _nextAvailableaddress;
	_nextAvailableaddress += noOfBytes;

#ifdef _MEMIO_DEBUG  
	if (_nextAvailableaddress > _memSize) {
		Serial.println("Attempt to write outside of memory");
		return -availableaddress;
	} else {
		return availableaddress;
	}
#endif
	return availableaddress;		
}
 
uint8_t MemIO::read(int address)
{
	return readByte(address);
}

bool MemIO::readBit(int address, byte bit) {
	  if (bit> 7) return false; 
	  if (!isReadOk(address+sizeof(uint8_t))) return false;
	  byte byteVal = readByte(address);      
	  byte bytePos = (1 << bit);
      return (byteVal & bytePos);
}

uint8_t MemIO::readByte(int address)
{	
	if (!isReadOk(address+sizeof(uint8_t))) return 0;
	return _reader(address);
}

uint16_t MemIO::readInt(int address)
{
	uint16_t _value;
	readBlock<uint16_t>(address, _value);
	return _value;
}

uint32_t MemIO::readLong(int address)
{
	uint32_t _value;
	readBlock<uint32_t>(address, _value);
	return _value;
}

float MemIO::readFloat(int address)
{
	float _value;
	readBlock<float>(address, _value);
	return _value;
}

double MemIO::readDouble(int address)
{
	double _value;
	readBlock<double>(address, _value);
	return _value;
}

bool MemIO::write(int address, uint8_t value)
{
	return writeByte(address, value);
}

bool MemIO::writeByte(int address, uint8_t value)
{
	if (!isWriteOk(address+sizeof(uint8_t))) return false;
	_writer(address, value);
	return true;
}

bool MemIO::writeInt(int address, uint16_t value)
{
	return (writeBlock<uint16_t>(address, value)!=0);	
}

bool MemIO::writeLong(int address, uint32_t value)
{
	return (writeBlock<uint32_t>(address, value)!=0);	
}

bool MemIO::writeFloat(int address, float value)
{
	return (writeBlock<float>(address, value)!=0);	
}

bool MemIO::writeDouble(int address, double value)
{
	return (writeBlock<float>(address, value)!=0);	
}

bool MemIO::writeBit(int address, uint8_t bit, bool value) 
{
	  if (bit> 7) return false; 
	  
	  byte byteValInput  = readByte(address);
	  byte byteValOutput = byteValInput;	  
	  // Set bit
	  if (value) {	    
		byteValOutput |= (1 << bit);  //Set bit to 1
	  } else {		
	    byteValOutput &= ~(1 << bit); //Set bit to 0
	  }
	  // Store if different from input
	  if (byteValOutput!=byteValInput) {
		writeByte(address, byteValOutput);	  
	  }
}

bool MemIO::isWriteOk(int address)
{
#ifdef _MEMIO_DEBUG  
	_writeCounts++;
	if (_allowedWrites != 0 && _writeCounts > _allowedWrites ) {
		Serial.println("Exceeded maximum number of writes");
		Serial.print("_writeCounts ");
		Serial.println(_writeCounts);
		Serial.print("_allowedWrites ");
		Serial.println(_allowedWrites);
				
		return false;
	}
	
	if (address > _memSize) {
		Serial.println("Attempt to write outside of memory");
		return false;
	} else {
		return true;
	}
#endif		
	return true;
}

bool MemIO::isReadOk(int address)
{
#ifdef _MEMIO_DEBUG  
	if (address > _memSize) {
		Serial.println("Attempt to write outside of memory");
		return false;
	} else {
		return true;
	}
#endif
	return true;	
}

int MemIO::_base= 0;
int MemIO::_memSize= 512;
int MemIO::_nextAvailableaddress= 0;
int MemIO::_writeCounts =0;