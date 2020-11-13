#ifdef ESP82
// From the RobotDYN docs, the 'test esp' sketch
#include <Arduino.h> 
#include <ESP8266WiFi.h>
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
const char *ssid = "RobotWebServer"; // The name of the Wi-Fi network that will be created
const char *password = "";   // The password required to connect to it, leave blank for an open network

void setup() {
  // put your setup code here, to run once:
  pinMode(14,OUTPUT);

  Serial.begin(115200);
  delay(4000);
  Serial.println('\n');

  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  digitalWrite(14,HIGH);
  delay(100);
  digitalWrite(14,LOW);
}

#endif