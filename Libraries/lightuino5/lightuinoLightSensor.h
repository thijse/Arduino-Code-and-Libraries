#ifndef lightuinoAmbientLightSensorH
#define lightuinoAmbientLightSensorH

class LightSensor
  {
    public:
    unsigned char pin;
    
    enum {
      DirectSunlight = 150,
      BrightLight = 250,
      IndoorLight = 550,
      Dusk = 850,
      Dark = 1000
    };
    
    LightSensor(unsigned char panaPin=7);
    int read(void);
  };

#endif
