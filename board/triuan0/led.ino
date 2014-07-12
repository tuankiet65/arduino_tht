#include <digitalWriteFast.h>
#include <thAVR.h>
#include <thLedMatrix.h>

void startSplash(){
    thLedMatrix.clear();
    int i, a[2]={GREEN, BLACK}, dl=100, i2=0, i3=0;
    for (i=0; i<2; i++){
        for (i2=0; i2<8; i2++){
            for (i3=0; i3<=i2; i3++) thLedMatrix.setPixel(i2, i3, a[i]);
            for (i3=i2; i3>=0; i3--) thLedMatrix.setPixel(i3, i2, a[i]);
            delay(dl);
        }
    }
}

void setNumber(byte n){
    byte a=n/10, b=n%10;
    switch (b){
        case 0: thLedMatrix.setBitmap(ZERO_RIGHT); break;
        case 1: thLedMatrix.setBitmap(ONE_RIGHT); break;
        case 2: thLedMatrix.setBitmap(TWO_RIGHT); break;
        case 3: thLedMatrix.setBitmap(THREE_RIGHT); break;
        case 4: thLedMatrix.setBitmap(FOUR_RIGHT); break;
        case 5: thLedMatrix.setBitmap(FIVE_RIGHT); break;
        case 6: thLedMatrix.setBitmap(SIX_RIGHT); break;
        case 7: thLedMatrix.setBitmap(SEVEN_RIGHT); break;
        case 8: thLedMatrix.setBitmap(EIGHT_RIGHT); break;
        case 9: thLedMatrix.setBitmap(NINE_RIGHT); break;
    }
    Serial.println(a);
    switch (a){
        case 0: thLedMatrix.setBitmap(ZERO_LEFT); break;
        case 1: thLedMatrix.setBitmap(ONE_LEFT); break;
        case 2: thLedMatrix.setBitmap(TWO_LEFT); break;
        case 3: thLedMatrix.setBitmap(THREE_LEFT); break;
        case 4: thLedMatrix.setBitmap(FOUR_LEFT); break;
        case 5: thLedMatrix.setBitmap(FIVE_LEFT); break;
        case 6: thLedMatrix.setBitmap(SIX_LEFT); break;
        case 7: thLedMatrix.setBitmap(SEVEN_LEFT); break;
        case 8: thLedMatrix.setBitmap(EIGHT_LEFT); break;
        case 9: thLedMatrix.setBitmap(NINE_LEFT); break;
    }
}
