/*? <section name="lightuino">
 */ 
#ifndef lightuinoMatrix
#define lightuinoMatrix

#include "lightuinoSink.h"
#include "lightuinoSourceDriver.h"
#include <string.h>

typedef enum
{
  DRAW,
  ERASE,
  DRAW_BLACK_ON_WHITE,
  XOR
} GraphicOperation;

//?<class name="LightuinoMatrix"> Display driver for LED matrices
// This class creates a video RAM that maps bits to LEDs in a matrix.  It provides a loop() function which, if 
// called rapidly and periodically will "drive" the matrix based on the state of the bits in the RAM.
// Functions are available to draw graphics and text to the RAM
// This class creates by default a video ram which represents the largest matrix possible (70x16).  However, if 
// your physical matrix is smaller than this, you may choose to "drive" fewer rows.  This will result in a brighter
// matrix since the refresh will happen sooner.  It is essentially defining the visible window within the total videoRam.
class LightuinoMatrix
{
  public:
  unsigned char videoRam[Lightuino_NUMSRCDRVR*((Lightuino_NUMOUTS/8)+1)];
  LightuinoSink&   sink;
  LightuinoSourceDriver& src;
  unsigned char curRow;
  unsigned char numRows;
  unsigned char startRow;
  //? <ctor>constructor
  // <arg name="lht">Reference to the Lightuino</arg>
  // <arg name="srcDrvr">Reference to the Lightuino source driver object</arg>
  // <arg name="pstartRow">what source driver row 0-16 to start displaying on (if a subset of the full matrix is used)</arg>
  // <arg name="numRows">How many rows are you using?</arg>
  // </ctor>
  LightuinoMatrix(LightuinoSink& lht, LightuinoSourceDriver& srcDrvr,unsigned char pstartRow=0,unsigned char numRows=16);

  //?? Destructor
  ~LightuinoMatrix();
  
  //? <method> Draw the next scan line in the matrix</method>
  void loop(unsigned char amt=1);
  //? <method> Set or Clear one LED in the matrix at position x (columns, or sinks),y (rows or sources)</method> 
  void pixel(unsigned char x,unsigned char y,unsigned char val);
  //? <method> write some text to the matrix </method>
  void print(int x, int y,const char* s,unsigned char spacing=1,GraphicOperation g=DRAW);
  //? <method> clear the matrix, pass 1 to turn it all 'on', 0 to turn it all 'off'</method>
  void clear(unsigned char val) { if (val) val=0xFF; memset(videoRam,Lightuino_NUMSRCDRVR*((Lightuino_NUMOUTS/8)+1),val); } 

};
//?</class>

#endif
//?</section>
