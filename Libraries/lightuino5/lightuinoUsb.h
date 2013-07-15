#include "fifo.h"
#include "avr/pgmspace.h"

/*?<class name="LightuinoUSB">
  This class handles communications to the Lightuino USB infrastructure.  It contains the same APIs as the "serial" class, except that binary data (in particular 0xff) cannot be transmitted.
  <html>
  A typical use is shown below.  In the first line the object is created.
  <pre>

  #include "lightuino5.h"

  void setup()
    {
     Usb.begin();         // Initialize connection to the USB subsystem
     Serial.begin(9600);  // Only because I am using serial.print() in this example
    }

  void loop(void)
    {
     Usb.println("Open device hardware is c00l!");
     while (Usb.available())  // Read in characters from the USB and output them via the UART.
       {
       char c[2];
       c[1] = 0;
       c[0] = usb.read();
       Serial.print(c);
       }
    }


  </pre>
  </html>
*/

#if defined(__AVR_ATmega328P__)||defined(SIM__AVR_ATmega328P__)  // If its not the 328, its not a Lightuino so I don't need spi stuff since I am not using my USB...

class LightuinoUSB
  {
    public:
    LightuinoUSB(int ssPin = 10) : slaveSelectPin(ssPin), lastMicros(0) {}

    //?<method> Initialize SPI communications to the USB infrastructure.  Please override member variable slaveSelectPin if needed before coalling this function.</method>
    void begin();
    //?<method> Stop communicating with USB over SPI.  You generally would only need to call this if you are using SPI for something else.</method>
    void end();
    //?<method> Print a string out the USB serial port. ASCII only. </method>
    void print(const char* str);
    //?<method> Print a number out the USB serial port. </method>
    void print(unsigned long int i, char base=10);
    //?<method> Print a string out the USB serial port. ASCII only. </method>
    void pgm_print(const char* str);

    // <method> Print a number out the USB serial port. </method>
    //void print(long int i, char format)
    //{ if (i<0) { print("-"); i*=-1;}; print((unsigned long int) i,format); }

    //?<method> Print a number out the USB serial port. </method>
    void println(unsigned long int i, char base=10)
      { print(i,base); print("\n");}
    //?<method> Print a string out the USB serial port with appended carriage return. ASCII only. </method>
    void println(const char* str); 
    //?<method> Print a string out the USB serial port with appended carriage return. ASCII only. </method>
    void pgm_println(const char* str);

    //?<method> Is USB serial input available?</method>
    char available(void);
    //?<method> Return a character from the USB serial input buffer, but do not remove from the buffer.</method>
    int  peek(void);
    //?<method> Read and discard all available input.</method>
    void flush(void);
    //?<method> Return a character from the USB serial input buffer.  Returns -1 if nothing is available.</method>
    int  read(void);
    //?<method> Write a character to the USB serial.</method>
    int  write(char send) { xfer(send); }

    //?<method> (Not standard Arduino Serial) Wait for a character from the USB serial input buffer.</method>
    int  readwait(void);


    //?<method> (internal) Send one character to the USB serial port. </method>
    void xfer(char send);
  protected:

    FifoBuf spiRcv;
    unsigned char slaveSelectPin;
    unsigned long lastMicros;
  };
//?</class>

//?? Lightuino USB global variable (analoguous to "Serial")
extern LightuinoUSB Usb;
#endif
