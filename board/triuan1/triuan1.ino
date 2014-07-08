#include <EEPROM.h>
#include <digitalWriteFast.h>
#include <messages.h>
#include <thAVR.h>
#include <thVLC.h>

int i=2;
char PORT_ID[7]={0, 0, 12, 13, 14, 15, 16};

void setup(){
   Serial.begin(BAUD);
   thVLC.begin();
   Serial.println("Started");
}

void loop(){
  Serial.print(F("Waiting for handshake message on port "));
  Serial.println(i);
  if (thVLC.receiveReady(i)&&thVLC.receiveResult(i)==HANDSHAKE_MESSAGE){
     Serial.print(F("Received handshake message on port "));
     Serial.println(i);
     Serial.print(F("Sending port number on port "));
     Serial.println(i);
     thVLC.sendByte(i, PORT_ID[i]);
     Serial.println("Waiting for data");
     unsigned char a[50], i2=0;
     while (!thVLC.receiveReady(i)){delay(10);}
     a[i2]=thVLC.receiveResult(i);
     while (a[i2]!=MESSAGE_END){
       while (!thVLC.receiveReady(i)){delay(10);}
       i2++;
       a[i2]=thVLC.receiveResult(i);
       Serial.println(a[i2]);
     }
     Serial.println("End of message received");
     Serial.println("Now check");
     thVLC.sendByte(a[0], HANDSHAKE_MESSAGE);
     delay(1000);
     if (thVLC.receiveReady(a[0])&&thVLC.receiveResult(a[0])==a[1]){
         thVLC.sendByte(i, SUCCESS);
         Serial.println("Board detected");
     } else {
         thVLC.sendByte(i, FAIL); 
         Serial.println("No board detected");
     }
   }
  i++;
  if (i==7) i=2;
}
