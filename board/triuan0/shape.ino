#include <avr/pgmspace.h>
#include <message.h>

const unsigned char PROGMEM SHAPE[][7][11]={
	{
		{2, 13, 6, 23, 5, 24, 2, 43, 0, 0, 0},
		{9, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0}, {0}, {0}, {0}, {0}
	},
	{
		{2, 185, 3, 24, 0, 0, 0, 0, 0, 0, 0},
		{4, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{7, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{9, 215, 7, 24},
                {0}, {0}, {0}
	},
        {
                {2, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {5, 22, 4, 47, 0, 0, 0, 0, 0, 0, 0},
                {6, 22, 5, 33, 0, 0, 0, 0, 0, 0, 0},
                {9, 65, 0},
                {0}, {0}, {0}
        },
        {
                {2, 235, 2, 13, 0},
                {2, 235, 3, 24, 0},
                {2, 235, 9, 65, 0},
                {9, 165, 7, 24, 0},
                {0}, {0}, {0}
        },
        {
                {4, 22, 0},
                {2, 185, 0},
                {7, 22, 0},
                {9, 215, 9, 13, 0},
                {9, 215, 8, 65, 0},
                {0}, {0}
        },
        {
                {3, 22, 0},
                {8, 12, 0},
                {9, 33, 5, 60, 0},
                {9, 33, 49, 8, 24, 0},
                {0}, {0}, {0}
        },
        {
                {8, 22, 4, 24, 0},
                {9, 165, 0},
                {2, 215, 2, 13},
                {2, 215, 8, 65},
                {0}, {0}, {0}
        },
        {
                {2, 65, 0},
                {5, 23, 4, 36, 3, 48, 0},
                {6, 23, 4, 36, 2, 24, 0},
                {9, 13, 0},
                {0}, {0}, {0}
        },
        {
                {2, 49, 4, 65, 6, 16, 0},
                {4, 22, 4, 37, 4, 22, 0},
                {0}, {0}, {0}, {0}, {0}
        },
        {
                {2, 165, 0},
                {3, 230, 3, 24, 0},
                {4, 225, 4, 65, 0},
                {6, 70, 0},
                {9, 24, 0},
                {0}, {0}
        {
};

unsigned char signalWait(unsigned char port){
    unsigned char i=0;
    Serial.println("Waiting");
    while (!thVLC.receiveReady(port)) {
        delay(100);
        i++;
        if (i==300) return 0;
    }
    return 1;
}

unsigned char shapeCheck(unsigned char num){
        thLedMatrix.setPixel(1, 6, GREEN);
        thLedMatrix.setPixel(3, 6, GREEN);
        thLedMatrix.setPixel(5, 6, GREEN);
	unsigned char i=0, i2=0;
	for (i=0; i<10; i++){
		unsigned char a=pgm_read_byte(&SHAPE[num][i][0]);
		if (a) {
                        thVLC.sendByte(a, pgm_read_byte(&SHAPE[num][i][1]));
                        if (!signalWait(a)) return 0;
			if (thVLC.receiveResult(a)==SUCCESS){
				if (pgm_read_byte(&SHAPE[num][i][2])){
					thVLC.sendByte(a, MESSAGE_BEGIN);
					for (i2=2; pgm_read_byte(&SHAPE[num][i][i2])!=0; i2++) thVLC.sendByte(a, pgm_read_byte(&SHAPE[num][i][i2]));
					thVLC.sendByte(a, MESSAGE_END);
					if (!signalWait(a)) return 0;
					if (thVLC.receiveResult(a)==FAIL) return 0;
				} else thVLC.sendByte(a, IGNORE);
			} else return 0;
		}
	}
	return 1;
}
