#include "lightuinoUsb.h"
#include "ideCompat.h"

#if defined(__AVR_ATmega328P__)  // If its not the 328, its not a Lightuino so I don't need spi stuff since I am not using my USB...

#include "spi.h"
#include "avr/pgmspace.h"

char LightuinoUSB::available(void)
{
  if (spiRcv.fend == spiRcv.start) xfer(0xff);
  return (spiRcv.fend != spiRcv.start);
}

int LightuinoUSB::peek(void)
{
  return fifoPeek(&spiRcv);
}

void LightuinoUSB::flush(void)
{
  while (available()) read();
}

int LightuinoUSB::read(void)
{
  available();  // Trigger a read if needed
  return fifoPop(&spiRcv);
}

int LightuinoUSB::readwait(void)
{
  while (!available()) ;  // Wait for a character
  return fifoPop(&spiRcv);
}


void LightuinoUSB::end()
  {
  disable_spi();
  }

void LightuinoUSB::begin()
  {
    // set the slaveSelectPin as an output:
    pinMode (slaveSelectPin, OUTPUT);
    digitalWrite(slaveSelectPin,LOW);
    fifoCtor(&spiRcv);
    setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK8);
  }


void LightuinoUSB::print(unsigned long int num,char base)
{
  char buf[(sizeof(unsigned long int) * 8)+1];
  unsigned char i=(sizeof(unsigned long int) * 8);

  buf[i] = 0;
  do
    {
      unsigned char temp = (num % base);
      i--;
      buf[i] = (temp<10) ? temp+'0': temp+'A'-10;
      num /= base;
    } while(num);
  print(&buf[i]);
}

void LightuinoUSB::print(const char* str)
{
  while(*str!=0)
    {
      xfer(*str);
      // delayMicroseconds(200); delay moved into xfer
      str++;
    }
}


void LightuinoUSB::println(const char* str)
{
  print(str);
  xfer('\r');
  xfer('\n');
}

void LightuinoUSB::pgm_print(const char* str)
{
  char c;
  while((c = pgm_read_byte(str))!=0)
    {
      xfer(c);
      // delayMicroseconds(200); delay moved into xfer
      str++;
    }
}

void LightuinoUSB::pgm_println(const char* str)
{
  pgm_print(str);
  xfer('\r');
  xfer('\n');
}


void LightuinoUSB::xfer(char s)
{
  if (s != 0xFF)
    {
      // Empirically, 500us/character avoids overrun issues.
      while (micros() - lastMicros < 500)
        delayMicroseconds(5);
      lastMicros = micros();
    }
  uint8_t in = send_spi(s);
  if (in != 0xFF) 
    {
      fifoPush(&spiRcv, in);
      //Serial.print("spi rcvd");
      //Serial.println((int) in);
    }
}


LightuinoUSB Usb;

#endif

#ifdef SIM

char LightuinoUSB::available(void)
{
  return arduinoSim.Serial.available();
}

int LightuinoUSB::peek(void)
{
  return arduinoSim.Serial.peek();
}

void LightuinoUSB::flush(void)
{
  while (available()) read();
}

int LightuinoUSB::read(void)
{
  return arduinoSim.Serial.read();
}

int LightuinoUSB::readwait(void)
{
  while (!available()) ;  // Wait for a character
  return arduinoSim.Serial.read();
}


void LightuinoUSB::end()
  {
  
  }

void LightuinoUSB::begin()
  {
    // set the slaveSelectPin as an output:
    pinMode (slaveSelectPin, OUTPUT);
    digitalWrite(slaveSelectPin,LOW);
    arduinoSim.Serial.begin(9600);
  }


void LightuinoUSB::print(unsigned long int num,char base)
{
  arduinoSim.Serial.print(num,base);
}

void LightuinoUSB::print(const char* str)
{
  arduinoSim.Serial.print(str);
}


void LightuinoUSB::println(const char* str)
{
  arduinoSim.Serial.println(str);
}

void LightuinoUSB::pgm_print(const char* str)
{
  arduinoSim.Serial.print(str);
}

void LightuinoUSB::pgm_println(const char* str)
{
  arduinoSim.Serial.println(str);  
}


void LightuinoUSB::xfer(char s)
{
  warn("Dont call this directly");
}


LightuinoUSB Usb;
#endif
