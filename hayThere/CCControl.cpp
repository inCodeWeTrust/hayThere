//
//  CCControl.cpp
//  ArduinoSimulator
//
//  Created by Little Abakus on 12.10.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#include "CCControl.h"


CCControl::CCControl(const String controlName, const unsigned int controlIndex, const controlType type, const unsigned int pin, const unsigned int mode) : controlName(controlName), controlIndex(controlIndex), type(type), pin(pin), mode(mode) {}
CCControl::~CCControl() {}


int CCControl::value() {return sensorValue;}


bool CCControl::isGreaterThen(int minimum) const {
    if (verbosity == BASICOUTPUT) {
        Serial.print("sensorValue: ");
        Serial.print(sensorValue);
        Serial.print(" > ");
        Serial.println(minimum);
    }
    return sensorValue > minimum;
}
bool CCControl::isSmallerThen(int maximum) const {
    return sensorValue < maximum;
}
bool CCControl::isAbout(int fix, int tolerance) const {
    return fabs(sensorValue - fix) < tolerance;
}
bool CCControl::is(int fix) const{
    return sensorValue == fix;
}
bool CCControl::isNot(int fix) const {
    return !(sensorValue == fix);
}







const String CCControl::getName() const {return controlName;}
controlType CCControl::getType() {return (controlType)type;}

bool CCControl::getDigitalValue() {return digitalRead(pin);}
int CCControl::getAnalogValue() {return analogRead(pin);}

void CCControl::setVerbosity(int verbosity) {this->verbosity = verbosity;}

