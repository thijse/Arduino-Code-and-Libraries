#ifndef Utils_h
#define Utils_h

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
#include <Time.h>

//#define DEBUG_BLINK_PIN 13	     // Connected to debug led
//#define DEBUG

#define DEBUG_BLINK_PIN 13	     // Connected to debug led
#define LIGHTUINO_USB	         // Lightuino USB
#define DEBUG


#define intDisable()      ({ uint8_t sreg = SREG; cli(); sreg; })
#define intRestore(sreg)  SREG = sreg 

#ifdef DEBUG
 #ifdef LIGHTUINO_USB
  #define LogLn(fmt, ...) do { Usb.println(fmt); } while (0)
  #define Log(fmt, ...) do { Usb.print(fmt); } while (0)  
 #else
  #define LogLn(fmt, ...) do { Serial.println(fmt); } while (0)
  #define Log(fmt, ...) do { Serial.print(fmt); } while (0)  
 #endif
  #define BlinkDebug(state) digitalWrite(DEBUG_BLINK_PIN, state);
#else
 #define LogLn(fmt, ...) do { } while (0)
 #define Log(fmt, ...) do { } while (0)
#endif

void print(unsigned long int num,char base = 10);
void print(const char* str = "");
void println(const char* str = "");
void println(unsigned long int num,char base = 10);
#endif
