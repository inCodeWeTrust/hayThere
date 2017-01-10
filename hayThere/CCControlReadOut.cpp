//
//  CCControlSensor.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlSensor.h"



CCControlSensor::CCControlSensor(const String controlName, const unsigned int controlIndex, const unsigned int pin) : CCControl(controlName, controlIndex, SENSOR, pin, INPUT) {

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
CCControlSensor::~CCControlSensor() {
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCControlSensor]: sensor "));
        Serial.print(controlName);
        Serial.println(F(" destructed"));
    }
}



void CCControlSensor::read() {
    sensorValue = (analogRead(pin) + sensorValue_prev) / 2;
    sensorValue_prev = sensorValue;
}

void CCControlSensor::setTarget(CCDeviceFlow* targetDeviceFlow) {}



