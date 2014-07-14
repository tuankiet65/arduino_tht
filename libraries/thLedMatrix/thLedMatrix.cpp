#include "thLedMatrix.h"
#include <digitalWriteFast.h>
#include <avr/pgmspace.h>

#define  USEC_PER_TICK  (10000 / 8)

#define  COL_0  (~_BV(7))
#define  COL_2  (~_BV(6))
#define  COL_4  (~_BV(5))
#define  COL_6  (~_BV(4))
#define  COL_7  (~_BV(3))
#define  COL_5  (~_BV(2))
#define  COL_3  (~_BV(1))
#define  COL_1  (~_BV(0))

#define  LATCH_OE  SS

uint8_t DisplayBuffer[2][8];

thLedMatrixClass thLedMatrix;

#define  SPI_SPCR_BASE  (_BV(SPE) | _BV(MSTR))

const int ColumnTable[8] PROGMEM= {COL_7, COL_0, COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};

void thLedMatrixClass::begin() {
    digitalWriteFast(LATCH_OE, HIGH);
    pinModeFast(LATCH_OE, OUTPUT);
    pinModeFast(SCK, OUTPUT);
    pinModeFast(MOSI, OUTPUT);
    SPSR = _BV(SPI2X);
    SPCR = SPI_SPCR_BASE;
    SPDR = 0;
    cli();
    avrTimer1ConfigNormal(AVR_TIMER1_TOP(USEC_PER_TICK));
    avrTimer1OC1BInteruptEnable();
    sei();
    SPDR = 0;
}


void thLedMatrixClass::setColumn(byte x, byte redBitmap, byte greenBitmap) {
    DisplayBuffer[0][x] = redBitmap;
    DisplayBuffer[1][x] = greenBitmap;
}


void thLedMatrixClass::setBitmap(const unsigned char PROGMEM *bitmap) {
    for(byte x = 0; x < 8; x++) {
        byte redBitmap = pgm_read_byte_near(bitmap++);
        byte grnBitmap = pgm_read_byte_near(bitmap++);
        setColumn(x, redBitmap, grnBitmap);
    }
}


void thLedMatrixClass::clear() {
    for(byte x = 0; x < 8; x++)
        setColumn(x, 0x00, 0x00);
}


void thLedMatrixClass::setPixel(byte x, byte y, byte color) {
    uint8_t row = _BV(y);
    uint8_t row_inv = ~row;
    uint8_t *ptr = &DisplayBuffer[0][x];
    if(color & RED)
        ptr[0] |= row;
    else
        ptr[0] &= row_inv;
    if(color & GREEN)
        ptr[8] |= row;
    else
        ptr[8] &= row_inv;
}


void LedMatrixScan() {
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
    while(!(SPSR & _BV(SPIF)));
    SPCR = SPI_SPCR_BASE | _BV(DORD);
    SPDR = byte1;
}

SIGNAL(TIMER1_COMPB_vect) {
    LedMatrixScan();
}
