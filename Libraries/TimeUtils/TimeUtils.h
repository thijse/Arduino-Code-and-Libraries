#ifndef TimeUtils_h
#define TimeUtils_h

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
#include <Time.h>

class TimeUtils {
private:
	//Private variables
	time_t _time;           	
	tmElements_t _timeElements;
	int strintToInt(String value);
	
	static const char *days[];
	static const char *months[];
	static const uint8_t monthLengths[];
	void printDec2(int value);
     
	
public:	
	// Public Functions
	TimeUtils(); 
	TimeUtils(time_t time); 
	TimeUtils(tmElements_t timeElements);
	void setTime(time_t time);
	void setTime(tmElements_t timeElements);
	time_t getTime();
	tmElements_t getTimeElements();
	void parseDate(const char* dateStr);
	void parseTime(const char* timeStr);
	void printDate();
	void printTime();
	bool isLeapYear(int year);
	uint8_t monthLength(tmElements_t *timeElements);
	
 };

#endif	