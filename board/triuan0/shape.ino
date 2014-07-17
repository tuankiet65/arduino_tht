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
		{13, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	}
};

unsigned char shapeCheck(unsigned char num){
	unsigned char i=0, i2=0;
	for (i=2; i<10; i++){
		unsigned char a=pgm_read_byte(&SHAPE[num][i-2][0]);
		if (a) {
			thVLC.sendByte(i, HANDSHAKE_MESSAGE);
			delay(1000);
			if (thVLC.receiveReady(i)&&thVLC.receiveResult(i)==a){
				thVLC.sendByte(i, MESSAGE_BEGIN);
				for (i2=1; pgm_read_byte(&SHAPE[num][i-2][i2])!=0; i2++) thVLC.sendByte(i, pgm_read_byte(&SHAPE[num][i-2][i2]));
				while (!thVLC.receiveReady(i)) delay(10);
				if(thVLC.receiveResult(i)==FAIL) return 0; else if(thVLC.receiveResult(i)==SUCCESS) return 1;
			}
		}
	}
}
