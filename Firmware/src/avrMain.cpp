#ifdef AVR

// After success with ESP code, I'm going to try to store MachineState.h under /lib for PIO *first*
// If that doesn't work, I'll just put the .h file in 'src' (Which is a big no-no)
// UPDATE: It seems to just work tm
#include <Arduino.h>
#include <string.h>

#include <MachineState.h>

#include <SabertoothSimplified.h>

// Function defines
static int parseUrlParams(char *queryString, char *results[][2],
                        int resultsMaxCt, boolean decodeUrl);
static void percentDecode(char *src);
static void log(char const *msg);
static void log(String msg);
static void sendESP(char const *m);

/* --------------------------------------------------------------------*/
static const unsigned long sMotorSafetyInterval = 1000 * 20; // secs      This is set very large because you don't want to overlap it in testing?
static const unsigned long sESPUpdateInterval = 500; // 2 updates/second  Heartbeat from ESP requirement?
static bool sRunning = false;
static bool sVerbose = true;
static MachineState sMachineState; // Create a MachineState object

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
  
    // SabertoothTXPinSerial.begin(9600) This goes here

    log("AVR Booting up...");
}


void loop() {

    unsigned long now = millis(); // t0 of loop

    sMachineState.update(now);    // call `update()` for our own machineState

    while (Serial.available()) {  // process multiple messages
                                  // NOTE: This language suggests that the robot WILL CRASE if serial goes down
    
        sMachineState.lastchange = now; 
        // we could process one-byte at a time if the variable timing
        // of this blocking call is a problem.
        String msg = Serial.readStringUntil('\n'); 

        if(msg.startsWith("/ard/ping")) { 
            sendESP(sMachineState.serialize("ping")); // Takes the previx ("ping") and prepends it the robot status
        } else 
        if(msg.startsWith("/ard")) {// ignore elog message 
                                    // Decoding Arduino-Speicif messages
          char const *query = msg.c_str() + 5; // Skip the first 5 bytes
          char buf[100];
          char *params[10][2]; 

          strncpy(buf, query, 100);
          int n = parseUrlParams(buf, params, 10, false); // n = number of paramaters
                                                          // AND puts the paramaters in params[][]
          for(int i=0;i<n;i++) {                // Checking each paramater for a verbose setting (?)
              char *param = params[i][0];       
              char *value = params[i][1];
              if(!strcmp(param, "verbose")) { 
                  sVerbose = (value[0] == '1');
                  break;
              }
          }
          if(sVerbose)  
              log(msg);
          for(int i=0;i<n;i++) {          // For each paramater 
              char *param = params[i][0];
              char *value = params[i][1];
              if(!strcmp(param, "led")) { // LED settings
                  sMachineState.led = (value[0] == '1');
                  digitalWrite(LED_BUILTIN, sMachineState.led ? HIGH : LOW);
              }
              else
              if(!strcmp(param, "M1")) { // Motor 1 settings
                  int ival = atoi(value);
                  sMachineState.M1.set(ival, now);
                  if(0 != ival)
                      sRunning = true;
                  // set motor value
              }
              else
              if(!strcmp(param, "M2")){ // Motor 2 settings
                  int ival = atoi(value);
                  sMachineState.M2.set(ival, now);
                  if(0 != ival)
                      sRunning = true;
                  // set motor value
              }
              else
              if(!strcmp(param, "verbose")) {}  // handled above
              else
              if(param[0] != '\n' &&    // Erorr-checking our paramaters
                  param[0] != '\r' &&
                  param[0] != '\0') {
                  log(String("unknown parameter '") 
                              + String(param) + "'");
              }
            }
        }
        else log(msg); // We don't know what happened, so just spew it
    }
    if(sRunning && sMachineState.lastchange != 0 && 
        (now - sMachineState.lastchange) > sMotorSafetyInterval) {
        log("ALERT: lost connection with control");
        // turn off motors here
        sRunning = false;
    }
}

static int
parseUrlParams(char *queryString, char *results[][2],   // This basically takes a WS-style URL string and finds the number of paramaters passed
              int resultsMaxCt, boolean decodeUrl) {
    int ct = 0;
    while (queryString && *queryString && ct < resultsMaxCt) {
        results[ct][0] = strsep(&queryString, "&");
        results[ct][1] = strchrnul(results[ct][0], '=');
        if (*results[ct][1]) *results[ct][1]++ = '\0';
        if (decodeUrl) {
            percentDecode(results[ct][0]);
            percentDecode(results[ct][1]);
        }
        ct++;
    }
    return ct;
}


/**
 * Perform URL percent decoding.
 * Decoding is done in-place and will modify the parameter.
 * NB: if we're using direct websocket communication there
 * may be no need to encode our parameters inwhich case
 * this function shouldn't be called (see above).
 */
static void percentDecode(char *src)
{
  char *dst = src;

  while (*src) {
    if (*src == '+') {
      src++;
      *dst++ = ' ';
    }
    else if (*src == '%') {
      // handle percent escape

      *dst = '\0';
      src++;

      if (*src >= '0' && *src <= '9') {
        *dst = *src++ - '0';
      }
      else if (*src >= 'A' && *src <= 'F') {
        *dst = 10 + *src++ - 'A';
      }
      else if (*src >= 'a' && *src <= 'f') {
        *dst = 10 + *src++ - 'a';
      }

      // this will cause %4 to be decoded to ascii @, but %4 is invalid
      // and we can't be expected to decode it properly anyway

      *dst <<= 4;

      if (*src >= '0' && *src <= '9') {
        *dst |= *src++ - '0';
      }
      else if (*src >= 'A' && *src <= 'F') {
        *dst |= 10 + *src++ - 'A';
      }
      else if (*src >= 'a' && *src <= 'f') {
        *dst |= 10 + *src++ - 'a';
      }

      dst++;
    }
    else {
      *dst++ = *src++;
    }

  }
  *dst = '\0';
}

static void log(String msg)
{
    Serial.print("/ardlog:");
    Serial.println(msg);
}

static void log(char const *msg)
{
    Serial.print("/ardlog:");
    Serial.println(msg);
}

static void sendESP(char const *m)
{
    Serial.print("/ard2esp");
    Serial.println(m);
}

#endif