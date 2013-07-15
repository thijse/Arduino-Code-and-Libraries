#ifndef FIFO_H
#define FIFO_H
#define FIFO_BUF_SIZE 16

typedef struct
{
  char buf[FIFO_BUF_SIZE];
  unsigned char start;
  unsigned char fend;
} FifoBuf;

extern unsigned int fifoPeek(FifoBuf* ths);
extern void fifoPush(FifoBuf* ths, unsigned char c);
extern void fifoCtor(FifoBuf* ths);
extern void fifoWrite(FifoBuf* ths, unsigned char* c, unsigned char len);
extern void fifoPushStr(FifoBuf* ths, char* s);
extern unsigned int fifoPop(FifoBuf* ths);

#endif
