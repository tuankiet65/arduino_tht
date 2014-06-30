#include <avr/pgmspace.h>
#include <SPI.h>
#include <EEPROM.h>
#include <digitalWriteFast.h>
#include <thVLC.h>
#include <thLedMatrix.h>
#include <thAVR.h>

void setup(){
  thLedMatrix.begin();
}

void loop(){
   thLedMatrix.setPixel(1, 2, GREEN);
   thLedMatrix.clear(); 
}


