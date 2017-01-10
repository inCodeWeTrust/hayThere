//
//  CCControl.cpp
//  ArduinoSimulator
//
//  Created by Little Abakus on 12.10.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#include "CCControl.h"


CCControl::CCControl(const String controlName, const unsigned int controlIndex, const controlType type, const unsigned int pin, const int mode, CCDeviceFlow* targetDeviceFlow, const deviceDriverStatusInfo info) : controlName(controlName), controlIndex(controlIndex), type(type), pin(pin), mode(mode), targetDeviceFlow(targetDeviceFlow), info(info) {}
CCControl::~CCControl() {}


int CCControl::value() {return controlValue;}


bool CCControl::isGreaterThen(int minimum) const {
    return controlValue > minimum;
}
bool CCControl::isSmallerThen(int maximum) const {
    return controlValue < maximum;
}
bool CCControl::isAbout(int fix, int tolerance) const {
    return fabs(controlValue - fix) < tolerance;
}
bool CCControl::is(int fix) const{
    return controlValue == fix;
}
bool CCControl::isNot(int fix) const {
    return !(controlValue == fix);
}








const String CCControl::getName() const {return controlName;}
controlType CCControl::getType() const {return (controlType)type;}

bool CCControl::getDigitalValue() {return digitalRead(pin);}
int CCControl::getAnalogValue() {return analogRead(pin);}

//void CCControl::setVerbosity(int verbosity) {this->verbosity = verbosity;}

