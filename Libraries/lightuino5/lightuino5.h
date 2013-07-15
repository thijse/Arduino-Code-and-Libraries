#ifndef LIGHTUINO5

#define LIGHTUINO
#define LIGHTUINO5

#define Lightuino5_CLOCK_PIN 7
#define Lightuino5_SER_DATA_LEFT_PIN 6
#define Lightuino5_SER_DATA_RIGHT_PIN 4
#define Lightuino5_BRIGHTNESS_PIN 5

#define Lightuino5_SRC_CLOCK_PIN 8
#define Lightuino5_SRC_DATA_PIN 4
#define Lightuino5_SRC_STROBE_PIN 6
#define Lightuino5_SRC_ENABLE_PIN 0xff

// If no "default" has been defined, then make this rev the default
#ifndef Lightuino_CLOCK_PIN
#define Lightuino_CLOCK_PIN 7
#define Lightuino_SER_DATA_LEFT_PIN 6
#define Lightuino_SER_DATA_RIGHT_PIN 4
#define Lightuino_BRIGHTNESS_PIN 5

#define Lightuino_SRC_CLOCK_PIN 8
#define Lightuino_SRC_DATA_PIN 4
#define Lightuino_SRC_STROBE_PIN 6
#define Lightuino_SRC_ENABLE_PIN 0xff
#endif

#define Lightuino_USB(x) x

#include "lightuinoUsb.h"
#include "lightuinoSink.h"
#include "lightuinoPwm.h"
#include "lightuinoIR.h"
#include "lightuinoLightSensor.h"
#include "lightuinoSourceDriver.h"
#include "lightuinoMatrix.h"
#include "lightuinoAnimatedMatrix.h"
#include "lightuinoAni.h"

#endif
