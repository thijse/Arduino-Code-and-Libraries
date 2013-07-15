/*? <section name="lightuino">
 */ 
#ifndef MATRIXPWM_h
#define MATRIXPWM_h

#define M5451_CLK 1
#define Lightuino_NUM_SINKS 70
#define M5451_NUMOUTS 35

#define delayOneCycle  \
   asm volatile("nop"); \

#include "lightuino.h"
#include "lightuinoSink.h"
#include <string.h>
#include <FlexiTimer2.h>
#include <DigitalPin.h>

//?<class name="lightuinoAnimatedMatrix"> Display driver for LED matrices
// This class creates a video RAM that maps bits to LEDs in a matrix.  It provides a loop() function which, if 
// called rapidly and periodically will "drive" the matrix based on the state of the bits in the RAM.
// Functions are available to draw graphics and text to the RAM
// This class creates by default a video ram which represents the largest matrix possible (70x16).  However, if 
// your physical matrix is smaller than this, you may choose to "drive" fewer rows.  This will result in a brighter
// matrix since the refresh will happen sooner.  It is essentially defining the visible window within the total videoRam.
class lightuinoAnimatedMatrix
{
	private:
	
		static const unsigned char srcClkPin    = Lightuino_SRC_CLOCK_PIN;
		static const unsigned char srcDataPin   = Lightuino_SRC_DATA_PIN;
		static const unsigned char srcStrobePin = Lightuino_SRC_STROBE_PIN;
		static const unsigned char srcEnaPin    = Lightuino_SRC_ENABLE_PIN;		
		
		static const unsigned int NUMOUTSDIV8ADD1 = (Lightuino_NUMOUTS/8)+1; // often used calculation
		unsigned char videoRam1[Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1];  // image buffer 1
		unsigned char videoRam2[Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1];  // image buffer 2
		
		unsigned char * videoRamCurrent; // Pointer to current image buffer
		unsigned char * videoRamNext;    // Pointer to next image buffer
		unsigned char * videoRamUsed;    // Pointer to current image buffer that is being used
		LightuinoSink&   sink;		     // Sink driver
		LightuinoSourceDriver& src;      // Source driver
			
		unsigned char curRow;            // Current row (source) being shown
		unsigned char numRows;           // Number of rows (sources) that are used
		unsigned char startRow;	         // First row (source) that is used					
		int noPWM;                       // Position in PWM cycle
		int totPWM;                      // Length of PWM cycle
		int noTransition;                // Position in transition cycle
		int curTransition;               // Position in transition cycle, fixed during single PWM run
		int totTransition;               // Length of the transition cycle
		int noImg1;                      // Image 1 counter 	
  
        // Source
		void setupSource();
		void shiftSource(unsigned char bit);

  
		static void doLoop();			 // Static call to loop()
		static void doAnimatedLoop();    // Static call to animatedLoop();
		void resetAnimationCounters();   // Reset counters
		bool frameToShow();              // Logic to determine which image to show, during PWM loop
		
		void returnToNormalLoop();
		
	public:
		//? <ctor>constructor
		// <arg name="lht">Reference to the Lightuino</arg>
		// <arg name="srcDrvr">Reference to the Lightuino source driver object</arg>
		// <arg name="pstartRow">what source driver row 0-16 to start displaying on (if a subset of the full matrix is used)</arg>
		// <arg name="numRows">How many rows are you using?</arg>
		// </ctor>
		lightuinoAnimatedMatrix(LightuinoSink& lht, LightuinoSourceDriver& srcDrvr,unsigned char pstartRow=0,unsigned char numRows=16);

		//?? Destructor
		~lightuinoAnimatedMatrix();
		
		//? <method> Draw the next scan line in the matrix</method> 
		void loop();
		//? <method> Draw the next scan line in the matrix, while animating from image 1 to 2</method> 
		void animatedLoop();
		//? <method> Set or Clear one LED in the matrix at position x (columns, or sinks),y (rows or sources)</method> 
		void pixel(unsigned char x,unsigned char y,unsigned char val);  
		//memset(&videoRamCurrent,Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1,val); memset(&videoRamNext,Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1,val); }
		//? <method> clear the current  image, pass 1 to turn it all 'on', 0 to turn it all 'off'</method>
		void clearCurrent(unsigned char val) { if (val) val=0xFF; for (int i=0;i<Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1;i++) videoRamCurrent[i]=0; }		
		//memset(&videoRamCurrent,Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1,val); } 
		//? <method> clear the next image, pass 1 to turn it all 'on', 0 to turn it all 'off'</method>
		void clearNext(unsigned char val) { if (val) val=0xFF; for (int i=0;i<Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1;i++) videoRamNext[i]=0; }		
		//memset(&videoRamNext,Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1,val); } 
		//? <method> clear both the current and next image, pass 1 to turn it all 'on', 0 to turn it all 'off'</method>
		void clear(unsigned char val) { clearCurrent(val); clearNext(val); }
		//? <method>Start automatic looping</method>
		void startAutoLoop();
		//? <method>Start automatic looping with custom frequency</method>
		void startAutoLoop(float rate);		
		//? <method>Stop automatic looping</method>
		void stopAutoLoop();
		//? <method>Show image 2 as the current image</method>
		void update();
		//? <method>Animate from image 1 to image 2 during a set time. Finally, make image 2 as the current image</method>
		void animate(float duration);				
		
		void animatedTestLoop();
		
};
//?</class>

#endif
//?</section>
