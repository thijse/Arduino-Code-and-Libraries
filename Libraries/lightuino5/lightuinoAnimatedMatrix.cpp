/*
  lightuinoAnimatedMatrix - Source-Sink matrix that supports for animations
  Thijs Elenbaas <thijs@contemplated.nl>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  lightuinoAnimatedMatrix is strongly based on lightuinoMatrix by Andrew Stone

  It holds 2 image buffers. While one is being shown, the other can be 
  editted with Pixel function. after editting, you can directly switch 
  to the next image, or blend the next image in.   
*/

#include "ideCompat.h"
#include "lightuino.h"
#include "lightuinoAnimatedMatrix.h"
#include "inttypes.h"
#include <lightuinoTimer2.h>
#include <DigitalPin.h>

lightuinoAnimatedMatrix* ledMatrix= 0;

// lightuinoAnimatedMatrix constructor
lightuinoAnimatedMatrix::lightuinoAnimatedMatrix(LightuinoSink& lht, LightuinoSourceDriver& srcDrvr,unsigned char pstartRow,unsigned char pnumRows):sink(lht),src(srcDrvr)
{

  totTransition = 0;    // Length of the transition cycle
  totPWM        = 0;    // Length of PWM cycle
 
  setupSource();
 
  resetAnimationCounters();
 
  startRow = pstartRow;
  numRows = pnumRows;
  memset(&videoRam1,0,NUMOUTSDIV8ADD1*Lightuino_NUMSRCDRVR);
  memset(&videoRam2,0,NUMOUTSDIV8ADD1*Lightuino_NUMSRCDRVR);
  videoRamCurrent = videoRam1;    // Current image buffer
  videoRamNext    = videoRam2;    // Next image buffer
  videoRamUsed    = videoRam1;    // Current image buffer that is being used
  curRow = numRows+startRow;
  //sink.finishReq=false; 
  sink.finishReq=true;
  ledMatrix=0;
  
  // Start with normal loop
  LightuinoTimer2::set(doLoop);
  stopAutoLoop();  
}

// Reset counters
void inline lightuinoAnimatedMatrix::resetAnimationCounters()
{
  noPWM         = 0; // Position in PWM cycle  
  noTransition  = 0; // Position in transition cycle
  curTransition = 0; // Position in transition cycle, fixed during single PWM run
  noImg1        = 0; // Image 1 counter 	
}

// Setting up sources
void lightuinoAnimatedMatrix::setupSource()
{
  fastPinMode(srcClkPin, OUTPUT);      // sets the srcClkPin as output
  fastPinMode(srcDataPin, OUTPUT);     // sets the srcDataPinas output
  fastPinMode(srcStrobePin, OUTPUT);   // sets the srcStrobePin as output
  fastDigitalWrite(srcStrobePin,HIGH);
}

// Shift the source bits and add new bit
#define shiftSource(bit)               \
  fastDigitalWrite(srcClkPin, LOW);    \
  fastDigitalWrite(srcStrobePin,LOW);  \
  fastDigitalWrite(srcDataPin, bit&1); \
  fastDigitalWrite(srcClkPin, HIGH);   \
  fastDigitalWrite(srcClkPin, LOW);    \   
  fastDigitalWrite(srcStrobePin,HIGH);  

  
// Destructor
lightuinoAnimatedMatrix::~lightuinoAnimatedMatrix()
{	
	stopAutoLoop();			
	sink.finishReq=false;  // Let the sink driver finish off the bit banging
}

// Updates shown image with updated image
void lightuinoAnimatedMatrix::update()
{  	
	// Swap next buffer with current buffer
	unsigned char * videoRamTemp(videoRamCurrent);
    videoRamCurrent = videoRamNext;
    videoRamNext    = videoRamTemp;
	videoRamUsed    = videoRamNext;
	for (int i=0;i<Lightuino_NUMSRCDRVR*NUMOUTSDIV8ADD1;i++) videoRamNext[i]=0;	
} 

