#ifndef	THVLC_H
#define THVLC_H

#include <Arduino.h>
#include <thAVR.h>

#define TRI_UAN_0  0
#define TRI_UAN_1  1
#define TRI_UAN_2  2
#define TRI_UAN_3  3
#define TRI_UAN_4  4

class	thVLCClass {
public:
    static void begin();
    static byte getID();

    static int sensorRead(byte pin);
    static boolean receiveReady(byte pin);
    static byte receiveResult(byte pin);

    static void ledWrite(byte pin, byte state);
    static void sendByte(byte pin, byte value);
    static void txSetByte(byte pin, byte value);
    static void txSend();
};

extern thVLCClass thVLC;

#endif
