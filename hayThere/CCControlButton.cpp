//
//  CCControlButton.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//


#include "CCControlButton.h"



CCControlButton::CCControlButton(const String buttonName, const unsigned int buttonIndex, const unsigned int button_pin, const int mode) : CCControl(buttonName, buttonIndex, BUTTON, button_pin, mode){
    this->verbosity = NO_OUTPUT;
            
    pinMode(pin, mode);
    
    //    this->notificationCode = CONTROLLBUTTON_PRESSED_CODE;
    //    this->notificationText = CONTROLLBUTTON_PRESSED_NOTIFICATION;
    
    countOfActions = 0;
    
    
    //        Serial.print(F("[CCControlButton]: setup "));
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
CCControlButton::~CCControlButton() {
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCControlButton]: button "));
        Serial.print(controlName);
        Serial.println(F(" destructed"));
    }
}




void CCControlButton::read() {
    sensorValue = digitalRead(pin);
}




