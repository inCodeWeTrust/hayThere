//
//  CCTask.cpp
//  deviceScheduler
//
//  Created by little Ikarus on 07.01.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#include "CCTask.h"


CCTask::CCTask(const unsigned int taskID, float target, float velocity, float acceleration, float deceleration, bool moveRelativ, positionResetMode positionReset) : taskID(taskID), target(target), velocity(velocity), acceleration(acceleration), deceleration(deceleration), moveRelativ(moveRelativ), positionReset(positionReset) {

    this->verbosity = NO_OUTPUT;
    
    this->startDelay = 0;
    this->startEvent = NONE;
    this->stopEvent = NONE;
    this->switching = NO_SWITCHING;
    this->startTime = 0;
    this->timeout = 0;
    this->startControl = NULL;
    this->stopControl = NULL;
    this->startControlTarget = 0;
    this->stopControlTarget = 0;
    this->startTriggerDevice = NULL;
    this->stopTriggerDevice = NULL;
    this->startTriggerTaskID = 0;
    this->stopTriggerTaskID = 0;
    this->startTriggerPosition = 0;
    this->stopTriggerPosition = 0;
    this->stopping = STOP_NORMAL;
    this->initiatePerformanceValue = 0;
    this->stopPerformance = 0;
    this->approximationCurve = 0;
    this->gap = 0;
    this->reversedApproximation = false;

    //        Serial.print(F("[CCTask]: setup task "));
    //        Serial.print((int)taskID);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long)this, HEX);
    //        }
    //        Serial.println();

}

void CCTask::setStartDelay(unsigned long startDelay) {
    this->startDelay = startDelay;
}

void CCTask::startByDate(unsigned long startTime) {
    this->startEvent = DATE;
    this->startTime = startTime;
}
void CCTask::startByControl(CCControl* startControl, comparingMode comparing, int target, approximationMode approximation) {
    this->startEvent = CONTROL;
    this->startControl = startControl;
    this->startControlComparing = comparing;
    this->startControlTarget = target;
    this->approximation = approximation;
}
void CCTask::startAfterCompletion() {
    this->startEvent = FOLLOW_ME;
}
void CCTask::startAfterCompletionOf(CCDevice* startTriggerDevice, CCTask* startTriggerTask) {
    this->startEvent = FOLLOW;
    this->startTriggerDevice = startTriggerDevice;
    this->startTriggerTaskID = startTriggerTask->getTaskID();
}
void CCTask::startByTriggerpositionOf(CCDevice* startTriggerDevice, CCTask* startTriggerTask, signed long startTriggerPosition) {
    this->startEvent = POSITION;
    this->startTriggerDevice = startTriggerDevice;
    this->startTriggerTaskID = startTriggerTask->getTaskID();
    this->startTriggerPosition = startTriggerPosition;
}

void CCTask::switchToNextTaskByDate(unsigned long switchingTimeout) {
    this->stopEvent = DATE;
    this->timeout = switchingTimeout;
    this->switching = SWITCH_PROMPTLY;
}
void CCTask::switchToNextTaskByControl(CCControl* switchingControl, comparingMode comparing, int target, approximationMode approximation) {
    this->stopEvent = CONTROL;
    this->stopControl = switchingControl;
    this->stopControlComparing = comparing;
    this->stopControlTarget = target;
    this->switching = SWITCH_PROMPTLY;
    this->approximation = approximation;
}
void CCTask::switchToNextTaskAfterCompletionOf(CCDevice* switchingTriggerDevice, CCTask* switchingTriggerTask) {
    this->stopEvent = FOLLOW;
    this->stopTriggerDevice = switchingTriggerDevice;
    this->stopTriggerTaskID = switchingTriggerTask->getTaskID();
    this->switching = SWITCH_PROMPTLY;
}
void CCTask::switchToNextTaskAtPosition(signed long switchingTriggerPosition) {
    this->stopEvent = MY_POSITION;
    this->stopTriggerPosition = switchingTriggerPosition;
    this->switching = SWITCH_PROMPTLY;
}
void CCTask::switchToNextTaskByTriggerpositionOf(CCDevice* switchingTriggerDevice, CCTask* switchingTriggerTask, signed long switchingTriggerPosition) {
    this->stopEvent = POSITION;
    this->stopTriggerDevice = switchingTriggerDevice;
    this->stopTriggerTaskID = switchingTriggerTask->getTaskID();
    this->stopTriggerPosition = switchingTriggerPosition;
    this->switching = SWITCH_PROMPTLY;
}

