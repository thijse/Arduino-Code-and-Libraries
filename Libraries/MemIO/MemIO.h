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

#ifndef MEMIO_h
#define MEMIO_h

#include <MemIO.h>

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
#include <inttypes.h>

// Writes next byte at position int pos, with value uint8_t val. This is always a continuous sequence
typedef void (*memWriteNextByte)(int, uint8_t);   
// Read next byte at position int pos, returning uint8_t value. This is always a continuous sequence
typedef uint8_t (*memReadNextByte)(int);
// Called at start of writing a sequence, starting at int pos, of int bytes - may be undefined
typedef void (*startWriteByteSequence)(int, int);
// Called at end of writing a sequence  may be undefined
typedef void (*endWriteByteSequence)();
// Called at start of reading a sequence, starting at int pos, of int bytes - may be undefined
typedef void (*startReadByteSequence)(int, int);
// Called at end of writing a sequence  may be undefined
typedef void (*endReadByteSequence)();
// Initializing a device (Called at instantiation time) - may be undefined
typedef bool (*initializeDevice)();
// Closing a device (called at disposeing time) - may be undefined
typedef bool (*closeDevice)();

class MemIO
{	  
  public:
	MemIO(memWriteNextByte writer, 
             memReadNextByte reader, 
             startWriteByteSequence startWriteSequence = NULL, 
			 endWriteByteSequence endWriteSequence= NULL,
			 startReadByteSequence startReadSequence= NULL, 
			 endReadByteSequence endReadSequence= NULL,
			 initializeDevice initializeDev= NULL,
			 closeDevice closeDev = NULL);		
	~MemIO();			 
	int 	 writtenBytes();
    void 	 setMemPool(int base, int memSize);
	void  	 setMaxAllowedWrites(int allowedWrites);
	int 	 getAddress(int noOfBytes);
    
	uint8_t  read(int);	
	bool 	 readBit(int, byte);
	uint8_t  readByte(int);
    uint16_t readInt(int);
    uint32_t readLong(int);
	float    readFloat(int);
	double   readDouble(int);
			
    bool     write(int, uint8_t);
	bool 	 writeBit(int , uint8_t, bool);
	bool     writeByte(int, uint8_t);
	bool 	 writeInt(int, uint16_t);
	bool 	 writeLong(int, uint32_t);
	bool 	 writeFloat(int, float);
	bool 	 writeDouble(int, double);

	bool     update(int, uint8_t);
	bool 	 updateBit(int , uint8_t, bool);
	bool     updateByte(int, uint8_t);
	bool 	 updateInt(int, uint16_t);
	bool 	 updateLong(int, uint32_t);
	bool 	 updateFloat(int, float);
	bool 	 updateDouble(int, double);

	
    // Use template for other data formats


	template <class T> int readBlock(int address, const T value[], int items)
	{
		if (_startReadSequence!=NULL) _startReadSequence(address, sizeof(T)*items);
		if (!isWriteOk(address+items*sizeof(T))) return 0;
		unsigned int i;
		for (i = 0; i < items; i++) 
			readSequence<T>(address+(i*sizeof(T)),value[i]);
		if (_endReadSequence!=NULL) _endReadSequence();
		return i;
	}
	
	template <class T> int readBlock(int address, const T& value)
	{			
		if (_startReadSequence!=NULL) _startReadSequence(address, sizeof(T));
		int count = readSequence<T>(address,value);
		if (_endReadSequence!=NULL) _endReadSequence();
		return count;
	}
	
	template <class T> int readSequence(int address, const T& value)
	{			
		int writeCount=0;		
		byte* bytePointer = (byte*)(const void*)&value;
		for (unsigned int i = 0; i < sizeof(value); i++) {
			*bytePointer = readByte(address);
			writeCount++;		
			address++;
			*bytePointer++;
		}
		return writeCount;
	}
	
	template <class T> int writeBlock(int address, const T value[], int items)
	{	
		if (_startWriteSequence!=NULL) _startWriteSequence(address, sizeof(T)*items);
		if (!isWriteOk(address+items*sizeof(T))) return 0;
		unsigned int i;
		for (i = 0; i < items; i++) 
			  writeSequence<T>(address+(i*sizeof(T)),value[i]);
		if (_endWriteSequence!=NULL) _endWriteSequence();
		return i;
	}
	
	template <class T> int writeBlock(int address, const T& value)
	{
		if (_startWriteSequence!=NULL) _startWriteSequence(address,sizeof(T));
		int count = writeSequence<T>(address,value);
		if (_endWriteSequence!=NULL) _endWriteSequence();
		return count;
	}
	
	template <class T> int writeSequence(int address, const T& value)
	{
		int writeCount=0;
		if (!isWriteOk(address+sizeof(value))) return 0;
		const byte* bytePointer = (const byte*)(const void*)&value;
		for (unsigned int i = 0; i < sizeof(value); i++) {
			writeByte(address, *bytePointer);
			writeCount++;		
			address++;
			*bytePointer++;
		}
		return writeCount;
	}
	
private:
	//Private variables
	static int _base;
	static int _memSize;
	static int _nextAvailableaddress;	
	static int _writeCounts;
	int _allowedWrites;	
	bool checkWrite(int base,int noOfBytes);	
	bool isWriteOk(int address);
	bool isReadOk(int address);
	memWriteNextByte _writer;
	memReadNextByte _reader;
	startWriteByteSequence _startWriteSequence;
	endWriteByteSequence _endWriteSequence;
	startReadByteSequence _startReadSequence;
	endReadByteSequence _endReadSequence;
	initializeDevice _initializeDev;
	closeDevice _closeDev;			
};

#endif

