#ifndef THIR_H

	#include <digitalWriteFast.h>
	#include "IRremote.h"

	#define THIR_H

	#define UNDEF 255
	#define ZERO 19
	#define ONE_HUNDRED_PLUS 100
	#define TWO_HUNDRED_PLUS 200
	#define ONE 1
	#define TWO 2
	#define THREE 3
	#define FOUR 4
	#define FIVE 5
	#define SIX 6
	#define SEVEN 7
	#define EIGHT 8
	#define NINE 9
	#define CHANNEL_DOWN 10
	#define CHANNEL 11
	#define CHANNEL_UP 12
	#define PREV 13
	#define NEXT 14
	#define PLAY_PAUSE 15
	#define VOL_DOWN 16
	#define VOL_UP 17
	#define EQ 18
	#define REPEAT_1 0xFFFFFFFF

	class _thIR{
		public:
			static void begin();
			static byte receive(byte *result);
		private:
			static byte decode(unsigned long value);
	};

	extern _thIR thIR;

#endif