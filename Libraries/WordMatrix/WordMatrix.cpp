/*
  EEPROMEx.cpp - Extended EEPROM library
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
#include "WordMatrix.h"
#include "lightuino.h"
#include "lightuinoAnimatedMatrix.h"
#include <Time.h>        //http://www.arduino.cc/playground/Code/Timevs

/******************************************************************************
 * Definitions
 ******************************************************************************/

 #define _WORDMATRIX_VERSION 1 // software version of this library
 
/******************************************************************************
 * Constructors
 ******************************************************************************/

// By default we choose conservative settings
WordMatrix::WordMatrix(lightuinoAnimatedMatrix& _matrix) : matrix(_matrix)
{	
}

/******************************************************************************
 * User API
 ******************************************************************************/

void WordMatrix::SetPixel(unsigned char x,unsigned char y,unsigned char val)
{
    // map physical matrix on sinks and sources
		
	const int COLUMNS = 11;
	const int ROWS    = 10;
	
	// Matrix is 11 columns and 10 rows
	// row is defined right to left
	// row 0,2,4,6,8 use source 0
	// row 1,3,5,7,9 use source 1
	// colums 0&1, row 0 - use sink 0, column 1&2 use sink 1    
	
	int sinkNo   = (int)((10-x) + ((y/2) * COLUMNS)); // 0..54
	int sourceNo = ( y % 2 );					      // 0..1
		
	matrix.pixel(sinkNo,sourceNo, val);
}

void WordMatrix::SetWord(const unsigned char ClockWord[])
{
	for (int i = 0; i < ClockWord[2]; i++)
    {
		SetPixel(ClockWord[0]+i,ClockWord[1],1);
	}
}

void inline WordMatrix::ItIs() 
{
	SetWord(ClockWords[HET]);
	SetWord(ClockWords[IS]);
	// Usb.println("HET IS");
}

void inline WordMatrix::Min0()
{	
	SetWord(ClockWords[UUR]);
	// Usb.println("UUR");
}

void inline WordMatrix::Min5()
{
	SetWord(ClockWords[VIJF]);
	SetWord(ClockWords[OVER]);
	// Usb.println("VIJF OVER");
}

void inline WordMatrix::Min10()
{
	SetWord(ClockWords[TIEN]);
	SetWord(ClockWords[OVER]);
	// Usb.println("TIEN OVER");
}

void inline WordMatrix::Min15()
{
	SetWord(ClockWords[KWART]);
	SetWord(ClockWords[OVER2]);
	// Usb.println("KWART OVER");
}

void inline WordMatrix::Min20()
{
	SetWord(ClockWords[TIEN]);
	SetWord(ClockWords[VOOR]);
	SetWord(ClockWords[HALF]);
	// Usb.println("TIEN VOOR HALF");
}

void inline WordMatrix::Min25()
{
	SetWord(ClockWords[VIJF]);
	SetWord(ClockWords[VOOR]);
	SetWord(ClockWords[HALF]);
	// Usb.println("VIJF VOOR HALF");
}

void WordMatrix::Min30()
{
	SetWord(ClockWords[HALF]);
	// Usb.println("HALF");
}

void inline WordMatrix::Min35()
{
	SetWord(ClockWords[VIJF]);
	SetWord(ClockWords[OVER]);
	SetWord(ClockWords[HALF]);
	// Usb.println("VIJF OVER HALF");
}

void inline WordMatrix::Min40()
{
	SetWord(ClockWords[TIEN]);
	SetWord(ClockWords[OVER]);
	SetWord(ClockWords[HALF]);
	// Usb.println("TIEN OVER HALF");
}

void inline WordMatrix::Min45()
{
	SetWord(ClockWords[KWART]);
	SetWord(ClockWords[VOOR2]);
	// Usb.println("KWART VOOR");
}

void inline WordMatrix::Min50()
{
	SetWord(ClockWords[TIEN]);
	SetWord(ClockWords[VOOR]);
	// Usb.println("TIEN VOOR");
}

