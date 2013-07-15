#include <DigitalPin.h>

// class with compile time pin number
DigitalPin<13> pin13;

void setup() {
  // set mode to OUTPUT and level LOW
  pin13.config(OUTPUT, LOW);
}
void loop() {
  pin13.toggle();
  pin13.toggle();
  delay(1);
}