/*
  TimeUtils.c - Time utility library 
  Copyright (c) Thijs Elenbaas 2012

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
  
  18 Feb 2013 - initial release 
*/

#include <Time.h>        //http://www.arduino.cc/playground/Code/Time
#include <TimeUtils.h>

#define _TIMEUTILS_VERSION 0_1_0 // software version of this library

// Constructor
TimeUtils::TimeUtils() 
{
}

// Constructor, sets time
TimeUtils::TimeUtils(time_t time) 
{
	_time = time;
}

// Constructor, sets time elements
TimeUtils::TimeUtils(tmElements_t timeElements) 
{
	_time = makeTime(timeElements);
}

// Sets time
void TimeUtils::setTime(time_t time) {
	_time = time;
}

// Sets time elements
void TimeUtils::setTime(tmElements_t timeElements) {
	_time = makeTime(timeElements);
}

// Returns time 
time_t TimeUtils::getTime() {
	return _time;
}

// Returns time elements
tmElements_t TimeUtils::getTimeElements() {	
	breakTime(_time, _timeElements); 
	return _timeElements;
}

//parses string with HH:MM:SS date format
void TimeUtils::parseTime(const char* timeChar) {
	String timeStr = timeChar;
	_timeElements = getTimeElements();
	
	int hour = strintToInt(timeStr.substring(0,2));
	if (hour > 24) {
		Serial.println("Day must be between 0 and 24");
		return;
	}
	_timeElements.Hour = hour;
	
	int minute = strintToInt(timeStr.substring(3,5));
	if (minute > 60) {
		Serial.println("Minute must be between 0 and 60");
		return;
	}
	_timeElements.Minute = minute;		
	
	int second = strintToInt(timeStr.substring(6,8));
	if (second > 60) {
		Serial.println("Second must be between 0 and 60");
		return;
	}
	_timeElements.Second = second;	
	
	setTime(_timeElements);
}

//parses string with YYYYMMDD date format
void TimeUtils::parseDate(const char* dateChar) {
	String dateStr = dateChar;
	_timeElements = getTimeElements();
	
	int year = strintToInt(dateStr.substring(0,4));
	if (year < 2000 || year >= 2100) {
		Serial.println("Year must be between 2000 and 2099");
		return;
	}
	_timeElements.Year = CalendarYrToTm(year);        
	
	int month = strintToInt(dateStr.substring(4,6));        
	if (month < 1 || month > 12) {
		Serial.println("Month must be between 1 and 12");
		return;
	}
	_timeElements.Month = month;
	
	int day = strintToInt(dateStr.substring(6,8));
	uint8_t len = monthLength(&_timeElements);
	if (day < 1 || day > len) {
		Serial.print("Day must be between 1 and ");
		Serial.println(len, DEC);
		return;
	}
	_timeElements.Day = day; 
	
	setTime(_timeElements);
}

// Print time
void TimeUtils::printTime() {
	_timeElements = getTimeElements();
    printDec2(_timeElements.Hour);
    Serial.print(':');
    printDec2(_timeElements.Minute);
    Serial.print(':');
    printDec2(_timeElements.Second);
}

// Print date
void TimeUtils::printDate() {   
	_timeElements = getTimeElements();
    Serial.print(days[weekday(_time) - 1]);
    Serial.print(_timeElements.Day, DEC);
    Serial.print(months[_timeElements.Month - 1]);
    Serial.print(tmYearToCalendar(_timeElements.Year), DEC);
}

// Converts String to Int
int TimeUtils::strintToInt(String value) {
	char chars[value.length()+1]; 
	value.toCharArray(chars, value.length()+1);
	return atoi(chars);
}

// Determines if leapyear
bool TimeUtils::isLeapYear(int year)
{
    if ((year % 100) == 0)
        return (year % 400) == 0;
    else
        return (year % 4) == 0;
}

// Returns length on months
uint8_t TimeUtils::monthLength(tmElements_t *timeElements)
{
    if (timeElements->Month != 2 || !isLeapYear(tmYearToCalendar(timeElements->Year)))
        return monthLengths[timeElements->Month - 1];
    else
		return 29;
}

// Returns 2-decimal long string representation of int
void TimeUtils::printDec2(int value)
{
    Serial.print((char)('0' + (value / 10)));
    Serial.print((char)('0' + (value % 10)));
}

 const char *TimeUtils::days[] = {
	"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "
};

const char *TimeUtils::months[] = {
	" Jan ", " Feb ", " Mar ", " Apr ", " May ", " Jun ",
	" Jul ", " Aug ", " Sep ", " Oct ", " Nov ", " Dec "
};

const uint8_t TimeUtils::monthLengths[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


