#include <digitalWriteFast.h>
#include <message.h>
#include <thAVR.h>
#include <thVLC.h>

unsigned char i=2, PORT_ID[10]= {0}, maxPort=0;

unsigned char isUpsideDown(){
  if (thVLC.getID()==2) return 0;
  unsigned int tmp=0, i;
  for (i=0; i<=10; i++) tmp+=thVLC.sensorRead(10);
  if (tmp/10>=50) return 0; else return 1;  
}

void portIDGen() {
    PORT_ID[0]=0;
    PORT_ID[1]=0;
    unsigned char id=thVLC.getID();
    switch(id) {
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
    for(i=2; i<maxPort; i++) PORT_ID[i]=id*10+i;
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
        if (isUpsideDown()) thVLC.sendByte(i, PORT_ID[i]*5); else thVLC.sendByte(i, PORT_ID[i]);
        unsigned char b=0, a[8]={0};
        char i2=-1;
        while(!thVLC.receiveReady(i))
            delay(10);
        b=thVLC.receiveResult(i);
        if(b==MESSAGE_BEGIN) {
            Serial.println(F("Recceived MESSAGE_BEGIN, begin receiving data"));
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
            delay(1000);
            if(thVLC.receiveReady(a[0])&&thVLC.receiveResult(a[0])==a[1]) {
                Serial.println("Found board");
                if(i2-2==0) {
                    thVLC.sendByte(a[0], IGNORE);
                    thVLC.sendByte(i, SUCCESS);
                } else {
                    unsigned char i3=0;
                    Serial.println(F("Sending bytes"));
                    thVLC.sendByte(a[0], MESSAGE_BEGIN);
                    for(i3=2; a[i3]!=MESSAGE_END; i3++) {
                        Serial.println(a[i3]);
                        thVLC.sendByte(a[0], a[i3]);
                    }
                    thVLC.sendByte(a[0], MESSAGE_END);
                    while(!thVLC.receiveReady(a[0])) delay(10);
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
    if(i>=maxPort) i=2;
}
