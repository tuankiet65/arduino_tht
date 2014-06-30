#include "thAVR.h"
#include <EEPROM.h>

#define ADDFACTOR_EEPROMADDR       508
#define MULFACTOR_EEPROMADDR       510
#define CHECKSUM_EEPROMADDR        502
#define CHECK5V_EEPROMADDR         500

// ********************************************************************** //
void avrTimer1ConfigNormal(const uint16_t TopValue)
{
  TCCR1A = 0; 
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  ICR1  = TopValue; 
  OCR1A = TopValue * 2/3;
  OCR1B = TopValue * 1/3; 
  TCNT1 = 0; 
}
// ********************************************************************** //

// ********************************************************************** //
void avrADCClockDivConfig(byte divisionFactor)
{
  ADCSRA = (ADCSRA & ~0x07) | (divisionFactor & 0x07);
}
// ********************************************************************** //

// ********************************************************************** //
unsigned int avrGetBandgap(void) 
{
  //timer1InteruptDisable();
  ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
   
  ADCSRA |= _BV( ADSC );
  while (((ADCSRA & (1 << ADSC)) != 0));
  delayMicroseconds(10);
  ADCSRA |= _BV(ADSC);
  while (((ADCSRA &(1 << ADSC)) != 0));
  ADMUX = 0x42;     // reselect analog channel 2 
  return ADC;
}
// ********************************************************************** //

// ********************************************************************** //
uint16_t avrEepromReadWord(uint16_t addr)
{
  /*if (addr > 510)   // EEPROM 0f Atmega8: 512 byte 
  {
    return 0;
  } */
  uint16_t readBuff = EEPROM.read(addr) | (EEPROM.read(addr+1) << 8) ;
  return readBuff;
}

// ********************************************************************** //
uint8_t avrDetermineOsccal(uint16_t vccValue)
{
  // read tow factor from eeprom
  uint16_t _addFactor   = avrEepromReadWord(ADDFACTOR_EEPROMADDR);
  uint16_t _mulFactor   = avrEepromReadWord(MULFACTOR_EEPROMADDR);
  uint16_t checkSumWord = avrEepromReadWord(CHECKSUM_EEPROMADDR);
    
  uint16_t checkSum = (_addFactor ^ _mulFactor) ^ 0xAAAA;
  
  // determine the factor to load into OSCCAL 
  if (checkSum != checkSumWord) 
  {
    return 0;
  }
  else
  {
    uint32_t factor32 = _mulFactor;
    factor32 = factor32 * vccValue;
    uint16_t osccal = (factor32 / 256) + _addFactor;
    return (osccal >> 8);
  }
}
// ********************************************************************** //

// ********************************************************************** //
void avrConfigFreq()
{ 
  uint16_t DVcc = avrGetBandgap();
  
  uint8_t osccalValue = avrDetermineOsccal(DVcc);
  
  if (osccalValue != 0)
  {
    OSCCAL = osccalValue;
  }
}
// ********************************************************************** //
