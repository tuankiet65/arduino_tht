#ifndef THAVR_H
#define THAVR_H

#include <Arduino.h>
#include <EEPROM.h>


void avrTimer1ConfigNormal(const uint16_t TopValue);

#define AVR_TIMER1_TOP(microSecPerTick) (((F_CPU) /  8000000UL) * microSecPerTick)  // 8000000UL <- TCCR1B = _BV(WGM12) | _BV(CS11)

#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
  #define  TIMSK1  TIMSK
#endif

#define avrTimer1InteruptEnable()       { bitSet(TIMSK1, OCIE1A); } //TIMSK1 |=  _BV(OCIE1A); }
#define avrTimer1InteruptDisable()      { TIMSK1 &= ~_BV(OCIE1A); }
#define avrTimer1OCRBInteruptEnable()   { TIMSK1 |=  _BV(OCIE1B); }
#define avrTimer1OCRBInteruptDisable()  { TIMSK1 &= ~_BV(OCIE1B); }


void avrADCClockDivConfig(byte divisionFactor);
#define ADC_CLK_DIV_2     1
#define ADC_CLK_DIV_4     2
#define ADC_CLK_DIV_8     3
#define ADC_CLK_DIV_16    4
#define ADC_CLK_DIV_32    5
#define ADC_CLK_DIV_64    6
#define ADC_CLK_DIV_128   7


// Disable the pull-ups in ALL I/O pins
#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
  #define avrPullUpDisable()  { SFIOR |= (_BV(PUD)); }
#else
  #define avrPullUpDisable()  { MCUCR |= (_BV(PUD)); }
#endif


unsigned int avrGetBandgap(void);
uint16_t avrEepromReadWord(uint16_t addr);
uint8_t avrDetermineOsccal(uint16_t vccValue);
void avrConfigFreq();


#endif
