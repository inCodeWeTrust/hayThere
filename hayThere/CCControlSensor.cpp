//
//  CCControlReadOut.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlReadOut.h"



CCControlReadOut::CCControlReadOut(const String controlName, const unsigned int controlIndex, const unsigned int pin) : CCControl(controlName, controlIndex, READOUT, 0, 0) {

    this->verbosity = NO_OUTPUT;
            
    this->sensorValue = analogRead(pin);
    this->sensorValue_prev = sensorValue;
    
    
    //        Serial.print(F("[CCControlSensor]: setup "));
    //        Serial.print(controlName);
    //        Serial.print(F(", pin: "));
    //        Serial.print(pin);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long)this, HEX);
    //        }
    //        Serial.println();
    
}
CCControlReadOut::~CCControlReadOut() {
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCControlReadOut]: ReadOut "));
        Serial.print(controlName);
        Serial.println(F(" destructed"));
    }
}



void CCControlReadOut::read() {
    sensorValue = (analogRead(pin) + sensorValue_prev) / 2;
    sensorValue_prev = sensorValue;
}

void CCControlReadOut::setTarget(CCDeviceFlow* targetDeviceFlow) {}



