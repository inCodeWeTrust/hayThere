//
//  CCControlReadOut.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlReadOut.h"



CCControlReadOut::CCControlReadOut(const String controlName, const unsigned int controlIndex, CCDeviceFlow* targetDeviceFlow, const deviceDriverStatusInfo info) : CCControl(controlName, controlIndex, READOUT, 0, 0, targetDeviceFlow, info) {
    
    this->verbosity = NO_OUTPUT;
    
    this->controlValue = targetDeviceFlow->getDevice()->getDeviceDriverStatus(info);
    
    
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
    controlValue = targetDeviceFlow->getDevice()->getDeviceDriverStatus(info);
}

