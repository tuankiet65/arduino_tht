#include <digitalWriteFast.h>
#include <message.h>
#include <thAVR.h>
#include <thVLC.h>
#include <thIR.h>

unsigned char currScore=0, currNum=0, currScreen=0;

void setup() {
    thVLC.begin();
    thLedMatrix.begin();
    thIR.begin();
    scoreInit();
    soundInit();
    currScore=scoreRead();
    soundDelay(1000);
    startSplash();
    numUpdate(0, 0);
}

void loop() {
    unsigned char irSignal=255;
    while(!thIR.receive(&irSignal));
    switch(irSignal) {
        case ZERO:
        case ONE:
        case TWO:
        case THREE:
        case FOUR:
        case FIVE:
        case SIX:
        case SEVEN:
        case EIGHT:
        case NINE:
            if(!currScreen) {
                if (currNum>10) currNum%=10;
                currNum=currNum*10+irSignal;
                numUpdate(currNum, currScreen);
                soundDelay(BUTTON_ACCEPTED);
            }
            break;
        case ONE_HUNDRED_PLUS:
        case TWO_HUNDRED_PLUS:
        case NEXT:
        case VOL_UP:
            if(!currScreen) {
                if(irSignal==ONE_HUNDRED_PLUS) irSignal=10;
                if(irSignal==TWO_HUNDRED_PLUS) irSignal=20;
                if(irSignal==NEXT||irSignal==VOL_UP) irSignal=1;
                currNum+=irSignal;
                if(currNum>99) currNum-=100;
                numUpdate(currNum, currScreen);
                soundDelay(BUTTON_ACCEPTED);
            }
            break;
        case VOL_DOWN:
        case PREV:
            if(!currScreen) {
                if(currNum==0) currNum=99;
                else currNum--;
                numUpdate(currNum, currScreen);
                soundDelay(BUTTON_ACCEPTED);
            }
            break;
        case CHANNEL:
        case CHANNEL_UP:
        case CHANNEL_DOWN:
            if(currScreen) {
                currScreen=0;
                numUpdate(currNum, currScreen);
            } else {
                currScreen=1;
                numUpdate(currScore, currScreen);
            }
            soundDelay(BUTTON_ACCEPTED);
            break;
        case EQ:
            if(!currScreen) {
                currNum/=10;
                numUpdate(currNum, currScreen);
                soundDelay(BUTTON_ACCEPTED);
            }
            break;
        case PLAY_PAUSE:
            goDisplay();
            if (waitScreen())
                if(shapeCheck(currNum)) {
                    checkMarkDisplay();
                    scoreUpdate(currScore, currNum);
                    currScore=scoreRead();
                } else crossMarkDisplay();
            numUpdate(currNum, currScreen);
            break;
    }
}