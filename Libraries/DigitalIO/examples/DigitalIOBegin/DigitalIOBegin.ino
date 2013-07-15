// read pin 12 write value to pin 13
// use begin to assign pin number
#include <DigitalIO.h>

DigitalIO readPin;
DigitalIO writePin;

void setup() {
  // Assign pin 12
  readPin.begin(12);
  // input mode with pullup disabled
  readPin.config(INPUT, LOW);

  // Assign pin 13 and set output mode
  writePin.begin(13);
  writePin.mode(OUTPUT);
}
void loop() {
  writePin.write(readPin.read());
}