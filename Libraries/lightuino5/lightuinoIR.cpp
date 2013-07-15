#include "lightuino.h"
#include "lightuinoIR.h"
#include "ideCompat.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>


// IR receiver globals
char IrReceiver::pin;
unsigned long int IrReceiver::zeroTimeMin;
unsigned long int IrReceiver::zeroTimeMax;
unsigned long int IrReceiver::oneTimeMin;
unsigned long int IrReceiver::oneTimeMax;
unsigned long int IrReceiver::startTime;
unsigned long int IrReceiver::quietTime;

char IrReceiver::signalState;
Lightuino_IR_CODETYPE IrReceiver::codes[Lightuino_IR_CODEBUFLEN];
char IrReceiver::lastCode=1;
char IrReceiver::firstCode=0;

volatile boolean codeReady;
volatile Lightuino_IR_CODETYPE irCode=0;
volatile boolean lastState=0;
volatile unsigned long int lastTime=0;
//volatile boolean p13state=0;

// Return the difference between now and some start time
unsigned long int delta(unsigned long int start, unsigned long int now)
{
  unsigned long int tmp = now-start;
  if (start < now) return tmp;
  else 
  {
    return 0xFFFFFFFFUL - tmp;
  }
}

void pushCode()
{
  if (IrReceiver::lastCode != IrReceiver::firstCode)
    {
      IrReceiver::codes[IrReceiver::lastCode] = irCode;  // Stick it on the queue
      IrReceiver::lastCode++;
      if (IrReceiver::lastCode >= Lightuino_IR_CODEBUFLEN) IrReceiver::lastCode=0;  // wrap it around
    }
      
  //digitalWrite(13,p13state);
  //p13state=!p13state;
  irCode = 0;
}

#ifndef SIM

ISR( sleepWakeup)
{
}

ISR( irHandler )
{
  // When are we?
  unsigned long int now = micros();
  // What are we now?
  boolean state;
  boolean check=true;
 
  if (IrReceiver::signalState != -1) 
    {
      state = digitalRead(2);
      // If its not the signal state now, then it WAS signal before this interrupt fired
      // and that's the interval we are interested in.
      if (state == IrReceiver::signalState) check = false;
    }
  
  if (check)
    {
    // How long since last time?
    unsigned long int interval = delta(lastTime, now);  
    
    // Did we get a "HEY WAKE UP!!!" signal?
    if ((interval > IrReceiver::startTime)&&irCode)
    {
      pushCode();
    }
    
    // If we are within the range for a zero then shift a 0 in. 
    if ((interval >= IrReceiver::zeroTimeMin)&&(interval <=IrReceiver::zeroTimeMax))
    {
    irCode *=2;
    }
    // Or if we are within the range for a 1 then shift in a one.
    else if ((interval >= IrReceiver::oneTimeMin)&&(interval <= IrReceiver::oneTimeMax))
    {
    irCode *=2;
    irCode |=1;
    }
    // Otherwise I don't know what!    
    }
    
  // Remember when we were woken up for the next time.   
  lastTime = now;
}
#else
void sleepWakeup(void)
{
}
void irHandler(void)
{
}

#endif

IrReceiver::~IrReceiver()
{
  detachInterrupt(pin-2);
}

void IrReceiver::sleepUntil(unsigned long int wakeCode)
{
  unsigned long int code=0;
  do
    {
      attachInterrupt(pin-2,sleepWakeup,RISING);
      set_sleep_mode(SLEEP_MODE_IDLE);  //PWR_DOWN);
      sleep_enable();
      power_adc_disable();
      power_spi_disable();
      power_timer0_disable();
      power_timer1_disable();
      power_timer2_disable();
      power_twi_disable();
      attachInterrupt(pin-2,sleepWakeup,RISING);  // Attach our wakeup interrupt
      sleep_mode();  // Goodnight
  
      // Once we get here, it has woken up!
      sleep_disable();
      power_all_enable();

      // Put the IR interrupt back   
      attachInterrupt(pin-2,irHandler,CHANGE);
 
      if (wakeCode==0) break;  // user wants control whenever there is any IR

      byte i=0;
      while(i<50)  // Wait for whatever woke me up to finish xmitting.
        {
          delay(100);
          code = read();
          if (code == wakeCode) break;  // Sift thru garbage for 5 seconds, if we get our code we are done
          i++;
        }
    } while (code != wakeCode);  // Loop if I didn't get the right wakeup code.
}


Lightuino_IR_CODETYPE IrReceiver::read()
{
  Lightuino_IR_CODETYPE ret=0;

  cli();
  unsigned long int interval = delta(lastTime, micros());
  if (interval>IrReceiver::startTime)
    {
      pushCode();
    }
  
  char temp = IrReceiver::firstCode+1;
  if (temp >= Lightuino_IR_CODEBUFLEN) temp=0;
  if (temp != IrReceiver::lastCode)
    {
      ret = IrReceiver::codes[temp];
      IrReceiver::codes[temp] = 0;
      IrReceiver::firstCode = temp;
    }
  
  sei();
  
  return ret;
}

IrReceiver::IrReceiver(unsigned long int ZeroTime,unsigned long int OneTime,unsigned long int StartTime,unsigned long int QuietTime,unsigned int Variation,char SignalState, char Pin)
  {
    zeroTimeMin=ZeroTime-(ZeroTime/Variation); zeroTimeMax = ZeroTime+(ZeroTime/Variation);
    oneTimeMin=OneTime-(OneTime/Variation); oneTimeMax = OneTime+(OneTime/Variation);

    for (char c=0;c<Lightuino_IR_CODEBUFLEN;c++) codes[c]=0;
    lastCode=1;
    firstCode=0;

    
    startTime=StartTime; quietTime=QuietTime;
    signalState=SignalState;
    pin = Pin;
    attachInterrupt(pin-2,irHandler,CHANGE);

  }
