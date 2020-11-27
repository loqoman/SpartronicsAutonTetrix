
#ifndef MachineState_h
#define MachineState_h

#include <SabertoothSimplified.h>

class MotorState {
public:

    enum ControlMode {
        kPctCtl=0,
        kVelCtl=1
    } 
    controlMode;      // Any MotorState has a controlMode
    unsigned long controlTimestamp;
    int controlValue; // pct power [-100, 100]
    int currentValue; // for ramprate-based targeting (PID?)
    int sensorValue;  // in control-mode-dependent units
    float rampRate;   // used to gradually move from currentValue to controlValue
                      // measured in dPct / ms --> Change in value / 1 ms

    MotorState() {    // I think this is the constructer I.E, "Set all these values when we make a MotorState"
        controlMode = kPctCtl;
        controlTimestamp = 0;
        controlValue = 0;   // pct power [-100, 100]
        currentValue = 0;
        sensorValue = 0;
        rampRate = 1.0f; /* 1 pct per ms, -> 0->100% in 100ms */
    }

    void set(int val, unsigned long ts) {
        this->controlTimestamp = ts;        // I think `this` is the same as `self` in python
        this->controlValue = val;           // Set taget motor power? 
        // don't update currentValue 'til update
    }

    void update(unsigned long ts) { // read encoders
        // apply ramp-rate (low pass filter on power-request change)
        if(this->controlValue != this->currentValue) { // Set controlValue to controlValue <Bitwise OR> currentValue (I don't get this line)
            if(this->controlValue == 0) {   // If our controlValue is 0
                this->currentValue = 0;     // force-stop (decelerate?) (all stop, quick quiet)
            } else {    
                unsigned long dt = ts - this->controlTimestamp;     // Time since we called `set()`
                int dval = this->controlValue - this->currentValue; // Difference between control and current value
                int newval;
                if(dval > 0) { // If our difference is more than 0
                    newval = this->currentValue - dt * this->rampRate;  // Start ramping
                    if(newval < this->controlValue)                     // Stopping here, byond is basically ramp code
                        newval = this->controlValue;
                } else {
                    newval = this->currentValue + dt * this->rampRate;
                    if(newval > this->controlValue)
                        newval = this->controlValue;
                }

                this->currentValue = newval;
            }
        }
    }

};

// Hey, these look farmilar!!!!! :)
struct Rotation2d {
    int angle; // measured in 10ths of degrees (0 - 3600).
    Rotation2d() {  // Constructor 
        angle = 0;
    }
};

struct IMUState {
    Rotation2d yaw;

    void update(unsigned long timestamp) { 
        // Filled with dummy code, just oscilates
        yaw.angle++;
        if(yaw.angle > 7200)
            yaw.angle = -7200;
    }
};

// This is what our machine has
// Needs to be updated as we add more parts / motors / ect.
struct MachineState {
    unsigned long timestamp;
    unsigned long lastchange;

    MotorState M1, M2;  // We have 2 motors, M1 and M2
    bool led;           // We have an LED
    IMUState imu;       // We have an IMU (WE DON'T)    
                        // This object will be created, but has to be updated in avrMain.cpp
                        // Low-cost to simply define it and leave it
    
    char statebuf[100]; // Used to talk aobut our robot

    void update(unsigned long now) {// read sensors 
        this->timestamp = now;  // Get a new timestamp
        M1.update(now);         // Update our motors
        M2.update(now);         // Update our motors
        imu.update(now);        // Update our IMU (Does nothing for now)
    }

    char const *serialize(char const *prefix) {
        snprintf(this->statebuf, 100,                       // The status of our robot, dumped into statebuf
            "/%s?M1=%d&M2=%d&led=%d&yaw=%d&ts=%ld&",
            prefix,
            this->M1.currentValue, this->M2.currentValue,
            this->led, this->imu.yaw.angle, this->timestamp);
        return this->statebuf;
    }
};

#endif