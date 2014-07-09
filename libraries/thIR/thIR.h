#ifndef THIR_H

	#define THIR_H

	#include <digitalWriteFast.h>
	#include "IRremote.h"
	#include <Arduino.h>

	class _thIR{
		public:
			static void begin();
			static unsigned long receive();
	};

	extern _thIR thIR;

#endif