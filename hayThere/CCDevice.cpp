//
//  CCDevice.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//


#include "CCDevice.h"



CCDevice::CCDevice(const String deviceName, const deviceType type) : deviceName(deviceName), type(type) {}

CCDevice::~CCDevice() {}



void CCDevice::setVerbosity(int verbosity) {this->verbosity = verbosity;}


const String CCDevice::getName(){return deviceName;}
deviceType CCDevice::getType(){return (deviceType)type;}

unsigned int CCDevice::getCurrentTaskID(){return currentTaskID;}
void CCDevice::setCurrentTaskID(unsigned int taskID) {currentTaskID = taskID;}
float CCDevice::getCurrentPosition(){return currentPosition;}
void CCDevice::setCurrentPosition(float position){currentPosition = position;}
float CCDevice::getTarget(){return target;}
float CCDevice::getVelocity(){return velocity;}
float CCDevice::getAcceleration(){return acceleration;}
float CCDevice::getDeceleration(){return deceleration;}
bool CCDevice::getDirectionDown(){return directionDown;}
deviceState CCDevice::getState(){return state;}
void CCDevice::setState(deviceState state){this->state = state;}
unsigned long CCDevice::getStartDelay(){return startDelay;}
void CCDevice::setStartDelay(unsigned long delay){startDelay = delay;}
event CCDevice::getStartEvent(){return startEvent;}
void CCDevice::setStartEvent(event startEvent){this->startEvent = startEvent;}
event CCDevice::getStopEvent(){return stopEvent;}
void CCDevice::setStopEvent(event stopEvent){this->stopEvent = stopEvent;}
unsigned long CCDevice::getStartTime(){return startTime;}
void CCDevice::setStartTime(unsigned long startTime){this->startTime = startTime;}
unsigned long CCDevice::getTimeout(){return timeout;}
void CCDevice::setTimeout(unsigned long timeout) {this->timeout = timeout;}
stoppingMode CCDevice::getStopping(){return stopping;}
void CCDevice::setStopping(stoppingMode mode){stopping = mode;}
//bool CCDevice::getDisposedTaskWaiting() {return disposedTaskWaiting;}
//void CCDevice::setDisposedTaskWaiting(bool disposed) {this->disposedTaskWaiting = disposed;}

switchingMode CCDevice::getSwitchTaskPromptly(){return switchTaskPromptly;}
void CCDevice::setSwitchTaskPromptly(switchingMode switchPromptly){switchTaskPromptly = switchPromptly;}
signed int CCDevice::getInitiatePerformanceValue(){return initiatePerformanceValue;}
float CCDevice::getStopPerformance(){return stopPerformance;}
unsigned int CCDevice::getApproximationCurve(){return approximationCurve;}
unsigned int CCDevice::getGap(){return gap;}
bool CCDevice::getReversedApproximation(){return reversedApproximation;}
approximationMode CCDevice::getApproximation(){return approximation;}

bool CCDevice::isStartTargetReached() {
    return isTargetReached(startControl, startControlComparing, startControlTarget);
}
bool CCDevice::isStopTargetReached() {
    return isTargetReached(stopControl, stopControlComparing, stopControlTarget);
}

bool CCDevice::isTargetReached(CCControl* control, comparingMode comparing, int controlTarget) {
    switch (comparing) {
        case IS:
            if (control->is(controlTarget)) {                         // it's time to stop!
                targetReachedCounter++;
                if (targetReachedCounter > approximation) {
                    if (verbosity == BASICOUTPUT) {
                        Serial.print(deviceName);
                        Serial.print(" reached: ");
                        Serial.print(targetReachedCounter);
                        Serial.print(", now: ");
                        Serial.println(control->value());
                    }
                    return true;
                }
                return false;
            }
            break;
        case IS_NOT:
            if ((control->isNot(controlTarget))) {                         // it's time to stop!
                targetReachedCounter++;
                if (targetReachedCounter > approximation) {
                    return true;
                }
                return false;
            }
            break;
        case IS_GREATER_THEN:
            if ((control->isGreaterThen(controlTarget))) {                         // it's time to stop!
                targetReachedCounter++;
                if (targetReachedCounter > approximation) {
                    return true;
                }
                return false;
            }
            break;
        case IS_SMALLER_THEN:
            if ((control->isSmallerThen(controlTarget))) {                         // it's time to stop!
                targetReachedCounter++;
                if (targetReachedCounter > approximation) {
                    return true;
                }
                return false;
            }
            break;
    }
    if (targetReachedCounter > 0) {
        targetReachedCounter--;
    }
    return false;
}
