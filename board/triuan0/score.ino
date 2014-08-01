#include <EEPROM.h>
#define SCORE_ADDRESS 100

unsigned char bitmapCache[13]={0};

void scoreInit() {
    unsigned char i;
    for(i=0; i<=12; i++) bitmapCache[i]=EEPROM.read(i);
}

unsigned char scoreRead() {
    return EEPROM.read(SCORE_ADDRESS);
}

void scoreUpdate(unsigned char score, unsigned char shapeNum) {
    unsigned char a=shapeNum/8, b=shapeNum%8;
    if(!(bitmapCache[a]&(1<<b))) {
        bitmapCache[a]|=(1<<b);
        EEPROM.write(SCORE_ADDRESS, score+1);
        EEPROM.write(a, bitmapCache[a]);
    }
}
