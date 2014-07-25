#include <EEPROM.h>
#include <digitalWriteFast.h>
#include <message.h>
#include <thBuzzer.h>
#include <thAVR.h>
#include <thVLC.h>
#include <thLedMatrix.h>
#include <thIR.h>

unsigned char currScore=0, currNum=0, currScreen=0;  

void setup() {
    scoreInit();
    currScore=scoreRead();
    Serial.begin(BAUD);
    thVLC.begin();
    thLedMatrix.begin();
    thBuzzer.begin();
    thIR.begin();
    Serial.println("Started");
    thBuzzer.sound(1000);
    startSplash();
    numUpdate(0, 0);
}

void loop() {
    unsigned char irSignal=255, irSignal2=255;
    while (!thIR.receive(&irSignal));
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
        if(!currScreen){
            if(currNum<10) {
                currNum=currNum*10+irSignal;
                numUpdate(currNum, currScreen);
                thBuzzer.sound(BUTTON_ACCEPTED);
            } else thBuzzer.sound(BUTTON_INVALID);
        }
        break;
    case ONE_HUNDRED_PLUS:
    case TWO_HUNDRED_PLUS:
    case NEXT:
    case VOL_UP:
        if(!currScreen){
            if(irSignal==ONE_HUNDRED_PLUS) irSignal=10;
            if(irSignal==TWO_HUNDRED_PLUS) irSignal=20;
            if(irSignal==NEXT||irSignal==VOL_UP) irSignal=1;
            currNum+=irSignal;
            if(currNum>99) currNum-=100;
            numUpdate(currNum, currScreen);
            thBuzzer.sound(BUTTON_ACCEPTED);
        }
        break;
    case VOL_DOWN:
    case PREV:
        if(!currScreen){
            if(currNum==0) currNum=99;
            else currNum--;
            numUpdate(currNum, currScreen);
            thBuzzer.sound(BUTTON_ACCEPTED);
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
        thBuzzer.sound(BUTTON_ACCEPTED);
        break;
    case EQ:
        if(!currScreen){
            if(currNum<10||currNum%10==0) currNum=0;
            else currNum-=(currNum%10);
            numUpdate(currNum, currScreen);
            thBuzzer.sound(BUTTON_ACCEPTED);
        }
        break;
    case PLAY_PAUSE:
        goDisplay();
        while (!thIR.receive(&irSignal2)||irSignal2!=PLAY_PAUSE);
        if (shapeCheck(currNum)){
            checkMarkDisplay();
            scoreUpdate(currScore, currNum);
            currScore=scoreRead();
        } else crossMarkDisplay();
        delay(1000);
        numUpdate(currNum, currScreen);
        break;
    }
}
