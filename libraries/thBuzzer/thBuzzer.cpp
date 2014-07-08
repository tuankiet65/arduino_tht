#include "thBuzzer.h"
#include <thAVR.h>
#include <digitalWriteFast.h>
#include <avr/pgmspace.h>

_thBuzzer thBuzzer;

void _thBuzzer::begin() {
    pinModeFast(A5, OUTPUT);
}

void _thBuzzer::sound(int ms) {
    digitalWriteFast(A5, HIGH);
    delay(ms);
    digitalWriteFast(A5, LOW);
}