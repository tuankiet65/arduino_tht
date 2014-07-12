/* 
 * http://eletroarduino.wordpress.com/2013/07/12/ir-receiver-module/
 */

#include <digitalWriteFast.h>
#include "thIR.h"
#include "IRremote.h"

_thIR thIR;
IRrecv IR(12);
byte prevValue;

void _thIR::begin(){
	pinModeFast(A4, OUTPUT);
	digitalWriteFast(A4, HIGH);
	IR.enableIRIn();
}

byte _thIR::decode(unsigned long value){
	if (value==REPEAT_1) return prevValue; else {
		switch (value) {
			case 0xE318261B:
			case 0xFFA25D:
			case 0xAD25B5D6:
			case 0x1A244145:
				prevValue=CHANNEL_DOWN;
				break;
			case 0x511DBB:
			case 0xFF629D:
			case 0x74DF66F2:
			case 0x511DB8:
				prevValue=CHANNEL;
				break;
			case 0xFFE21D:
			case 0xEE886D7F:
			case 0x4AB0F7B5:
			case 0x291CB0C3:
			case 0x7AC30A1C:
			case 0x564BCC4B:
			case 0x70203191:
			case 0x294867B9:
			case 0x60FF419E:
				prevValue=CHANNEL_UP;
				break;
			case 0x5DEB68F6:
			case 0x52A3D41F:
			case 0xFF22DD:
				prevValue=PREV;
				break;
			case 0xD7E84B1B:
			case 0xFF02FD:
			case 0x72B5A8EC:
				prevValue=NEXT;
				break;
			case 0x20FE4DBB:
			case 0xFFC23D:
			case 0xC438EB45:
			case 0x4D8E1829:
			case 0x2162C27A:
			case 0xBBF3CCD6:
			case 0x4300C1D5:
			case 0x749EC680:
			case 0xB613ABA9:
			case 0xF5FFA3A6:
			case 0x72EB3966:
				prevValue=PLAY_PAUSE;
				break;
			case 0xFFE01F:
			case 0xF076C13B:
				prevValue=VOL_DOWN;
				break;
			case 0xA3C8EDDB:
			case 0xFFA857:
				prevValue=VOL_UP;
				break;
			case 0xFF906F:
			case 0xE5CFBD7F:
			case 0x1E0D1202:
			case 0xB257B3E6:
				prevValue=EQ;
				break;
			case 0xC101E57B:
			case 0xFF6897:
				prevValue=ZERO;
				break;
			case 0x1592252E:
			case 0x97483BFB:
			case 0xFF9867:
				prevValue=ONE_HUNDRED_PLUS;
				break;
			case 0xFFB04F:
			case 0xF0C41643:
				prevValue=TWO_HUNDRED_PLUS;
				break;
			case 0x9716BE3F:
			case 0xFF30CF:
				prevValue=ONE;
				break;
			case 0xFF18E7:
			case 0x3D9AE3F7:
			case 0x3380B6E0:
			case 0xFD293D01:
			case 0x2F07F406:
			case 0x6D14A741:
			case 0xAB013206:
				prevValue=TWO;
				break;
			case 0x6182021B:
			case 0xFF7A85:
				prevValue=THREE;
				break;
			case 0xFF10EF:
			case 0x8C22657B:
				prevValue=FOUR;
				break;
			case 0xFF38C7:
			case 0x488F3CBB:
			case 0xA5589520:
				prevValue=FIVE;
				break;
			case 0x449E79F:
			case 0xFF5AA5:
				prevValue=SIX;
				break;
			case 0x32C6FDF7:
			case 0xFF42BD:
			case 0x607B10CF:
				prevValue=SEVEN;
				break;
			case 0x1BC0157B:
			case 0xFF4AB5:
			case 0x5DFEC4C0:
			case 0xAAE00C7F:
			case 0xE8CCACF6:
			case 0x15AC686E:
			case 0x382B9ADF:
			case 0x35CEBA5:
			case 0xD49751B0:
			case 0x47828900:
			case 0xFA4835C7:
				prevValue=EIGHT;
				break;
			case 0x3EC3FC1B:
			case 0xFF52AD:
				prevValue=NINE;
				break;
			default:
				prevValue=UNDEF;
				Serial.println(value, HEX);
				break;
		}
		return prevValue;
	}
}

byte _thIR::receive(byte *result){
	decode_results tmpRes;
	if (IR.decode(&tmpRes)){
		IR.resume();
		*result=thIR.decode(tmpRes.value);
		return 1;
	} else return 0;
}
