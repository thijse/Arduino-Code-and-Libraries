#include <EEPROMex.h>

int address = 0;
byte bitNo  = 7;
bool input  = true;
bool output = false;

void setup()
{
  Serial.begin(9600);  
  EEPROM.writeBit(address, bitNo, input);  
  output = EEPROM.readBit(address,bitNo);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(output);
  Serial.println();  
}

void loop()
{
  // Nothing to do during loop
}    

