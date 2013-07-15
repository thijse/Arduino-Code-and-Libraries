#include "lightuinoSourceDriver.h"
#include "ideCompat.h"
#include <DigitalPin.h>

unsigned int reverse16bits(unsigned int x) {
 unsigned int h = 0;
 unsigned char i = 0;

 for(h = i = 0; i < 16; i++) {
  h = (h << 1) + (x & 1); 
  x >>= 1; 
 }
 return h;
}

LightuinoSourceDriver::LightuinoSourceDriver(unsigned char pclkPin, unsigned char pdataPin, unsigned char pstrobePin, unsigned char penaPin)
{
  fastPinMode(clkPin, OUTPUT);      // sets the digital pin as output
  fastPinMode(dataPin, OUTPUT);     // sets the digital pin as output
  fastPinMode(strobePin, OUTPUT);   // sets the digital pin as output
  fastDigitalWrite(strobePin,HIGH);
}

void LightuinoSourceDriver::set(unsigned int bits, unsigned int depth)
{
  bits = reverse16bits(bits);
  fastDigitalWrite(clkPin, LOW);
  fastDigitalWrite(strobePin,LOW);
  //Usb.println("Setting bits");
  for (char i=0;i<16;i++,bits>>=1)
    {
	//  Usb.println(bits&1);	
      fastDigitalWrite(dataPin, bits&1);
      fastDigitalWrite(clkPin, HIGH);
      fastDigitalWrite(clkPin, LOW);      
    }

  fastDigitalWrite(strobePin,HIGH);
}

void LightuinoSourceDriver::shifts(unsigned int bit, unsigned int depth)
{
  fastDigitalWrite(clkPin, LOW);
  fastDigitalWrite(strobePin,LOW);
  for (char i=0;i<depth;i++)
    {
	  //Usb.println(bit);	
      fastDigitalWrite(dataPin, bit);
      fastDigitalWrite(clkPin, HIGH);
      fastDigitalWrite(clkPin, LOW);      
    }

  fastDigitalWrite(strobePin,HIGH);
}

void LightuinoSourceDriver::off()
{
  if (enaPin != 0xFF) { 
  fastDigitalWrite(enaPin,HIGH);    // OFF
  } 
  
}

void LightuinoSourceDriver::on()
{
  if (enaPin != 0xFF) fastDigitalWrite(enaPin,LOW);    // OFF
}


void LightuinoSourceDriver::shift(unsigned char bit)
{
  fastDigitalWrite(clkPin, LOW);
  fastDigitalWrite(strobePin,LOW);

  fastDigitalWrite(dataPin, bit&1);
  fastDigitalWrite(clkPin, HIGH);
  fastDigitalWrite(clkPin, LOW);      

  fastDigitalWrite(strobePin,HIGH);
}
