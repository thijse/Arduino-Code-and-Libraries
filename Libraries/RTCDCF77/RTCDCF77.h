/*
 * RTCDCF77.h - library for DS1307 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef RTCDCF77_h
#define RTCDCF77_h

#include <Time.h>
#include <MemIO.h>
#include <TimeUtils.h>
#include <DCF77.h>
#include <DS1307RTC.h>

struct Stat
{
    time_t previousRefTime; 
	time_t interval; 
    long drift;
    unsigned int updates;
	double correctionFactor;
};

// library interface description
class RTCDCF77
{
  private:
  // private functions  
    DS1307RTC* DSRTC;
	DCF77* DCFC;
	TimeUtils* convert;
	void printTime(time_t time);
	void printDateTime(time_t time);
	bool set(time_t curRefTime);
	void startStopDCF();
	void retrieve();
	void store();
	//double correctionFactor;
	double secondOrder;
	int addrPreviousRefTime;
	int addrInterval;
	int addrDrift;
	int addrUpdates;
	bool useInterval;
	Stat status;
	
  public:
  // public functions
    RTCDCF77(DS1307RTC* RTCClock, DCF77* DCFClock);	
    time_t get();
	time_t getUncorrected();
	void reset();	
	bool update();
	void initialize();
	Stat getStatus();
};


#endif
 

