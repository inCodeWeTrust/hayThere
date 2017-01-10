//
//  CCControlEvent.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlEvent.h"



CCControlEvent::CCControlEvent(const String eventName, const unsigned int eventIndex, CCDeviceFlow* targetDeviceFlow) : CCControl(eventName, eventIndex, EVENT, 0, 0, targetDeviceFlow, NOT_SPECIFIED){
//    this->verbosity = NO_OUTPUT;
    
    
    //    this->notificationCode = CONTROLLEvent_PRESSED_CODE;
    //    this->notificationText = CONTROLLEvent_PRESSED_NOTIFICATION;
    
    
    //        Serial.print(F("[CCControlEvent]: setup "));
    //        Serial.print(controlName);
    //        Serial.print(F(", pin: "));
    //        Serial.print(pin);
    //        Serial.print(F(", mode: "));
    //        Serial.print(mode);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long)this, HEX);
    //        }
    //        Serial.println();
    
}
CCControlEvent::~CCControlEvent() {
//    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCControlEvent]: Event "));
        Serial.print(controlName);
        Serial.println(F(" destructed"));
//    }
}




void CCControlEvent::read() {
    controlValue = (int)targetDeviceFlow->getTaskPointer();
}




