#include <EEPROMex.h>

int address;
const int maxAllowedWrites = 10;
const int memBase          = 350;


void setup()
{
    // Note: Be sure that _EEPROMEX_DEBUG is enabled in EEPROMex.h, to enable error checking
    Serial.begin(9600);  
    Serial.println();
     
     // Always get the adress(es) first
     address = EEPROM.getAddress(sizeof(byte));
 
    // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno 
    // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMEX_DEBUG is set
    EEPROM.setMemPool(memBase, EEPROMSizeUno);
  
    // Set maximum allowed writes to maxAllowedWrites. 
    // More writes will only give errors when _EEPROMEX_DEBUG is set
    EEPROM.setMaxAllowedWrites(maxAllowedWrites);
  
    Serial.println("-------------------------------------------------------------");     
    Serial.println("Check if we get errors when writing too much or out of bounds");     
    Serial.println("-------------------------------------------------------------");   


    Serial.println("Trying to write outside of EEPROM memory...");
    EEPROM.writeLong(EEPROMSizeUno+10,1000);
    Serial.println();    
    
    Serial.println("Trying to exceed number of writes...");        
    for(int i=1;i<=20; i++)
    {
        if (!EEPROM.writeLong(address,1000)) { return; }    
    }
    Serial.println();   
}

void loop()
{ } 
