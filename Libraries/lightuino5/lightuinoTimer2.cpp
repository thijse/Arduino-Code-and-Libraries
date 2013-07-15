/*
  lightuinoTimer2 - Using timer2 with autmatically chosen prescaler
  Thijs Elenbaas <thijs@contemplated.nl>

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
  
  lightuinoTimer2 class is based on information from
  - http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
  - http://electronics.stackexchange.com/questions/26363/how-do-i-create-a-timer-interrupt-with-arduino
  - http://www.atmel.com/Images/doc8271.pdf, 18.10 
*/

#include "ideCompat.h"
#include "lightuino.h"
#include <lightuinoTimer2.h>
#include <avr/interrupt.h>

unsigned char LightuinoTimer2::timerLoadValue;

void (*LightuinoTimer2::func)();

void LightuinoTimer2::set(void (*f)()) {		
	func = f;	
}

int LightuinoTimer2::prescalerToMultiplier(int myReg)
{
	int multip[7] = {1,8,32,64,128,256,1024};	
	int reg = min(7,max(1,myReg));
	return multip[reg-1];
}

int LightuinoTimer2::calcPrescaler(float rate)
{
	double period = 1/rate;
	// Loop from highest resolution to lowest
	for ( int reg = 1; reg < 7; reg++ ) {	
		// Check if period can be reached with this resolution
		double  maxPeriod =  (prescalerToMultiplier(reg) * 255.0)/ F_CPU ;
		if (period < maxPeriod) { return reg; }
	}
	return 7; // coarsest resolution
}

void LightuinoTimer2::start(float rate)
{
  // Calculate best prescaler
  byte prescaler = calcPrescaler(rate);
  float multiplier = prescalerToMultiplier(prescaler);
   
  //Calculate the timer load value
  timerLoadValue=(int)((256.0-(F_CPU/(rate*multiplier)))+0.5); //the 0.5 is for rounding;

  //Usb.print("rate : ");
  //Usb.println((16*1000000)/ (multiplier * (255 - timerLoadValue))); 
  
  
  TCCR2A = 0;			// Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = prescaler;   // Timer2 Control Reg B: Timer Prescaler 
  TIMSK2 = 1<<TOIE2; 	// Timer2 INT Reg: Overflow Interrupt Enable
  TCNT2=timerLoadValue; // Reset Timer Count, load for its first cycle
}

void LightuinoTimer2::stop() {
    TCCR2B = 0;  // Stop the timer/counter 
}

ISR(TIMER2_OVF_vect) 
{
  //Capture the current timer value. This is how much error we have
  //due to interrupt latency and the work in this function
  //unsigned int timerLoad=TCNT2+LightuinoTimer2::timerLoadValue;
  unsigned int timerLoad=LightuinoTimer2::timerLoadValue;
  
  //Call function
  (*LightuinoTimer2::func)(); 
  
  //Reload the timer and correct for latency.  
  TCNT2=timerLoad; 
}




