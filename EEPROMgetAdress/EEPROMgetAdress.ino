#include <EEPROMex.h>

int address = 0;

void setup()
{
    Serial.begin(9600);  
  
  
    // Always get the adresses first and in the same order
    int addressByte      = EEPROM.getAddress(sizeof(byte));
    int addressInt       = EEPROM.getAddress(sizeof(int));
    int addressLong      = EEPROM.getAddress(sizeof(long));
    int addressFloat     = EEPROM.getAddress(sizeof(float));
    int addressDouble    = EEPROM.getAddress(sizeof(double));    
    int addressByteArray = EEPROM.getAddress(sizeof(byte)*7);  
    int addressCharArray = EEPROM.getAddress(sizeof(char)*7);  


    Serial.println("-----------------------------------");     
    Serial.println("Following adresses have been issued");     
    Serial.println("-----------------------------------");      
    
    Serial.println("adress \t\t size");
    Serial.print(addressByte);      Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (byte)");
    Serial.print(addressInt);       Serial.print(" \t\t "); Serial.print(sizeof(int));  Serial.println(" (int)");
    Serial.print(addressLong);      Serial.print(" \t\t "); Serial.print(sizeof(long)); Serial.println(" (long)"); 
    Serial.print(addressFloat);     Serial.print(" \t\t "); Serial.print(sizeof(float)); Serial.println(" (float)");  
    Serial.print(addressDouble);    Serial.print(" \t\t "); Serial.print(sizeof(double));  Serial.println(" (double)");    
    Serial.print(addressByteArray); Serial.print(" \t\t "); Serial.print(sizeof(byte)*7); Serial.println(" (array of 7 bytes)");     
    Serial.print(addressCharArray); Serial.print(" \t\t "); Serial.print(sizeof(char)*7); Serial.println(" (array of 7 chars)");    
}

void loop()
{ } 
