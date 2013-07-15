This is a library for fast digital I/O on the Arduino.

An experimental software SPI class is included in SoftSPI.h.  The only
example for SoftSPI is a scope test for development.

The functions in this library do not clear PWM mode for pins.

Clearing PWM mode for each call has a high overhead and does not address the
fact that pins can be in other special modes such as Serial, I2C, or SPI.

There are two folders, DigitalPin and DigitalIO.  Copy the folders you
wish to use to your libraries folder.

The DigitalPin folder has the DigitalPin class which provides very fast
inline functions. DigitalPin is a template base class and pin numbers
must be specified at compile time.  For 328 pins and low address Mega
pins read() and write() execute in two cycles or 125 ns for a 16 MHz CPU.
This folder also contains fast static functions simular to the Arduino
digitalRead and digitalWrite functions.

The main member functions for the DigitalPin class are:

void config(bool mode, bool level)
void high()
void low()
void mode(bool pinMode)
bool read()
void toggle()
void write(bool value)

The toggle() function toggles the pin level in output mode or toggles
the state of the 20K pullup in input mode.

The Arduino pinMode() function, as of version 1.01, is a mixture of config()
and mode().  The mapping is as follows:

pinMode(pin, INPUT) is the same as DigitalPin::config(INPUT, false).
pinMode(pin, INPUT_PULLUP) is the same as DigitalPin::config(INPUT, true).
pinMode(OUTPUT) is the same as DigitalPin::mode(OUTPUT).


The DigitalPin/examples directory has these programs:

ArduinoReadWrite - read pin 12 and write pin 13 using digitalRead/digitalWrite.

DigitalPinConfigToggle - demonstrate the config() and toggle() member functions.

DigitalPinReadWrite - read pin 12 and write pin 13 using DigitalPin.

testArduino - scope timing test using Arduino digitalWrite function.

testDigitalPin - scope timing test using DitialPin high()/low() functions.

testFastDigital - scope test for static fastDigital functions.

testSoftSPI - scope test for SoftSPI class.


The DigitalIO folder has the DigitalIO class with runtime pin numbers.  
It has member variables to store the pin's bit mask and port address
so it is much slower than the DigitalPin class.

The DigitalIO class has three versions of write.

writeISR() must be used with interrupts disabled since it will not be atomic
if interrupts are enabled.  This is the fastest write function.

write() must be used with interrupts enabled since it disables interrupt
before accessing the pin and then enables interrupts.

writeRestore() restores the interrupt state but is much slower the write()
or writeISR().

DigitalIO is somewhat faster than the Arduino digitalWrite/digitalRead
functions on non-PWM pins and more than twice as fast on PWM pins.

Look at the html for more details and try the examples in DigitalIO/examples.