void inline WordMatrix::Min55()
{
	SetWord(ClockWords[VIJF]);
	SetWord(ClockWords[VOOR]);
	// Usb.println("VIJF VOOR");
}
  
// Hours
void inline WordMatrix::Hour1()
{
	SetWord(ClockWords[uEEN]);
	// Usb.println("EEN");
}

void inline WordMatrix::Hour2()
{
	SetWord(ClockWords[uTWEE]);
	// Usb.println("TWEE");
}

void inline WordMatrix::Hour3()
{
	SetWord(ClockWords[uDRIE]);
	// Usb.println("DRIE");
}

void inline WordMatrix::Hour4()
{
	SetWord(ClockWords[uVIER]);
	// Usb.println("VIER");
}

void inline WordMatrix::Hour5()
{
	SetWord(ClockWords[uVIJF]);
	// Usb.println("VIJF");
}

void inline WordMatrix::Hour6()
{
	SetWord(ClockWords[uZES]);
	// Usb.println("ZES");
}

void inline WordMatrix::Hour7()
{
	SetWord(ClockWords[uZEVEN]);
	// Usb.println("ZEVEN");
}

void inline WordMatrix::Hour8()
{
	SetWord(ClockWords[uACHT]);
	// Usb.println("ACHT");
}

void inline WordMatrix::Hour9()
{
	SetWord(ClockWords[uNEGEN]);
	// Usb.println("NEGEN");
}

void inline WordMatrix::Hour10()
{
	SetWord(ClockWords[uTIEN]);
	// Usb.println("TIEN");
}

void inline WordMatrix::Hour11()
{
	SetWord(ClockWords[uELF]);
	// Usb.println("ELF");
}

void inline WordMatrix::Hour12()
{
	SetWord(ClockWords[uTWAALF]);
	// Usb.println("TWAALF");
}

void inline WordMatrix::Plus1()
{
	SetWord(ClockWords[PLUSEEN]);
}

void inline WordMatrix::Plus2()
{
	SetWord(ClockWords[PLUSTWEE]);
}

void inline WordMatrix::Plus3()
{
	SetWord(ClockWords[PLUSDRIE]);
}

void inline WordMatrix::Plus4()
{
	SetWord(ClockWords[PLUSVIER]);
}

void WordMatrix::SetTime(time_t time)
{
	tmElements_t timeinfo;
	
	breakTime(time, timeinfo);
		
	int min     = timeinfo.Minute % 5;
	int minDiv5 = timeinfo.Minute / 5;
	int hour    = timeinfo.Hour > 12 ? timeinfo.Hour -12 : timeinfo.Hour;
	
	// It is ..
	ItIs();
	
	// 5 minute intervals
	switch ( minDiv5 ) {
		case 0 : 
			Min0(); break;
		case 1 : 
			Min5(); break;
		case 2 : 
			Min10(); break;
		case 3 : 
			Min15(); break;
		case 4 : 
			Min20(); break;
		case 5 : 
			Min25(); break;
		case 6 : 
			Min30(); break;
		case 7 : 
			Min35(); break;
		case 8 : 
			Min40(); break;
		case 9 : 
			Min45(); break;
		case 10 : 
			Min50(); break;
		case 11 : 
			Min55(); break;
	}

	// Plus minutes indicators
	switch ( min ) {
		case 0 : 
			break;
		case 1 : 
			Plus1(); break;
		case 2 : 
			Plus2(); break;
		case 3 : 
			Plus3(); break;
		case 4 : 
			Plus4(); break;
	}

	// hours
	switch ( hour ) {
		case 1 : 
			Hour1(); break;
		case 2 : 
			Hour2(); break;
		case 3 : 
			Hour3(); break;
		case 4 : 
			Hour4(); break;
		case 5 :  
			Hour5(); break;
		case 6 : 
			Hour6(); break;
		case 7 : 
			Hour7(); break;
		case 8 : 
			Hour8(); break;
		case 9 :  
			Hour9(); break;
		case 10 : 
			Hour10(); break;
		case 11 : 
			Hour11(); break;
		case 12 : 
			Hour12(); break;
	}

}
