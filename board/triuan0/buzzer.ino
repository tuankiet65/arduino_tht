#include <Arduino.h>
#include <digitalWriteFast.h>

#define BUTTON_ACCEPTED 200

void soundInit(){
	pinModeFast(A5, OUTPUT);
}

void sound(){
	digitalWriteFast(A5, HIGH);
}

void noSound(){
	digitalWriteFast(A5, LOW);
}

void soundDelay(unsigned char ms){
	sound();
	delay(ms);
	noSound();
}