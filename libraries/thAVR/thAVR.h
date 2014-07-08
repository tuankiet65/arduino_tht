#ifndef THAVR_H
#define THAVR_H

#include <Arduino.h>
#include <avr/eeprom.h>

void avrTimer1ConfigNormal(word TopValue);

#define AVR_TIMER1_TOP(microSecPerTick) (((F_CPU) / 8000000UL) * microSecPerTick)

#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
#define  TIMSK1  TIMSK
#endif

#define avrTimer1OC1AInteruptEnable()   { bitSet(TIMSK1, OCIE1A); }
#define avrTimer1OC1AInteruptDisable()  { TIMSK1 &= ~_BV(OCIE1A); }
#define avrTimer1OC1BInteruptEnable()   { TIMSK1 |=  _BV(OCIE1B); }
#define avrTimer1OC1BInteruptDisable()  { TIMSK1 &= ~_BV(OCIE1B); }

void avrADCClockDivConfig(byte clkDiv);
#define ADC_CLK_DIV_2     1
#define ADC_CLK_DIV_4     2
#define ADC_CLK_DIV_8     3
#define ADC_CLK_DIV_16    4
#define ADC_CLK_DIV_32    5
#define ADC_CLK_DIV_64    6
#define ADC_CLK_DIV_128   7

#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
#define avrPullUpDisable()  { SFIOR |= (_BV(PUD)); }
#else
#define avrPullUpDisable()  { MCUCR |= (_BV(PUD)); }
#endif

#define  avrEepromRead(addr)  eeprom_read_byte((uint8_t*)(addr))
word avrEepromReadWord(word addr);

word avrGetBandgap();
void avrConfigFreq();

#endif