#include <DigitalIO.h>

// class with run time pin number
DigitalIO pin13(13);

void setup() {
  // set mode to OUTPUT and level LOW
  pin13.config(OUTPUT, LOW);
}
void loop() {
  pin13.toggle();
  pin13.toggle();
  delay(1);
}