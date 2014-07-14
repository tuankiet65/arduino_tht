/* 
 * http://eletroarduino.wordpress.com/2013/07/12/ir-receiver-module/
 */

#include <digitalWriteFast.h>
#include "thIR.h"
#include "IRremote.h"

_thIR thIR;
IRrecv IR(12);

void _thIR::begin(){
	pinModeFast(A4, OUTPUT);
	digitalWriteFast(A4, HIGH);
	IR.enableIRIn();
}

unsigned char _thIR::decode(unsigned long value){
	switch (value) {
		case 0xE318261B:
		case 0xFFA25D:
			return CHANNEL_DOWN;
		case 0x511DBB:
		case 0xFF629D:
			return CHANNEL;
		case 0xFFE21D:
		case 0xEE886D7F:
			return CHANNEL_UP;
		case 0x5DEB68F6:
		case 0x52A3D41F:
		case 0xFF22DD:
			return PREV;
		case 0xD7E84B1B:
		case 0xFF02FD:
			return NEXT;
		case 0x20FE4DBB:
		case 0xFFC23D:
			return PLAY_PAUSE;
		case 0xFFE01F:
		case 0xF076C13B:
			return VOL_DOWN;
		case 0xA3C8EDDB:
		case 0xFFA857:
			return VOL_UP;
		case 0xFF906F:
		case 0xE5CFBD7F:
			return EQ;
		case 0xC101E57B:
		case 0xFF6897:
			return ZERO;
		case 0x1592252E:
		case 0x97483BFB:
		case 0xFF9867:
			return ONE_HUNDRED_PLUS;
		case 0xFFB04F:
		case 0xF0C41643:
			return TWO_HUNDRED_PLUS;
		case 0x9716BE3F:
		case 0xFF30CF:
			return ONE;
		case 0xFF18E7:
		case 0x3D9AE3F7:
			return TWO;
		case 0x6182021B:
		case 0xFF7A85:
			return THREE;
		case 0xFF10EF:
		case 0x8C22657B:
			return FOUR;
		case 0xFF38C7:
		case 0x488F3CBB:
		case 0xA5589520:
			return FIVE;
		case 0x449E79F:
		case 0xFF5AA5:
			return SIX;
		case 0x32C6FDF7:
		case 0xFF42BD:
		case 0x607B10CF:
			return SEVEN;
		case 0x1BC0157B:
		case 0xFF4AB5:
			return EIGHT;
		case 0x3EC3FC1B:
		case 0xFF52AD:
			return NINE;
		default:
			return UNDEF;
	}
}

unsigned char _thIR::receive(unsigned char *result){
	decode_results tmpRes;
	if (IR.decode(&tmpRes)){
		IR.resume();
		*result=thIR.decode(tmpRes.value);
		return 1;
	} else return 0;
}
