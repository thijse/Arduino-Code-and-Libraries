/*
  RTCDCF77.h - library that combines a DCF77 clock with a RTC clock
    
  Copyright (c) Thijs Elenbaas 2013
  
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
  
  26 Juli 2013 - Initial release  
 */
 
#include <Wire.h> 
#include <TimeUtils.h>
#include <MemIO.h>
#include <DS1307DSRTC.h>
#include <DCF77.h>
#include <RTCDCF77.h>

#define _RTCDCF77_VERSION 0_2_0 // software version of this library

  const time_t halfTime    = 604800;   //   7 * 24 * 60 * 60 = 7 days
  const time_t maxInterval = 15552000; // 180 * 24 * 60 * 60 = half a year
  const time_t minInterval = 30;       //       12 * 60 * 60 = half a day
  const time_t minDrift    = 1;        //                      2 sec

/**
 * Constructor
 */
RTCDCF77::RTCDCF77(DS1307RTC* RTCClock, DCF77* DCFClock)
{
    DSRTC = RTCClock;
    DCFC  = DCFClock;
    
    addrPreviousRefTime = DSRTC->RAM->getAddress(sizeof(time_t));
    addrInterval        = DSRTC->RAM->getAddress(sizeof(time_t));
    addrDrift           = DSRTC->RAM->getAddress(sizeof(long));
    addrUpdates         = DSRTC->RAM->getAddress(sizeof(unsigned int));
    
    convert = new TimeUtils();  
    initialize();
}
 
/**
 * Initialize library
 */
void RTCDCF77::initialize()   
{
  retrieve();
  startStopDCF();
}
 
/**
 * Retrieve drift data from RTC RAM
 */
void RTCDCF77::retrieve()
{
  DSRTC->RAM->readBlock<time_t>(addrPreviousRefTime, status.previousRefTime);    
  DSRTC->RAM->readBlock<time_t>(addrInterval, status.interval);    
  DSRTC->RAM->readBlock<long>(addrDrift, status.drift);    
  DSRTC->RAM->readBlock<unsigned int>(addrUpdates, status.updates);

  // Calculate derivative data
  // correction factor
  status.correctionFactor = (double)status.drift/(double)status.interval;  

}

/**
 * Store drift data to RTC  AM
 */
void RTCDCF77::store()
{
  DSRTC->RAM->writeBlock<time_t>(addrPreviousRefTime, status.previousRefTime);    
  DSRTC->RAM->writeBlock<time_t>(addrInterval, status.interval);    
  DSRTC->RAM->writeBlock<long>(addrDrift, status.drift);             
  DSRTC->RAM->writeBlock<unsigned int>(addrUpdates, status.updates);
}

/**
 * Update RTC with new DCFTime
 * This will also update the drift prediction
 */
bool RTCDCF77::set(time_t curRefTime)
{ 
  // Fetch current RTC time
  time_t curIntTime = DSRTC->get();
  
  // Read previous drift parameters 
  retrieve();  

// Calculate new drift parameters 
  long curDrift = curIntTime - curRefTime;   
  time_t curInterval = curRefTime - status.previousRefTime;
   
  // if interval is too short with respect to 1 sec resolution we will ignore it
  if (curInterval < minInterval) { return false; }  

  // if drift is small with respect to 1 sec resolution we will ignore it
  if (curDrift < minDrift) { return false; }
  
  // Update internal time
  DSRTC->set(curRefTime);
  
  // if interval is too long we will not use it for drift correction
  useInterval = true;
  if (curInterval > maxInterval) { useInterval = false; }
  
  // if correction factor is too large we will also not use it for drift correction
  double corrFactor = (double)curDrift/(double)curInterval;
  if (corrFactor > 0.01) { useInterval = false; } 
  
    
  if (useInterval) {
 
      // Use exponential filtering to update new drift time
      // coeffient determined by halftime and duration of current and previous interval(s)
      double alpha = (double)halfTime/ (double)(curInterval + status.interval);
      if (alpha>1.0) { alpha=1.0;}         
      
      // If not yet accumulated up to halfTime we will just accumulate      
       
      // Calculate filter averages 
      status.drift           = alpha * (curDrift    + status.drift);
      status.interval        = alpha * (curInterval + status.interval);
      status.updates++;             
       
      // Precalculate corrections to be used
      status.correctionFactor = (double)status.drift/(double)status.interval;  
  }
  status.previousRefTime = curRefTime;
        
  // Store new values 
  store();
  
  return true;
}
 
/**
 * Update RTC with new DCFTime
 * This will also update the drift prediction
 */ 
bool RTCDCF77::update()   
{
  // Update RTC clock if there is a new time in the DCF77 buffer      
  bool succes = false;
  if(DCFC->fetchTimeUpdate())    
  {      
    time_t DCFtime = DCFC->getTime();
    succes = set(DCFtime);
  } 
  // See if we should start or stop the DCF77 clock
  startStopDCF();
  return succes;
}

/**
 * Update RTC with new DCFTime
 */ 
void RTCDCF77::startStopDCF()
{
  // Determine if to turn on the DCF77 receiver 
  time_t currentTime = get();
  TimeElements _timeElements;
  breakTime(currentTime, _timeElements);
  if (
       ((long)(currentTime - status.previousRefTime) > minInterval) &&  // if last update longer ago than minInterval ..
       (_timeElements.Hour > 22) &&    (_timeElements.Hour < 4)         // and during evening/night ( better reception) ..       
   )   
   {
        if (!DCFC->isRunning()) { DCFC->Start(); }                      // Start DCF77 clock (if  not running)
   } else {
        if (DCFC->isRunning()) { DCFC->Stop(); }                        // Else, stop DCF77 clock (if running)
   }  
}

/**
 * Get time with drift correction
 */ 
time_t RTCDCF77::get()   
{        
    time_t unCorrectedTime = DSRTC->get();
  
    // Calculate correction factor
    //status.correctionFactor = (double)status.drift/(double)status.interval;  
  
  /*
	Serial.println("unCorrectedTime");   
	Serial.println(unCorrectedTime);   
	Serial.println("status.previousRefTime");   
	Serial.println(status.previousRefTime);   
	Serial.println("status.correctionFactor");   
	Serial.println(status.correctionFactor,5);   
  */
    long tdelta = (long)(unCorrectedTime - status.previousRefTime);
    long tdrift = (long)((double)status.correctionFactor*(double)tdelta);    

  /*
	Serial.println("tdelta");   
	Serial.println(tdelta); 
	Serial.println("tdrift");   
	Serial.println(tdrift);   
  */	
    if (tdelta < maxInterval && status.interval>0) {      
        time_t correctedTime = unCorrectedTime - tdrift;  
        return(correctedTime);      
    }
    return(unCorrectedTime);    
}

/**
 * Get time without drift correction
 */
time_t RTCDCF77::getUncorrected() 
{
  return(DSRTC->get());
}

/**
 * Reset drift settings 
 */
void RTCDCF77::reset()
{    
  DSRTC->RAM->writeBlock<time_t>(addrPreviousRefTime, 0);    
  DSRTC->RAM->writeBlock<time_t>(addrInterval, 0);    
  DSRTC->RAM->writeBlock<time_t>(addrDrift, 0);    
  DSRTC->RAM->writeBlock<time_t>(addrUpdates, 0);

  status.correctionFactor=0;
  retrieve();
  startStopDCF();
}

/**
 * Retreive clock status
 */
Stat RTCDCF77::getStatus()
{    
  retrieve();
  return status;
}



