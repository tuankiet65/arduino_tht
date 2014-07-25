#include <digitalWriteFast.h>
#include <message.h>
#include <thAVR.h>
#include <thVLC.h>

unsigned char i=2, PORT_ID[10]={0}, maxPort=0, id=0;

unsigned char isUpsideDown(){
  if (id==2) return 0;
  unsigned int tmp=0, i;
  for (i=0; i<=10; i++) tmp+=thVLC.sensorRead(10);
  if (tmp/10>=50) return 0; else return 1;  
}

void portIDGen() {
    PORT_ID[0]=0;
    PORT_ID[1]=0;
    id=thVLC.getID();
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
    if (id==2){
       PORT_ID[6]=23;
       PORT_ID[7]=22;
         PORT_ID[5]=24;
    }
    i=2;
}
void setup() {
    Serial.begin(BAUD);
    thVLC.begin();
    portIDGen();
    Serial.println("Started");
}

unsigned char signalWait(unsigned char port){
    unsigned char i=0;
    Serial.println("Waiting");
    while (!thVLC.receiveReady(port)) {
        delay(100);
        i++;
        if (i==100) return 0;
    }
    return 1;
}

void loop() {
    Serial.print(F("Waiting for handshake message on port "));
    Serial.println(i);
    unsigned char g=((isUpsideDown())?(PORT_ID[i]*5):(PORT_ID[i]));
    Serial.println(g);
    if(thVLC.receiveReady(i)) {
        if (thVLC.receiveResult(i)==g) {
            Serial.print(F("Received handshake on port "));
            Serial.println(i);
            Serial.println(F("Now send port ID"));
            thVLC.sendByte(i, SUCCESS);
            unsigned char b=0, a[8]={0};
            char i2=-1;
            if (signalWait(i)) b=thVLC.receiveResult(i); else return;
            if(b==MESSAGE_BEGIN) {
                Serial.println(F("Recceived MESSAGE_BEGIN, begin receiving data"));
                while(a[i2]!=MESSAGE_END) {
                    i2++;
                    if (!signalWait(i)) return;
                    a[i2]=thVLC.receiveResult(i);
                    Serial.println(i2);
                    Serial.println(a[i2]);
                }
                Serial.println(F("Done receiving data, now check"));
                thVLC.sendByte(a[0], a[1]);
                delay(1000);
                if(thVLC.receiveReady(a[0])&&thVLC.receiveResult(a[0])==SUCCESS) {
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
                        if (!signalWait(a[0])) return;
                        unsigned char x=thVLC.receiveResult(a[0]);
                        Serial.println(x);
                        thVLC.sendByte(i, x);
                    }
                } else {
                    Serial.println("Found no board");
                    thVLC.sendByte(i, FAIL);
                }
            }
        } else thVLC.sendByte(i, FAIL_PORT);
    };
    i++;
    if(i>=maxPort) i=2;
}
