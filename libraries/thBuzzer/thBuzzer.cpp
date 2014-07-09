#include "thBuzzer.h"
#include <digitalWriteFast.h>
#include <Arduino.h>

_thBuzzer thBuzzer;

void _thBuzzer::begin() {
    pinModeFast(A5, OUTPUT);
}

void _thBuzzer::sound(int ms) {
    digitalWriteFast(A5, HIGH);
    delay(ms);
    digitalWriteFast(A5, LOW);
}