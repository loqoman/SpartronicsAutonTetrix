#ifdef ESP82
// From the RobotDYN docs, the 'test esp' sketch
#include <Arduino.h> 
// I'm not really interested in getting the teest esp sketch to work...
/*
void setup() { 
    Serial.begin(115200);
    pinMode(14,OUTPUT);
    delay(500);
    Serial.println("AT+CIPMUX=1");
    delay(2000);
    Serial.println("AT+CIPSERVER=1,5000");
    delay(2000);
    Serial.println("AT+CIPSTO=3600");
    delay(2000); }

void loop() { 
    while(Serial.available()) { 
        char Rdata;
        Rdata=Serial.read();
        if(Rdata == 'A' | Rdata=='a') { 
            digitalWrite(14,HIGH);
            delay(50); 
        }
        else if(Rdata=='B'| Rdata=='b'){ 
            digitalWrite(14,LOW);
            delay(10);
            digitalWrite(14,HIGH);
            delay(10);
            digitalWrite(14,LOW); 
        } else {
            digitalWrite(14,LOW); } 
        }    
}
*/

void setup() {
  // put your setup code here, to run once:
  pinMode(14,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  digitalWrite(14,HIGH);
  delay(100);
  digitalWrite(14,LOW);
}

#endif