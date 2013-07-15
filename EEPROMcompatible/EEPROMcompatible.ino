#include <EEPROMex.h>

int address = 0;
byte input  = 100;
byte output = 0;

void loop()
{
  // Nothing to do during loop
}    

void setup()
{
  Serial.begin(9600);
  
  EEPROM.write(address, input);  
  output = EEPROM.read(address);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(output);
  Serial.println();
  
}
