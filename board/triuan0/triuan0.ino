#include <EEPROM.h>
#include <digitalWriteFast.h>
#include <messages.h>
#include <thBuzzer.h>
#include <thAVR.h>
#include <thVLC.h>
#include <thLedMatrix.h>

int i=2;

void setup() {
    Serial.begin(BAUD);
    thVLC.begin();
    thLedMatrix.begin();
    Serial.println("Started");
    thBuzzer.begin();
}

void loop() {
    Serial.print(F("Sending handshake message on port "));
    Serial.println(i);
    thVLC.sendByte(i, HANDSHAKE_MESSAGE);
    delay(1000);
    if(thVLC.receiveReady(i)) {
        int a=thVLC.receiveResult(i);
        int b=a%10;
        if(a==13) {
            Serial.print(F("Successfully received message from board "));
            Serial.print(a/10);
            Serial.print(F(" port "));
            Serial.println(b);
            Serial.println(F("Sending data"));
            thVLC.sendByte(i, 6);
            thVLC.sendByte(i, 23);
            thVLC.sendByte(i, MESSAGE_END);
            while(!thVLC.receiveReady(i))
                delay(10);
            Serial.println(thVLC.receiveResult(i));
            if(thVLC.receiveResult(i)==FAIL) {
            } else if(thVLC.receiveResult(i)==SUCCESS)
                thBuzzer.sound(1000);
        }
    }
    i++;
    if(i==10) i=2;
}
