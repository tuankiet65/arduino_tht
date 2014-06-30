#ifndef THLEDMATRIX_H
#define THLEDMATRIX_H

#include <Arduino.h>

#define  BLACK   0
#define  RED     _BV(0)
#define  GREEN   _BV(1)
#define  ORANGE  (RED | GREEN)

class _thLedMatrix
{
  public:
    static void begin();
    static void clear();
    static void setPixel(byte x, byte y, byte color);
    static void setColumn(byte x, byte redBitmap, byte greenBitmap);
};

extern _thLedMatrix thLedMatrix;

#endif
