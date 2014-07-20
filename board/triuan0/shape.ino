#include <avr/pgmspace.h>
#include <message.h>

const unsigned char PROGMEM SHAPE[][8][10]={
	{
		{13, 6, 23, 5, 24, 2, 43, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{65, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
	{
		{185, 3, 24, 0, 0, 0, 0, 0, 0, 0},
		{0},
		{15, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0},
		{0},
		{75, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0},
		{215, 7, 24}
	}
};

unsigned char signalWait(unsigned char port){
    unsigned char i=0;
    Serial.println("Waiting");
    while (!thVLC.receiveReady(port)) {
        delay(100);
        i++;
        if (i==200) return 0;
    }
    return 1;
}

unsigned char shapeCheck(unsigned char num){
	unsigned char i=0, i2=0;
	for (i=2; i<10; i++){
		unsigned char a=pgm_read_byte(&SHAPE[num][i-2][0]);
		if (a) {
                        thVLC.sendByte(i, a);
                        if (!signalWait(i)) return 0;
			unsigned char b=thVLC.receiveResult(i);
			Serial.println(b);
			if (b==SUCCESS){
				if (pgm_read_byte(&SHAPE[num][i-2][1])){
					thVLC.sendByte(i, MESSAGE_BEGIN);
					for (i2=1; pgm_read_byte(&SHAPE[num][i-2][i2])!=0; i2++) thVLC.sendByte(i, pgm_read_byte(&SHAPE[num][i-2][i2]));
					thVLC.sendByte(i, MESSAGE_END);
					if (!signalWait(i)) return 0;
					if (thVLC.receiveResult(i)==FAIL) return 0;
				} else thVLC.sendByte(i, IGNORE);
			} else return 0;
		}
	}
	return 1;
}
