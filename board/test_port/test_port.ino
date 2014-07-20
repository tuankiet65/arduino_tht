#include <digitalWriteFast.h>
#include <thAVR.h>
#include <thVLC.h>

void setup(){
   thVLC.begin(); 
}

void loop(){
   for (int i=2; i<=5; i++) thVLC.sendByte(i, 255); 
}
