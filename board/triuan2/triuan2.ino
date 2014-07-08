#include <EEPROM.h>
#include <digitalWriteFast.h>
#include <messages.h>
#include <thAVR.h>
#include <thVLC.h>

int i=2, PORT_ID[8]= {0, 0, 22, 23, 24, 25, 26, 27};

void setup() {
    Serial.begin(BAUD);
    thVLC.begin();
    Serial.println("Started");
}

void loop() {
    Serial.print("Waiting for handshake message on port ");
    Serial.println(i);
    if(thVLC.receiveReady(i)&&thVLC.receiveResult(i)==HANDSHAKE_MESSAGE) {
        Serial.print("Received handshake message on port ");
        Serial.println(i);
        Serial.print("Now sending port ID on port ");
        Serial.println(i);
        thVLC.sendByte(i, PORT_ID[i]);
    }
    i++;
    if(i==8) i=2;
}