// Animates from shown image to updated image
void lightuinoAnimatedMatrix::animate(float duration)
{ 		
	// Increase refresh-rate
	float rate = 9000.0f;
	
	// Reset animation counters
    resetAnimationCounters();
		
	// Update transition times
	totTransition = duration * rate;                  // no of counts for full transition
	totPWM = 14;
	startAutoLoop(rate);	
		
	// Start running animated loop
	LightuinoTimer2::set(doAnimatedLoop);
} 

// Set a single pixel
void lightuinoAnimatedMatrix::pixel(unsigned char x,unsigned char y,unsigned char val)
{
  // Write pixels to next image buffer
  int offset = (y*NUMOUTSDIV8ADD1) + (x>>3);
  
  if (val)
	videoRamNext[offset] |= (1 << (x&7));
  else
	videoRamNext[offset] &= ~(1 << (x&7));
}

// Normal loop
void lightuinoAnimatedMatrix::loop()
{   
	curRow++;
	if (curRow >= startRow+numRows) {
		curRow=startRow;
	}
	sink.set(videoRamCurrent + (curRow*NUMOUTSDIV8ADD1)); 
    sink.finish();
	src.shift((curRow ==startRow) ? 1: 0);	
}

// Animated loop
void lightuinoAnimatedMatrix::animatedLoop()
{   
	curRow++;
	// At end of all lines (sources) of image..
	if (curRow >= startRow+numRows) {
		// Set first startline
		curRow=startRow;
		// Determine which image should be shown
		videoRamUsed = frameToShow() ?  videoRamNext: videoRamCurrent;		
	}
	
	// Strange: inlining fastSetBy32(char* ) makes the loop significantly slower!
	sink.fastSetBy32(videoRamUsed + (curRow*NUMOUTSDIV8ADD1));     
	
	shiftSource((curRow ==startRow) ? 1: 0);
	
	if (curTransition >= totTransition) { returnToNormalLoop(); }	
}

// Determines if frame 0 or 1 should be shown 
bool inline lightuinoAnimatedMatrix::frameToShow()
{			
	// next PWM cycle 
	noTransition++;
	noPWM++;
	
	//if at start of a PWM cycle
    if (noPWM >= totPWM) {	
		curTransition = noTransition;  // used for  blend ratio during PWM
		noPWM = 0;					   // reset PWM counter
		noImg1 = 0;					   // reset image 1 counter
	} 	
	//Determine if next image to show is image 0 or image 1: 
    // Show image 1 if noImg1 percentage is smaller than blend img1-to-img0 ratio	
	// rewrite so we don't need to use fractions (floating point values)    
	// noImg1/noPWM < blendRatio = curTransition/totTransition  -->
	// noImg1 * totTransition <  noPWM * curTransition 
	
	if(((noImg1 * totTransition) < (noPWM * curTransition )) ) {
		// show next frame 
		noImg1++;
		return 1;		    	
	}
	// show current frame
	return 0;	
}

// Go from animated to still loop
void inline lightuinoAnimatedMatrix::returnToNormalLoop()
{
	// Swap next buffer with current buffer	
	LightuinoTimer2::set(doLoop);	
	
	// Decrease to default refresh-rate
	startAutoLoop(500.0f);		
	update();
}

// Static function calling loop once.
void lightuinoAnimatedMatrix::doLoop()
{
	if (ledMatrix) {ledMatrix->loop();}
}

// Static function calling animatedloop once.
void lightuinoAnimatedMatrix::doAnimatedLoop()
{
	if (ledMatrix) {ledMatrix->animatedLoop();}
}

// Start automatic refreshing with custom scanrate
void lightuinoAnimatedMatrix::startAutoLoop(float rate)
{	
	ledMatrix = this;	
	LightuinoTimer2::start(rate);	
}

// Start automatic refreshing of scanlines
void lightuinoAnimatedMatrix::startAutoLoop()
{	
	startAutoLoop(500.0f);	
}

// Stop automatic refreshing of scanlines
void lightuinoAnimatedMatrix::stopAutoLoop()
{
	 if (ledMatrix == this) {
		LightuinoTimer2::stop;
		ledMatrix = 0;
	}  
}



