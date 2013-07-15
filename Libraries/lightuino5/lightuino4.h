#ifndef LIGHTUINO4

#define LIGHTUINO
#define LIGHTUINO4

#define Lightuino4_CLOCK_PIN 7
#define Lightuino4_SER_DATA_LEFT_PIN 6
#define Lightuino4_SER_DATA_RIGHT_PIN 4
#define Lightuino4_BRIGHTNESS_PIN 5

#define Lightuino4_SRC_CLOCK_PIN 8
#define Lightuino4_SRC_DATA_PIN 9
#define Lightuino4_SRC_STROBE_PIN 3
#define Lightuino4_SRC_ENABLE_PIN 0xff

#ifndef Lightuino_CLOCK_PIN
#define Lightuino_CLOCK_PIN 7
#define Lightuino_SER_DATA_LEFT_PIN 6
#define Lightuino_SER_DATA_RIGHT_PIN 4
#define Lightuino_BRIGHTNESS_PIN 5

#define Lightuino_SRC_CLOCK_PIN 8
#define Lightuino_SRC_DATA_PIN 9
#define Lightuino_SRC_STROBE_PIN 3
#define Lightuino_SRC_ENABLE_PIN 0xff
#endif

#include "lightuinoSink.h"
#include "lightuinoPwm.h"
#include "lightuinoIR.h"
#include "lightuinoLightSensor.h"
#include "lightuinoSourceDriver.h"
#include "lightuinoMatrix.h"
#include "lightuinoAni.h"

#endif
