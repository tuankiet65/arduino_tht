#include "thAVR.h"

#define  MUL_FACTOR_EEPROM_ADDR  510
#define  ADD_FACTOR_EEPROM_ADDR  508
#define  CHECKSUM_EEPROM_ADDR    502

void avrTimer1ConfigNormal(word TopValue) {
    TCCR1A = 0;
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
    ICR1  = TopValue;
    OCR1A = TopValue * 2/3;
    OCR1B = TopValue * 1/3;
    TCNT1 = 0;
}

void avrADCClockDivConfig(byte clkDiv) {
    ADCSRA = (ADCSRA & (~0x07)) | (clkDiv & 0x07);
}

word avrGetBandgap() {
    ADMUX = (0x01 << REFS0) | (0 << ADLAR) | (0x0E << MUX0);
    ADCSRA |= _BV(ADSC);
    while(ADCSRA & _BV(ADSC));
    delayMicroseconds(10);
    ADCSRA |= _BV(ADSC);
    while(ADCSRA & _BV(ADSC));
    return ADC;
}

word avrEepromReadWord(word addr) {
    return word(avrEepromRead(addr+1), avrEepromRead(addr));
}

void avrConfigFreq() {
    uint16_t addFactor    = avrEepromReadWord(ADD_FACTOR_EEPROM_ADDR);
    uint16_t mulFactor    = avrEepromReadWord(MUL_FACTOR_EEPROM_ADDR);
    uint16_t checkSumWord = avrEepromReadWord(CHECKSUM_EEPROM_ADDR);
    if(0 == (addFactor ^ mulFactor ^ checkSumWord ^ 0xAAAA)) {
        uint16_t bandgap = avrGetBandgap();
        uint32_t factor32 = mulFactor;
        factor32 = factor32 * bandgap;
        uint16_t osccal = (factor32 >> 8) + addFactor;
        OSCCAL = osccal >> 8;
    }
}