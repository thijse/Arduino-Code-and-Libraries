#include <lightuino5.h>
#include "Utils.h"

//?? Printf-style to the USB

void print(unsigned long int num,char base)
{
 #ifdef LIGHTUINO_USB		
	Usb.print(num,base);
#else
    Serial.print(num,base);
#endif	
}

void print(const char* str)
{
 #ifdef LIGHTUINO_USB		
	Usb.print(str);
#else
    Serial.print(str);
#endif	
}

void println(const char* str)
{
 #ifdef LIGHTUINO_USB		
	Usb.println(str);
#else
    Serial.println(str);
#endif	
}

void println(unsigned long int num,char base)
{
 #ifdef LIGHTUINO_USB		
	Usb.println(num,base);
#else
    Serial.println(num,base);
#endif	
}