#include <digitalWriteFast.h>
#include <messageCoding.h>
#include <thAVR.h>
#include <thVLC.h>

unsigned char i=2, PORT_ID[10]={0}, maxPort=0;

void portIDGen(){
    PORT_ID[0]=0;
    PORT_ID[1]=0;
    int id=thVLC.getID();
    switch (id) {
        case 1:
            maxPort=7;
            break;
        case 2:
        case 3:
            maxPort=8;
            break;
        case 4:
            maxPort=10;
            break;
    }
    for (i=2; i<maxPort; i++) PORT_ID[i]=id*10+i;
    i=2;
}
void setup() {
    Serial.begin(BAUD);
    thVLC.begin();
    portIDGen();
    Serial.println("Started");
}

void loop() {
    Serial.print(F("Waiting for handshake message on port "));
    Serial.println(i);
    if(thVLC.receiveReady(i)&&thVLC.receiveResult(i)==HANDSHAKE_MESSAGE) {
        Serial.print(F("Received handshake on port "));
        Serial.println(i);
        Serial.println(F("Now send port ID"));
        thVLC.sendByte(i, PORT_ID[i]);
        unsigned char a[50]={0}, b=0;
        int i2=-1;
        while(!thVLC.receiveReady(i))
            delay(10);
        b=thVLC.receiveResult(i);
        if (b==MESSAGE_BEGIN){
            Serial.println(F("Received MESSAGE_BEGIN, begin receiving data"));
            while(a[i2]!=MESSAGE_END) {
                i2++;
                while(!thVLC.receiveReady(i))
                    delay(10);
                a[i2]=thVLC.receiveResult(i);
                Serial.println(i2);
                Serial.println(a[i2]);
            }
            Serial.println(F("Done receiving data, now check"));
            thVLC.sendByte(a[0], HANDSHAKE_MESSAGE);
            delay(800);
            if (thVLC.receiveReady(a[0])&&thVLC.receiveResult(a[0])==a[1]){
                Serial.println("Found board");
                if (i2-2==0) {
                    thVLC.sendByte(a[0], IGNORE);
                    thVLC.sendByte(i, SUCCESS);
                } else {
                    unsigned char i3=0;
                    Serial.println(F("Sending bytes"));
                    thVLC.sendByte(a[0], MESSAGE_BEGIN);
                    for (i3=2; a[i3]!=MESSAGE_END; i3++) {
                        Serial.println(a[i3]);
                        thVLC.sendByte(a[0], a[i3]);
                    }
                    thVLC.sendByte(a[0], MESSAGE_END);
                    while (!thVLC.receiveReady(a[0])) delay(10);
                    unsigned char x=thVLC.receiveResult(a[0]);
                    Serial.println(x);
                    thVLC.sendByte(i, x);
                }
            } else {
                Serial.println("Found no board");
                thVLC.sendByte(i, FAIL);
            }
        }
    }
    i++;
    if (i>=maxPort) i=2;
}
