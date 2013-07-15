#include "lightuino.h"
#include "lightuinoAni.h"


void Ani::loop(void)
{
  erase();
  next();
  draw();
}

AniSweep::AniSweep(FlickerBrightness& pleds, char pstart, char pend, char pdelayTime,int pintensityChange):leds(pleds)
      {
        dir = (pend-pstart>0)? 1:-1;
        if (dir>0)
        {
        start=pstart;
        end=pend;
        }
        else
        {
          start = pend;
          end = pstart;
        }
        delayTime = pdelayTime;
        intensityChange = pintensityChange;
        cpos = pstart;
        lst = 0;
      }
      
void AniSweep::erase(void)
{
    leds.brightness[cpos] -= intensityChange;
}

void AniSweep::draw(void)
{
    leds.brightness[cpos] += intensityChange;
}

void AniSweep::next(void)
{
  cpos += dir;

  if (cpos > end) { cpos -= (end-start)+1; }
  if (cpos < start) { cpos += (end-start)+1; }
    
  //Serial.println((int)cpos);
}

AniWiper::AniWiper(FlickerBrightness& pleds, char pstart, char nleds, char pdelayTime,int pintensityChange):leds(pleds)
      {
        start=pstart;
        end=pstart + nleds-1;
        delayTime = pdelayTime;
        intensityChange = pintensityChange;
        cpos = start;
        dir = 1;
        lst = 0;
      }

void AniWiper::erase(void)
{
    leds.brightness[cpos] -= intensityChange;
}

void AniWiper::draw(void)
{
    leds.brightness[cpos] += intensityChange;
}

void AniWiper::next(void)
{
  cpos += dir;
  if (cpos >= end) { cpos = 2*end - cpos; dir *= -1; }
  if (cpos <= start) { cpos = 2*start -cpos; dir *= -1; }
  //Serial.println((int)cpos);
}

void AniDraw(Ani& pani)
  {
    Ani* ani = &pani;
    
    while(ani!=0)
      {
        ani->draw();
        ani = ani->lst;
      }
  }

void AniNext(Ani& pani)
  {
    Ani* ani = &pani;
    
    while(ani!=0)
      {
        ani->next();
        ani = ani->lst;
      }
  }

void AniLoop(Ani& pani)
  {
    Ani* ani = &pani;
    
    while(ani!=0)
      {
        ani->erase();
        ani->next();
        ani->draw();
        ani = ani->lst;
      }
  }

void AniErase(Ani& pani)
  {
    Ani* ani = &pani;
    
    while(ani!=0)
      {
        ani->erase();
        ani = ani->lst;
      }
  }

