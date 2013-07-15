/*
  WordMatrix.h - WordMatrix library
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

#ifndef WORDMATRIX_h
#define WORDMATRIX_h

#include <Time.h>        //http://www.arduino.cc/playground/Code/Time
#include "lightuino.h"
#include "lightuinoAnimatedMatrix.h"
   
enum wordNames { 
	HET,
	IS,
	VIJF,
	TIEN,
	VOOR,
	OVER,
	KWART,
	HALF,
	OVER2,
	VOOR2,
	uEEN,
	uTWEE,
	uDRIE,
	uVIER,
	uVIJF,
	uZES,
	uZEVEN,
	uNEGEN,
	uACHT,
	uTIEN,
	uELF,
	uTWAALF,
	UUR,
	PLUSEEN,
	PLUSTWEE,
	PLUSDRIE,
	PLUSVIER,
};

const unsigned char ClockWords[][3] = {
	{0,  0,  3}, // HET		
	{4,  0,  2}, // IS
	{7,  0,  4}, // VIJF
	{0,  1,  4}, // TIEN
	{7,  1,  4}, // VOOR
	{0,  2,  4}, // OVER
	{6,  2,  5}, // KWART
	{0,  3,  4}, // HALF
	{7,  3,  4}, // OVER2
	{0,  4,  4}, // VOOR2
	{7,  4,  3}, // EEN
	{0,  5,  4}, // TWEE
	{7,  5,  4}, // DRIE
	{0,  6,  4}, // VIER
	{4,  6,  4}, // VIJF
	{8,  6,  3}, // ZES
	{0,  7,  5}, // ZEVEN
	{6,  7,  5}, // NEGEN
	{0,  8,  4}, // ACHT
	{4,  8,  4}, // TIEN
	{8,  8,  3}, // ELF
	{0,  9,  5}, // TWAALF
	{8,  9,  3}, // UUR
	{0,  10,  1}, // plus 1 minute
	{0,  10,  2}, // plus 2 minutes
	{0,  10,  3}, // plus 3 minutes
	{0,  10,  4}  // plus 4 minutes
};

class WordMatrix
{	  	
	private:
		//Private variables
		lightuinoAnimatedMatrix& matrix;
		
		void ItIs();
		void Min0();
		void Min5();
		void Min10();
		void Min15();
		void Min20();
		void Min25();
		void Min30();
		void Min35();
		void Min40();
		void Min45();
		void Min50();
		void Min55();

		void Plus1();
		void Plus2();
		void Plus3();
		void Plus4();

		void Hour1();
		void Hour2();
		void Hour3();
		void Hour4();
		void Hour5();
		void Hour6();
		void Hour7();
		void Hour8();
		void Hour9();
		void Hour10();
		void Hour11();
		void Hour12();
		
	public:			
		WordMatrix(lightuinoAnimatedMatrix& _matrix);
		void SetPixel(unsigned char x,unsigned char y,unsigned char val);
		void SetWord(const unsigned char ClockWord[]);
		void SetTime(time_t time);
};

#endif

