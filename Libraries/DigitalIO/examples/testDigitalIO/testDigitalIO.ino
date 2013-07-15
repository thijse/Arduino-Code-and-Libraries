// scope test for write timing
#include <DigitalIO.h>

// class with runtime pin numbers
DigitalIO pin(13);

void setup() {
  // set mode to OUTPUT
  pin.mode(OUTPUT);
}
void loop() {
  pin.high();
  pin.low();
  pin.write(1);
  pin.write(0);
  pin.toggle();
  pin.toggle();
  delay(1);
}