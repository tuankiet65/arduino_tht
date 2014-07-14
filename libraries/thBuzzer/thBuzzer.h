#ifndef THBUZZER_H

	#define THBUZZER_H
	#define BUTTON_ACCEPTED 100
	#define BUTTON_INVALID 200

	#include <digitalWriteFast.h>
	#include <Arduino.h>

	class _thBuzzer {
	public:
	    static void begin();
	    static void sound(int ms);
	};

	extern _thBuzzer thBuzzer;

#endif
