#include <digitalWriteFast.h>
#include "thIR.h"
#include "IRremote.h"
#include <Arduino.h>

_thIR thIR;
IRrecv IRport(12);

void _thIR::begin(){
	pinModeFast(A4, OUTPUT);
	digitalWriteFast(A4, HIGH);
	IRport.enableIRIn();
}

unsigned long _thIR::receive(){
	decode_results result;
	if (IRport.decode(&result)){
		IRport.resume();
		return result.value;
	} else {
		return 0;
	}
}
