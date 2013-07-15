// read pin 12 write value to pin 13
#include <DigitalPin.h>

DigitalPin<12> readPin;
DigitalPin<13> writePin;

void setup() {
  // input mode with pullup disabled
  readPin.config(INPUT, LOW);
  writePin.mode(OUTPUT);
}
void loop() {
  writePin.write(readPin.read());
}