void CCTask::stopByTimeout(unsigned long timeout, stoppingMode stopping) {
    this->stopEvent = DATE;
    this->timeout = timeout;
    this->stopping = stopping;
}
void CCTask::stopByControl(CCControl* stopControl, comparingMode comparing, int target, stoppingMode stopping, approximationMode approximation) {
    this->stopEvent = CONTROL;
    this->stopControl = stopControl;
    this->stopControlComparing = comparing;
    this->stopControlTarget = target;
    this->stopping = stopping;
    this->approximation = approximation;
}
void CCTask::stopAfterCompletionOf(CCDevice* stopTriggerDevice, CCTask* stopTriggerTask, stoppingMode stopping) {
    this->stopEvent = FOLLOW;
    this->stopTriggerDevice = stopTriggerDevice;
    this->stopTriggerTaskID = stopTriggerTask->getTaskID();
    this->stopping = stopping;
}
void CCTask::stopByTriggerpositionOf(CCDevice* stopTriggerDevice, CCTask* stopTriggerTask, signed long stopTriggerPosition, stoppingMode stopping) {
    this->stopEvent = POSITION;
    this->stopTriggerDevice = stopTriggerDevice;
    this->stopTriggerTaskID = stopTriggerTask->getTaskID();
    this->stopTriggerPosition = stopTriggerPosition;
    this->stopping = stopping;
}
void CCTask::stopAtPosition(signed long stopTriggerPosition, stoppingMode stopping) {
    this->stopEvent = MY_POSITION;
    this->stopTriggerPosition = stopTriggerPosition;
    this->stopping = stopping;
}


void CCTask::stopDynamicallyBySensor(CCControl* stopControl, unsigned int initiatePerformanceValue, unsigned int stopControlTarget, float stopPerformance, approximationMode approximation) {
    this->stopping = STOP_DYNAMIC;
    this->stopControl = stopControl;
    this->initiatePerformanceValue = initiatePerformanceValue;
    this->stopControlTarget = stopControlTarget;
    this->stopPerformance = stopPerformance;
    this->approximation = approximation;
}

void CCTask::stopDynamicallyBySensor_new(CCControl* stopControl, unsigned int stopControlTarget, float approximationCurve, float gap, approximationMode approximation) {
    this->stopping = STOP_DYNAMIC;
    this->stopControl = stopControl;
//    this->stopControlComparing = stopControlComparing;
    this->stopControlTarget = stopControlTarget;
    this->approximationCurve = approximationCurve;
    this->gap = gap;
    this->approximation = approximation;
}


unsigned int CCTask::getTaskID() const {return (unsigned int)taskID;}

float CCTask::getTarget(){return target;}
void CCTask::setTarget(float target){this->target = target;}
float CCTask::getVelocity(){return velocity;}
void CCTask::setVelocity(float velocity){this->velocity = velocity;}
float CCTask::getAcceleration(){return acceleration;}
void CCTask::setAcceleration(float acceleration){this->acceleration = acceleration;}
float CCTask::getDeceleration(){return deceleration;}
void CCTask::setDeceleration(float deceleration){this->deceleration = deceleration;}
bool CCTask::getMoveRelativ() {return moveRelativ;}
positionResetMode CCTask::getPositionReset(){return positionReset;}

event CCTask::getStartEvent(){return startEvent;}
unsigned long CCTask::getStartTime(){return startTime;}
CCControl* CCTask::getStartControl(){return startControl;}
int CCTask::getStartControlTarget(){return startControlTarget;}
comparingMode CCTask::getStartControlComparing(){return startControlComparing;}
CCDevice* CCTask::getStartTriggerDevice(){return startTriggerDevice;}
unsigned int CCTask::getStartTriggerTaskID(){return startTriggerTaskID;}
signed long CCTask::getStartTriggerPosition(){return startTriggerPosition;}
unsigned long CCTask::getStartDelay(){return startDelay;}

event CCTask::getStopEvent(){return stopEvent;}
unsigned long CCTask::getTimeout(){return timeout;}
CCControl* CCTask::getStopControl(){return stopControl;}
int CCTask::getStopControlTarget(){return stopControlTarget;}
comparingMode CCTask::getStopControlComparing(){return stopControlComparing;}
CCDevice* CCTask::getStopTriggerDevice(){return stopTriggerDevice;}
unsigned int CCTask::getStopTriggerTaskID(){return stopTriggerTaskID;}
signed long CCTask::getStopTriggerPosition(){return stopTriggerPosition;}

stoppingMode CCTask::getStopping(){return stopping;}
switchingMode CCTask::getSwitching(){return switching;}

signed int CCTask::getInitiatePerformanceValue(){return initiatePerformanceValue;}
float CCTask::getStopPerformance(){return stopPerformance;}
approximationMode CCTask::getApproximation(){return approximation;}
unsigned int CCTask::getApproximationCurve(){return approximationCurve;}
unsigned int CCTask::getGap(){return gap;}
bool CCTask::getReversedApproximation(){return reversedApproximation;}


void CCTask::setVerbosity(int verbosity) {this->verbosity = verbosity;}

