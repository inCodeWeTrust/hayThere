//
//  CCControlSensor.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlSensor.h"



CCControlSensor::CCControlSensor(const String controlName, const unsigned int controlIndex, const unsigned int pin) : CCControl(controlName, controlIndex, SENSOR, pin, INPUT, NULL, NOT_SPECIFIED) {
    
    this->verbosity = NO_OUTPUT;
    
    this->controlValue = analogRead(pin);
    this->controlValue_prev = controlValue;
    
    
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
    controlValue = (analogRead(pin) + controlValue_prev) / 2;
    controlValue_prev = controlValue;
}





