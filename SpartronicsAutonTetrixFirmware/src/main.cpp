#ifdef AVR
// Blink test to verify AVR

#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(19);
  digitalWrite(13,HIGH);
  delay(19);
  digitalWrite(13,LOW);
}

#endif