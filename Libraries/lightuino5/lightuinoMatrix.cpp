#include "lightuino.h"
#include "lightuinoMatrix.h"

void LightuinoMatrix::pixel(unsigned char x,unsigned char y,unsigned char val)
    {
      int offset = (y*((Lightuino_NUMOUTS/8)+1)) + (x>>3);
      
      if (val)
      videoRam[offset] |= (1 << (x&7));
        //videoRam[y][x>>3] |= (1 << (x&7));
      else
        videoRam[offset] &= ~(1 << (x&7));
        //videoRam[y][x>>3] &= ~(1 << (x&7)); 
    }
 
LightuinoMatrix::LightuinoMatrix(LightuinoSink& lht, LightuinoSourceDriver& srcDrvr,unsigned char pstartRow,unsigned char pnumRows):sink(lht),src(srcDrvr)
{
  startRow = pstartRow;
  numRows = pnumRows;
  memset(&videoRam,0,((Lightuino_NUMOUTS/8)+1)*Lightuino_NUMSRCDRVR);
  curRow = numRows+startRow;
  sink.finishReq=true; // Let me finish off the sink bit banging so I can control exactly when that happens for maximum brightness.
}

LightuinoMatrix::~LightuinoMatrix()
{
  sink.finishReq=false;  // Let the sink driver finish off the bit banging
}

void LightuinoMatrix::loop(unsigned char amt)
  {
    //unsigned char line[((Lightuino_NUMOUTS/8)+1)];
    unsigned char cnt = amt;
    while(cnt>0)
      {
        cnt--;
        curRow++;
        if (curRow >= startRow+numRows) 
          {
            curRow=startRow;
          }
        sink.set(videoRam + (curRow*((Lightuino_NUMOUTS/8)+1)));       
        src.off();
        sink.finish();
        if (curRow ==startRow) 
          {
            //src.shift(1);
            src.set(1<<startRow);
          }
        else src.shift(0);
        //memcpy(line,videoRam[curRow],((Lightuino_NUMOUTS/8)+1));
        //memset(line,0,((Lightuino_NUMOUTS/8)+1));
        //sink.set(line);
      }
  }
