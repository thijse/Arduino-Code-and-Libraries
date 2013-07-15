#include "ideCompat.h"
#include "lightuino.h"
#include "lightuinoLightSensor.h"

LightSensor::LightSensor(unsigned char panaPin):pin(panaPin) {}    

int LightSensor::read(void) { return analogRead(pin); };    

