#include <EEPROM.h>
#define SCORE_ADDRESS 100

unsigned char scoreRead() {
    return EEPROM.read(SCORE_ADDRESS);
}

void scoreUpdate(unsigned char score) {
    EEPROM.write(SCORE_ADDRESS, score);
}
