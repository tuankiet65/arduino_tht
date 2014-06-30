#include <thLedMatrix.h>
#include <SPI.h>
#include <EEPROM.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("This is test!"); 
  
  thLedMatrix.begin();
}

void loop()
{    
  smillingFaceDisplay();
}


void redHeart()
{
//  setBitmap(0, 0x3c, 0x00);
//  setBitmap(1, 0x42, 0x00);
//  setBitmap(2, 0x95, 0x00);
//  setBitmap(3, 0xb1, 0x00);
//  setBitmap(4, 0xb1, 0x00);
//  setBitmap(5, 0x95, 0x00);
//  setBitmap(6, 0x42, 0x00);
//  setBitmap(7, 0x3c, 0x00);
}

void smillingFaceDisplay()
{
  thLedMatrix.setBitmap(0, 0x3c, 0x00);
  thLedMatrix.setBitmap(1, 0x42, 0x00);
  thLedMatrix.setBitmap(2, 0x95, 0x00);
  thLedMatrix.setBitmap(3, 0xb1, 0x00);
  thLedMatrix.setBitmap(4, 0xb1, 0x00);
  thLedMatrix.setBitmap(5, 0x95, 0x00);
  thLedMatrix.setBitmap(6, 0x42, 0x00);
  thLedMatrix.setBitmap(7, 0x3c, 0x00);
  
  delay(1000);
  
  thLedMatrix.setBitmap(0, 0x00, 0x3c);
  thLedMatrix.setBitmap(1, 0x00, 0x42);
  thLedMatrix.setBitmap(2, 0x00, 0x95);
  thLedMatrix.setBitmap(3, 0x00, 0xb1);
  thLedMatrix.setBitmap(4, 0x00, 0xb1);
  thLedMatrix.setBitmap(5, 0x00, 0x95);
  thLedMatrix.setBitmap(6, 0x00, 0x42);
  thLedMatrix.setBitmap(7, 0x00, 0x3c);
  
  delay(1000);
  
  thLedMatrix.setBitmap(0, 0x3c, 0x3c);
  thLedMatrix.setBitmap(1, 0x42, 0x42);
  thLedMatrix.setBitmap(2, 0x95, 0x95);
  thLedMatrix.setBitmap(3, 0xb1, 0xb1);
  thLedMatrix.setBitmap(4, 0xb1, 0xb1);
  thLedMatrix.setBitmap(5, 0x95, 0x95);
  thLedMatrix.setBitmap(6, 0x42, 0x42);
  thLedMatrix.setBitmap(7, 0x3c, 0x3c);
  
  delay(1000);
}

void displayMatrix_example()
{
  
  thLedMatrix.setPixels(7, 7, RED);
  thLedMatrix.setPixels(0, 0, GREEN);
  thLedMatrix.setPixels(0, 7, YELLOW);
  thLedMatrix.setPixels(7, 0, YELLOW);
  
  thLedMatrix.setBitmap(2, 0x00, 0x00);
  thLedMatrix.setBitmap(3, 0x18, 0x00);
  thLedMatrix.setBitmap(4, 0x18, 0x00);
  thLedMatrix.setBitmap(5, 0x00, 0x00);
  delay(500);
  
  thLedMatrix.setPixels(7, 0, GREEN);
  thLedMatrix.setPixels(0, 0, BLACK);
  thLedMatrix.setPixels(7, 7, GREEN);
  
  thLedMatrix.setBitmap(2, 0x3c, 0x00);
  thLedMatrix.setBitmap(3, 0x24, 0x18);
  thLedMatrix.setBitmap(4, 0x24, 0x18);
  thLedMatrix.setBitmap(5, 0x3c, 0x00);
  delay(500);
}

