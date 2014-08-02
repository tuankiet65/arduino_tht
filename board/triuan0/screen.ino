#include <avr/pgmspace.h>

#define RIGHT 4
#define LEFT 0

unsigned char prevNum=255;

const unsigned char PROGMEM WAIT_SCREEN[2][64]={
    {0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 4, 3, 2, 2, 2, 3, 4, 4, 3},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 5, 4, 3, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4, 3, 3, 3, 4, 4}
};

const unsigned char PROGMEM CHECK_MARK[2][16]= {
    {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7},
    {4, 5, 5, 6, 6, 7, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1}
};

const unsigned char PROGMEM CROSS_MARK[16]= {
    0, 0,
    B01000010, 0,
    B00100100, 0,
    B00011000, 0,
    B00011000, 0,
    B00100100, 0,
    B01000010, 0,
    0, 0
};

const unsigned char PROGMEM GO[16]= {
    0, B01111000,
    0, B10000100,
    0, B10100100,
    0, B01101000,
    0, B00000000,
    0, B11110000,
    0, B10010000,
    0, B11110000
};

const unsigned char PROGMEM DIGIT[10][2][19]= {
    {
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 0},
        {1, 2, 3, 4, 5, 6, 7, 1, 7, 1, 7, 1, 2, 3, 4, 5, 6, 7, 0}
    },
    {
        {1, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {0, 1, 2, 3, 3, 2, 1, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0},
        {2, 1, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {0, 1, 2, 3, 3, 2, 1, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 6, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {2, 1, 0, 0, 0, 1, 2, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 2, 3, 4, 5, 5, 5, 5, 4, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {3, 2, 1, 0, 0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 2, 3, 4, 4, 4, 4, 5, 6, 7, 7, 6, 0, 0, 0, 0}
    },
    {
        {3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 3, 2, 1, 0, 0, 0, 0, 0},
        {2, 1, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 4, 0, 0, 0, 0, 0}
    },
    {
        {0, 1, 2, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {1, 2, 3, 3, 2, 1, 0, 0, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 2, 3, 4, 4, 3, 2, 5, 6, 7, 7, 6, 5, 0, 0, 0, 0, 0}
    },
    {
        {2, 1, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0},
        {4, 4, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 7, 6, 0, 0, 0, 0}
    }
};

void startSplash() {
    thLedMatrix.clear();
    unsigned char i, i2=0;
    for(i=0; i<8; i++) {
        for(i2=0; i2<=i; i2++) {
            thLedMatrix.setPixel(i, i2, GREEN);
            thLedMatrix.setPixel(i2, i, GREEN);
        }
        delay(100);
    }
    for(i=0; i<8; i++) {
        for(i2=0; i2<=i; i2++) {
            thLedMatrix.setPixel(i, i2, BLACK);
            thLedMatrix.setPixel(i2, i, BLACK);
        }
        delay(100);
    }
}

void setDigit(unsigned char digit, unsigned char diff, unsigned char color, unsigned char screen) {
    unsigned char i, x, y;
    x=pgm_read_byte(&DIGIT[digit][0][0]);
    y=pgm_read_byte(&DIGIT[digit][1][0]);
    for(i=1; x||y; i++) {
        thLedMatrix.setPixel(x+diff, y, color);
        x=pgm_read_byte(&DIGIT[digit][0][i]);
        y=pgm_read_byte(&DIGIT[digit][1][i]);
    }
    if(screen) {
        thLedMatrix.setPixel(3, 0, RED);
        thLedMatrix.setPixel(4, 0, GREEN);
    } else {
        thLedMatrix.setPixel(4, 0, RED);
        thLedMatrix.setPixel(3, 0, GREEN);
    }
}

void numUpdate(unsigned char n, unsigned char screen) {
    thLedMatrix.clear();
    setDigit(n%10, RIGHT, ORANGE, screen);
    setDigit(n/10, LEFT, GREEN, screen);
    prevNum=n;
}

void checkMarkDisplay() {
    unsigned char i, i2;
    thLedMatrix.clear();
    for (i=0; i<=3; i++) {
        thLedMatrix.clear();
        for(i2=0; i2<16; i2+=2) {
            thLedMatrix.setPixel(pgm_read_byte(&CHECK_MARK[0][i2]), pgm_read_byte(&CHECK_MARK[1][i2]), GREEN);
            thLedMatrix.setPixel(pgm_read_byte(&CHECK_MARK[0][i2+1]), pgm_read_byte(&CHECK_MARK[1][i2+1]), GREEN);
            delay(50);
        }
        delay(500);
    }
}

void goDisplay() {
    unsigned char i;
    thLedMatrix.clear();
    for (i=0; i<=3; i++){
        thLedMatrix.setBitmap(GO);
        delay(300);
        thLedMatrix.clear();
        delay(300);
    }
}

void crossMarkDisplay() {
    unsigned char i;
    thLedMatrix.clear();
    for (i=0; i<=4; i++){
        thLedMatrix.setBitmap(CROSS_MARK);
        delay(250);
        thLedMatrix.clear();
        delay(250);
    }
}

unsigned char waitScreen(){
    unsigned char irSignal=255, i=0, i2=0;
    unsigned long prevMillis, currMillis;
    prevMillis=millis();
    currMillis=prevMillis;
    while (!(thIR.receive(&irSignal)&&(irSignal==PLAY_PAUSE||irSignal==EQ))){
        if (currMillis-prevMillis>=100) {
            if (i2%2) 
                thLedMatrix.setPixel(pgm_read_byte(&WAIT_SCREEN[0][i]), pgm_read_byte(&WAIT_SCREEN[1][i]), BLACK);
            else
                thLedMatrix.setPixel(pgm_read_byte(&WAIT_SCREEN[0][i]), pgm_read_byte(&WAIT_SCREEN[1][i]), GREEN);
            prevMillis=currMillis;
            i++;
            if (i==64) {
              i=0;
              i2++;
            }
        }
        currMillis=millis();
    }
    if (irSignal==PLAY_PAUSE) return 1; else return 0;
}
