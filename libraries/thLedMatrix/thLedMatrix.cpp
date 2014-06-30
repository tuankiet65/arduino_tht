#include "thLedMatrix.h"
#include <thAVR.h>
#include <digitalWriteFast.h>
#include <avr/pgmspace.h>

#define  USEC_PER_TICK  (10000 / 8)
#define  _BVuc(int) (unsigned char)(~_BV(int))
#define  COL_0  (_BVuc(7))
#define  COL_2  (_BVuc(6))
#define  COL_4  (_BVuc(5))
#define  COL_6  (_BVuc(4))
#define  COL_7  (_BVuc(3))
#define  COL_5  (_BVuc(2))
#define  COL_3  (_BVuc(1))
#define  COL_1  (_BVuc(0))
#define  LATCH_OE  SS
#define  SPI_SPCR_BASE  (_BV(SPE) | _BV(MSTR))

const unsigned char ColumnTable[8] PROGMEM= {COL_7, COL_0, COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};
const unsigned char bit2val[8] PROGMEM= {COL_1, COL_3, COL_5, COL_7, COL_6, COL_4, COL_2, COL_0};

_thLedMatrix thLedMatrix;
uint8_t DisplayBuffer[2][8];

void _thLedMatrix::begin(){
  digitalWriteFast(LATCH_OE, HIGH); 
  pinModeFast(LATCH_OE, OUTPUT); 
  pinModeFast(SCK, OUTPUT);
  pinModeFast(MOSI, OUTPUT);
  SPSR = _BV(SPI2X);
  SPCR = SPI_SPCR_BASE;
  SPDR = 0;  
  cli();  
  avrTimer1ConfigNormal(AVR_TIMER1_TOP(USEC_PER_TICK));
  avrTimer1OCRBInteruptEnable();
  sei();
  SPDR = 0;  
}


void _thLedMatrix::setColumn(byte x, byte redBitmap, byte greenBitmap){
  DisplayBuffer[0][x] = redBitmap;
  DisplayBuffer[1][x] = greenBitmap;
}


void _thLedMatrix::clear(){
  for (byte x = 0; x < 8; x++) setColumn(x, 0x00, 0x00);
}

void _thLedMatrix::setPixel(byte x, byte y, byte color){  
  uint8_t row = pgm_read_byte_near(bit2val + y);
  uint8_t row_inv = ~row;
  uint8_t *ptr = &DisplayBuffer[0][x];
  if (color & RED) ptr[0] |= row; else ptr[0] &= row_inv;
  if (color & GREEN) ptr[8] |= row; else ptr[8] &= row_inv; 
}


void LedMatrixScan(){   
  static uint8_t column = 0;
  uint8_t byte2 = pgm_read_byte_near(ColumnTable + column);
  SPSR;
  SPDR = byte2;
  uint8_t redBitmap = DisplayBuffer[0][column];
  uint8_t grnBitmap = DisplayBuffer[1][column];   
  uint8_t byte0 = ((redBitmap & 0x55) >> 0) | ((grnBitmap & 0x55) << 1);
  digitalWriteFast(LATCH_OE, HIGH);   
  SPCR = SPI_SPCR_BASE | 0;
  SPSR;
  digitalWriteFast(LATCH_OE, LOW);
  SPDR = byte0;
  uint8_t byte1 = ((redBitmap & 0xAA) >> 0) | ((grnBitmap & 0xAA) >> 1);
  column = (column + 1) & 0x07;
  while (!(SPSR & _BV(SPIF)));
  SPCR = SPI_SPCR_BASE | _BV(DORD);
  SPDR = byte1;
}

SIGNAL(TIMER1_COMPB_vect)
{
  LedMatrixScan();
}