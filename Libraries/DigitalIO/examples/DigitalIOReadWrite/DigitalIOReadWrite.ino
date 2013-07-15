// read pin 12 write value to pin 13
#include <DigitalIO.h>

DigitalIO readPin(12);
DigitalIO writePin(13);

void setup() {
  // set input mode and disable pullup
  readPin.config(INPUT, LOW);

  // set output mode
  writePin.mode(OUTPUT);
}
void loop() {
  writePin.write(readPin.read());
}