#include <EEPROMex.h>

int address = 0;
double input  = 101.101;
double output = 0;

void setup()
{
  Serial.begin(9600);  
  EEPROM.writeDouble(address, input);  
  output = EEPROM.readDouble(address);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(output);
  Serial.println();  
}

void loop()
{
  // Nothing to do during loop
}    

