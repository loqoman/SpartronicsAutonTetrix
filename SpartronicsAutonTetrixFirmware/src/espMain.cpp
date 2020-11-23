// TODO: As this begins to work, seperate segments into the /lib directory
// TODO: How to see serial output from ESP?
#ifdef ESP82

#include <Arduino.h> 
#include <ESP8266WiFi.h>

#include <WebSocketsServer.h>



const char *ssid = "RobotWebServer"; // The name of the Wi-Fi network that will be created
const char *password = "";   // The password required to connect to it, leave blank for an open network

const int LED_PIN = 14;

// --- A Set of Function Defines? (This is something that would normally be in a *.h file, I think) -- 
static void webSocketEvent(uint8_t num, WStype_t type,    // What to do with different types of WS messages 
                            uint8_t *payload, size_t len);// TODO: This does?
static void log(String const &msg);                       // TODO: This does?
static void log(char const *msg);                         // TODO: This does?
static void sendARD(char const *m);                       // TODO: This does? (Send a message to arduino?)
// --- Other defines / keepAlive values
static int sNextPing = 0;
static String sPingBufs[2];

WebSocketsServer webSocket(81);

// Code runs once
void setup() {
    pinMode(LED_PIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

    Serial.begin(115200); 
    delay(100);
    Serial.println('\n');

    WiFi.softAP(ssid, password);             // Start the access point
    log(String("Access Point \"") + ssid + "\" started.");
    log(String("IP address: ") + WiFi.softAPIP().toString()); // THIS VALUE IS IMPORTANT FOR PC-SIDE

    /* init websocket server */
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    log("websocket server started on port 81");
}

// Code runs more than once
void loop() {
    webSocket.loop();       // Keep alive the websocket
    if(Serial.available()) {
        String msg = Serial.readStringUntil('\n'); // \n is a terminal charater in serial, denotes end of message
        if(msg.startsWith("/ard2esp/")) {
            // This is where ping 
            sPingBufs[sNextPing] = msg.c_str() + 9;// What does c_str() return?
            sNextPing = (sNextPing + 1) % 2;       // The remainder when (sNextPing +1) is divided by 2
        }
    }
}

// From tttapa tutorial / Dana's code
// TODO: Could go in /lib?
static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t len){
    switch (type) {   
    case WStype_DISCONNECTED:
        log(String("Disconnected ") + String(num));
        break;
    case WStype_CONNECTED:    // No brackets nesissary?  <-- What do these brakets do?
                              // A new websocket has connected
        {   
            char buf[100];
            IPAddress ip = webSocket.remoteIP(num);
            snprintf(buf, 100,
                "[%u] Connected from %d.%d.%d.%d url: %s", 
                num, ip[0], ip[1], ip[2], ip[3], payload);// Just a facncy way to print something
            log(buf);
            webSocket.sendTXT(num, "Connected");          // Verifying client-side weather or not you're connected
                                                          // On connect, should see 'Connected' in output 
        }   
        break;
    case WStype_TEXT:         // Someone wants to tell us something
        
        // log(String("WSText ") + String(num));
        if(!strncmp((char *) payload, "/ard", 4)) {   // Comparing the first 4 bytes of the message to "/ard"
            sendARD((char *) payload);
            payload[0] = '\0';
            // webSocket.sendTXT(num, (char *) payload);
            webSocket.sendTXT(num, "ok");             // TODO: num = which websocket that is?
        } else
        if(!strncmp((char *) payload, "/esp/getping", 12)) {    // Checking if the client is asking for the esp heartbeat
            webSocket.sendTXT(num, sPingBufs[(sNextPing+1)%2]);
        }   
        else
        if(!strncmp((char *) payload, "/esp?led=", 9)) {   
            int l = atoi((char *) payload+9);     // Get the 9th (10th?) character as an integear (Is this just a cast?)
            digitalWrite(LED_PIN, l ? HIGH : LOW);// Mapping integar l to 1 or 0
                                                  // If l is 1, set pin to HIGH, else set to 0
            webSocket.sendTXT(num, "ok");         // Reading you loud and clear
        }   
        else
            sendARD("/error/unrecognized payload");   // SPEAK THE KING'S ENGLISH
        break;
    case WStype_ERROR:
        log(String("WSError ") + (char *) payload);
        break;
    case WStype_BIN: // currently unsupported
        log(String("WSText ") + String(num));
        break;
    case WStype_FRAGMENT_TEXT_START:  // Assuming these exist to avoid an error when sending a WS we don't have a case for
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    case WStype_PING:
    case WStype_PONG:
        /* unimplemented */
        break;
    }   
}

// TODO: Could go in /lib?
static void sendARD(char const *m) {
    // Serial.print("emsg ");
    Serial.println(m); // Is there only a '/avr2esp' heading and not a '/esp2avr' heading?
}
// TODO: Could go in /lib?

static void log(String const &logmsg) {
    // nb: this will be consumed by arduino code
    Serial.print("/esplog:");
    Serial.println(logmsg);
}

// TODO: Could go in /lib?
static void log(char const *msg) {
    Serial.print("/esplog:");
    Serial.println(msg);
}





#endif