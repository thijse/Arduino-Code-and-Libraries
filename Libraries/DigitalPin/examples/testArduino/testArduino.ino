// scope test for write timing
const uint8_t PIN13 = 13;

void setup() {
  // set mode to OUTPUT
  pinMode(PIN13, OUTPUT);
}
void loop() {
  digitalWrite(PIN13, HIGH);
  digitalWrite(PIN13, LOW);
  digitalWrite(PIN13, HIGH);
  digitalWrite(PIN13, LOW);
  delay(1);
}