// read pin 12 write value to pin 13
static const uint8_t READ_PIN = 12;
static const uint8_t WRITE_PIN = 13;

void setup() {
  pinMode(READ_PIN, INPUT);
  pinMode(WRITE_PIN, OUTPUT);
}
void loop() {
  digitalWrite(WRITE_PIN, digitalRead(READ_PIN));
}