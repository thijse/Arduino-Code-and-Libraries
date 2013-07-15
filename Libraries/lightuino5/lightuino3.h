#ifndef LIGHTUINO3

#define LIGHTUINO
#define LIGHTUINO3

#define Lightuino3_CLOCK_PIN 7
#define Lightuino3_SER_DATA_LEFT_PIN 6
#define Lightuino3_SER_DATA_RIGHT_PIN 5
#define Lightuino3_BRIGHTNESS_PIN 4

#define Lightuino3_SRC_CLOCK_PIN 8
#define Lightuino3_SRC_DATA_PIN 9
#define Lightuino3_SRC_STROBE_PIN 10
#define Lightuino3_SRC_ENABLE_PIN 11

#ifndef Lightuino_CLOCK_PIN
#define Lightuino_CLOCK_PIN 7
#define Lightuino_SER_DATA_LEFT_PIN 6
#define Lightuino_SER_DATA_RIGHT_PIN 5
#define Lightuino_BRIGHTNESS_PIN 4

#define Lightuino_SRC_CLOCK_PIN 8
#define Lightuino_SRC_DATA_PIN 9
#define Lightuino_SRC_STROBE_PIN 10
#define Lightuino_SRC_ENABLE_PIN 11
#endif

#include "lightuinoSink.h"
#include "lightuinoPwm.h"
#include "lightuinoIR.h"
#include "lightuinoLightSensor.h"
#include "lightuinoSourceDriver.h"
#include "lightuinoMatrix.h"
#include "lightuinoAni.h"

#endif
