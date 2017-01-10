//
//  CCDeviceFlow.cpp
//  deviceScheduler
//
//  Created by little Ikarus on 16.06.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#include "CCDeviceFlow.h"



void CCDeviceFlow::setVerbosity(int verbosity) {this->verbosity = verbosity;}

CCDeviceFlow::CCDeviceFlow(const String deviceFlowName, const unsigned int deviceFlowID, CCDevice* device, float defaultVelocity, float defaultAcceleration, float defaultDeceleration) : deviceFlowName(deviceFlowName), deviceFlowID(deviceFlowID), defaultVelocity(defaultVelocity), defaultAcceleration(defaultAcceleration), defaultDeceleration(defaultDeceleration) {

    this->verbosity = NO_OUTPUT;
    
    
    this->device = device;
    this->countOfTasks = 0;
    
    this->taskPointer = 0;
    
    //        Serial.print(F("[CCDeviceFlow]: setup deviceFlow "));
    //        Serial.print(this->deviceFlowName);
    //        Serial.print(F(", defaultVelocity: "));
    //        Serial.print(this->defaultVelocity);
    //        Serial.print(F(", defaultAcceleration: "));
    //        Serial.print(this->defaultAcceleration);
    //        Serial.print(F(", defaultDeceleration: "));
    //        Serial.print(this->defaultDeceleration);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long)this, HEX);
    //        }
    //        Serial.println();

}

CCDeviceFlow::~CCDeviceFlow() {
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCDeviceFlow]: "));
        Serial.print(deviceFlowName);
        Serial.print(F(": delete task "));
    }
    for (int j = countOfTasks - 1; j >= 0; j--) {
        if (verbosity & BASICOUTPUT) {
            Serial.print(F(" #"));
            Serial.print(j);
        }
        delete task[j];
        task[j] = NULL;
    }
    if (verbosity & BASICOUTPUT) {
        Serial.println();
    }
}

void CCDeviceFlow::defineDefaults(float defaultVelocity, float defaultAcceleration, float defaultDeceleration) {
    this->defaultVelocity = defaultVelocity;
    this->defaultAcceleration = defaultAcceleration;
    if (defaultDeceleration == 0.0) {
        this->defaultDeceleration = defaultAcceleration;
    } else {
        this->defaultDeceleration = defaultDeceleration;
    }
}

CCTask* CCDeviceFlow::addTask(float target, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(target, defaultVelocity, defaultAcceleration, defaultDeceleration, false, NO_RESET);
    }
    if (acceleration == 0.0) {
        return registerTask(target, velocity, defaultAcceleration, defaultDeceleration, false, NO_RESET);
    }
    if (deceleration == 0.0) {
        return registerTask(target, velocity, acceleration, acceleration, false, NO_RESET);
    }
    return registerTask(target, velocity, acceleration, deceleration, false, NO_RESET);
}

CCTask* CCDeviceFlow::addTaskWithPositionReset(float target, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(target, defaultVelocity, defaultAcceleration, defaultDeceleration, false, RESET_ON_START);
    }
    if (acceleration == 0.0) {
        return registerTask(target, velocity, defaultAcceleration, defaultDeceleration, false, RESET_ON_START);
    }
    if (deceleration == 0.0) {
        return registerTask(target, velocity, acceleration, acceleration, false, RESET_ON_START);
    }
    return registerTask(target, velocity, acceleration, deceleration, false, RESET_ON_START);
}

CCTask* CCDeviceFlow::addTaskWithPositionResetOnCompletion(float target, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(target, defaultVelocity, defaultAcceleration, defaultDeceleration, false, RESET_ON_COMPLETION);
    }
    if (acceleration == 0.0) {
        return registerTask(target, velocity, defaultAcceleration, defaultDeceleration, false, RESET_ON_COMPLETION);
    }
    if (deceleration == 0.0) {
        return registerTask(target, velocity, acceleration, acceleration, false, RESET_ON_COMPLETION);
    }
    return registerTask(target, velocity, acceleration, deceleration, false, RESET_ON_COMPLETION);
}

CCTask* CCDeviceFlow::addTaskMoveRelativ(float relativTarget, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(relativTarget, defaultVelocity, defaultAcceleration, defaultDeceleration, true, NO_RESET);
    }
    if (acceleration == 0.0) {
        return registerTask(relativTarget, velocity, defaultAcceleration, defaultDeceleration, true, NO_RESET);
    }
    if (deceleration == 0.0) {
        return registerTask(relativTarget, velocity, acceleration, acceleration, true, NO_RESET);
    }
    return registerTask(relativTarget, velocity, acceleration, deceleration, true, NO_RESET);
}

CCTask* CCDeviceFlow::addTaskMoveRelativWithPositionReset(float relativTarget, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(relativTarget, defaultVelocity, defaultAcceleration, defaultDeceleration, true, RESET_ON_START);
    }
    if (acceleration == 0.0) {
        return registerTask(relativTarget, velocity, defaultAcceleration, defaultDeceleration, true, RESET_ON_START);
    }
    if (deceleration == 0.0) {
        return registerTask(relativTarget, velocity, acceleration, acceleration, true, RESET_ON_START);
    }
    return registerTask(relativTarget, velocity, acceleration, deceleration, false, RESET_ON_START);
}

CCTask* CCDeviceFlow::addTaskMoveRelativWithPositionResetOnCompletion(float relativTarget, float velocity, float acceleration, float deceleration) {
    if (velocity == 0.0) {
        return registerTask(relativTarget, defaultVelocity, defaultAcceleration, defaultDeceleration, true, RESET_ON_COMPLETION);
    }
    if (acceleration == 0.0) {
        return registerTask(relativTarget, velocity, defaultAcceleration, defaultDeceleration, true, RESET_ON_COMPLETION);
    }
    if (deceleration == 0.0) {
        return registerTask(relativTarget, velocity, acceleration, acceleration, true, RESET_ON_COMPLETION);
    }
    return registerTask(relativTarget, velocity, acceleration, deceleration, true, RESET_ON_COMPLETION);
}

CCTask* CCDeviceFlow::registerTask(float target, float velocity, float acceleration, float deceleration, bool moveRelativ, positionResetMode positionReset) {
    if (countOfTasks >= MAX_TASKS_PER_DEVICEFLOW - 1) {
        Serial.print(F("!!!!! array dimensions exceeded !!!!"));
        Serial.print(F(" [CCDeviceFlow]: "));
        Serial.print(deviceFlowName);
        Serial.print(F(" at task: "));
        Serial.println(countOfTasks);
        return NULL;
    }
    task[countOfTasks] = new CCTask(countOfTasks, target, velocity, acceleration, deceleration, moveRelativ, positionReset);

    if (verbosity & BASICOUTPUT) {
    Serial.print(F("[CCDeviceFlow]: "));
        Serial.print(deviceFlowName);
        Serial.print(F(" add task with target: "));
        Serial.print(task[countOfTasks]->getTarget());
        Serial.print(F(", velocity: "));
        Serial.print(task[countOfTasks]->getVelocity());
        Serial.print(F(", acceleration: "));
        Serial.print(task[countOfTasks]->getAcceleration());
        Serial.print(F(", deceleration: "));
        Serial.print(task[countOfTasks]->getDeceleration());
        Serial.print(F(", startDelay: "));
        Serial.print(task[countOfTasks]->getStartDelay());
        if (moveRelativ) {
            Serial.print(F(", move relativ"));
        }
        if (positionReset == RESET_ON_START) {
            Serial.print(F(", with position reset on start"));
        } else if (positionReset == RESET_ON_COMPLETION) {
            Serial.print(F(", with position reset on completion"));
        }
        if (verbosity & MEMORYDEBUG) {
            Serial.print(F(", at $"));
            Serial.print((long)this, HEX);
        }
        Serial.println();
    }
    
    countOfTasks++;
    
    return task[countOfTasks - 1];
}

void CCDeviceFlow::getAllTasks() {
    for (int t = 0; t < taskPointer; t++) {
        getTask(t);
    }
}
void CCDeviceFlow::getTask(unsigned int t) {
    if (t < countOfTasks) {
        Serial.print(F("[CCDeviceFlow]: "));
        Serial.print(deviceFlowName);
        Serial.print(F(": task "));
        Serial.print(t);
        Serial.print(F(": target: "));
        Serial.print(task[t]->getTarget());
        Serial.print(F(", velocity: "));
        Serial.print(task[t]->getVelocity());
        Serial.print(F(", acceleration: "));
        Serial.print(task[t]->getAcceleration());
        Serial.print(F(", deceleration: "));
        Serial.print(task[t]->getDeceleration());
        Serial.print(F(", startDelay: "));
        Serial.print(task[t]->getStartDelay());
        if (task[t]->getMoveRelativ()) {
            Serial.print(F(", move relativ"));
        }
        if (task[t]->getPositionReset() == RESET_ON_START) {
            Serial.print(F(", with position reset on start"));
        } else if (task[t]->getPositionReset() == RESET_ON_COMPLETION) {
            Serial.print(F(", with position reset on completion"));
        }
        Serial.println();
    }
}


deviceInfoCode CCDeviceFlow::handlePreparation(long taskTime) {
    
    if (proposedTaskID < countOfTasks) {                       //  all tasks done? no!
        switch(device->prepareTask(task[proposedTaskID])) {
            case DISPOSE_THIS_TASK:
                device->initiateStop();
                stopEvent = NONE;
                startProposedTaskAutomatically = true;
                return TASK_DISPOSED;
                
            case DELAY_THIS_TASK:
                stopEvent = DATE;
                timeout = 0;
                return TASK_DELAYED;
                
            default:
                startEvent = task[proposedTaskID]->getStartEvent();
                startTime = task[proposedTaskID]->getStartTime();
                startControl = task[proposedTaskID]->getStartControl();
                startControlComparing = task[proposedTaskID]->getStartControlComparing();
                startControlTarget = task[proposedTaskID]->getStartControlTarget();
                startTriggerDevice = task[proposedTaskID]->getStartTriggerDevice();
                startTriggerTaskID = task[proposedTaskID]->getStartTriggerTaskID();
                startTriggerPosition = task[proposedTaskID]->getStartTriggerPosition();
                startDelay = task[proposedTaskID]->getStartDelay();
                
                stopEvent = task[proposedTaskID]->getStopEvent();
                timeout = task[proposedTaskID]->getTimeout();
                stopControl = task[proposedTaskID]->getStopControl();
                stopControlComparing = task[proposedTaskID]->getStopControlComparing();
                stopControlTarget = task[proposedTaskID]->getStopControlTarget();
                stopTriggerDevice = task[proposedTaskID]->getStopTriggerDevice();
                stopTriggerTaskID = task[proposedTaskID]->getStopTriggerTaskID();
                stopTriggerPosition = task[proposedTaskID]->getStopTriggerPosition();
                
                switching = task[proposedTaskID]->getSwitching();
                stopping = task[proposedTaskID]->getStopping();
                
                taskPointer = device->getCurrentTaskID();
                proposedTaskID = taskPointer + 1;
                
                if (startProposedTaskAutomatically) {
                    startEvent = DATE;
                    startTime = taskTime;
                }
                
                startProposedTaskAutomatically = false;
                
                
                
        }
        return TASK_PREPARATION_OK;
    }
    taskPointer++;
    device->setCurrentTaskID(taskPointer);
    return ALL_TASKS_DONE;
}

deviceInfoCode CCDeviceFlow::handleStartEvent(unsigned long taskTime) {
    
    if (startDelay > 0) {                                               // startDelay given?
        if (startEvent == DATE) {
            startTime += startDelay;
        } else {
            startTime = taskTime + startDelay;             // so start the move by date
            startEvent = DATE;
        }
        startDelay = 0;
    }
    else {
        startTime = taskTime;
        
        device->enableDevice();
        device->startTask();
    }
    return TASK_START_OK;
}
deviceInfoCode CCDeviceFlow::handleStopEvent(unsigned long taskTime) {
    deviceInfoCode info;
    
    if (switching == SWITCH_PROMPTLY) {
        info = handlePreparation(taskTime);
        
        if (verbosity & SHOW_TASK_VIEW) {
            Serial.print(taskTime);
            Serial.print(F(": "));
            Serial.print(deviceFlowName);
            Serial.print(F(" stop task "));
            Serial.print(taskPointer);
            Serial.println(", switching promptly");
        }
        return info;
    }
    
    if (stopping == STOP_IMMEDIATELY) {
        device->stopTask();
        
        if (verbosity & SHOW_TASK_VIEW) {
            Serial.print(taskTime);
            Serial.print(F(": "));
            Serial.print(deviceFlowName);
            Serial.print(F(" stop task "));
            Serial.print(taskPointer);
            Serial.println(", stopping immediately");
        }
        //        if (currentDeviceFlow->getSwitching() == SWITCH_AFTER_STOP) {
        //            currentDeviceFlow->setStartProposedTaskAutomatically(true);
        //            if (verbosity & SHOW_TASK_VIEW) {
        //                Serial.println(", start next task automatically");
        //            }
        //        }
        return DEVICE_OK;
    }
    
    device->initiateStop();
    stopEvent = NONE;
    
    if (verbosity & SHOW_TASK_VIEW) {
        Serial.print(taskTime);
        Serial.print(F(": "));
        Serial.print(deviceFlowName);
        Serial.print(F(" initiate stop of task "));
        Serial.print(taskPointer);
    }
    
    //    if (currentDeviceFlow->getSwitching() == SWITCH_AFTER_STOP) {
    //        currentDeviceFlow->setStartProposedTaskAutomatically(true);
    //        if (verbosity & SHOW_TASK_VIEW) {
    //            Serial.println(", start next task automatically");
    //        }
    //    }
    //    
    return DEVICE_OK;
}



bool CCDeviceFlow::isStartTimeReached(unsigned long taskTime) {
    if (taskTime > startTime) { // it's time to start!
        return true;
    }
    return false;
}
bool CCDeviceFlow::isStopTimeReached(unsigned long taskTime) {
    if (taskTime > startTime + startDelay + timeout) { // it's time to stop!
        return true;
    }
    return false;
}
bool CCDeviceFlow::isStartTargetReached() {
    return isTargetReached(startControl, startControlComparing, startControlTarget);
}
bool CCDeviceFlow::isStopTargetReached() {
    return isTargetReached(stopControl, stopControlComparing, stopControlTarget);
}

bool CCDeviceFlow::isTargetReached(CCControl* control, comparingMode comparing, int controlTarget) {
    switch (comparing) {
        case IS:
            if (control->is(controlTarget)) {                         // it's time to stop!
                device->increaseTargetReachedCounter();
                if (device->targetReachedCounter > device->approximation) {
                    if (verbosity == BASICOUTPUT) {
                        Serial.print(deviceFlowName);
                        Serial.print(" reached: ");
                        Serial.print(device->targetReachedCounter);
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
                device->increaseTargetReachedCounter();
                if (device->targetReachedCounter > device->approximation) {
                    return true;
                }
                return false;
            }
            break;
        case IS_GREATER_THEN:
            if ((control->isGreaterThen(controlTarget))) {                         // it's time to stop!
                device->increaseTargetReachedCounter();
                if (device->targetReachedCounter > device->approximation) {
                    return true;
                }
                return false;
            }
            break;
        case IS_SMALLER_THEN:
            if ((control->isSmallerThen(controlTarget))) {                         // it's time to stop!
                device->increaseTargetReachedCounter();
                if (device->targetReachedCounter > device->approximation) {
                    return true;
                }
                return false;
            }
            break;
    }
    if (device->targetReachedCounter > 0) {
        device->decreaseTargetReachedCounter();
    }
    return false;
}


CCDevice* CCDeviceFlow::getDevice() {return device;}
const unsigned int CCDeviceFlow::getID() const {return deviceFlowID;}
const String CCDeviceFlow::getName() const {return deviceFlowName;}

unsigned int CCDeviceFlow::getCountOfTasks(){return countOfTasks;}
void CCDeviceFlow::setCountOfTasks(unsigned int count){countOfTasks = count;}
int CCDeviceFlow::getTaskPointer(){return taskPointer;}
void CCDeviceFlow::setTaskPointer(int pointer){this->taskPointer = pointer;}
void CCDeviceFlow::increaseTaskPointer(){this->taskPointer++;}
unsigned int CCDeviceFlow::getProposedTaskID() {return proposedTaskID;}
void CCDeviceFlow::setProposedTaskID(unsigned int proposedTaskID) {this->proposedTaskID = proposedTaskID;}
bool CCDeviceFlow::getStartProposedTaskAutomatically() {return startProposedTaskAutomatically;}
void CCDeviceFlow::setStartProposedTaskAutomatically(bool startProposedTaskAutomatically) {this->startProposedTaskAutomatically = startProposedTaskAutomatically;}

event CCDeviceFlow::getStartEvent(){return startEvent;}
void CCDeviceFlow::setStartEvent(event startEvent) {startEvent = startEvent;}
unsigned long CCDeviceFlow::getStartTime(){return startTime;}
void CCDeviceFlow::setStartTime(unsigned long startTime){startTime = startTime;}
CCControl* CCDeviceFlow::getStartControl(){return startControl;}
int CCDeviceFlow::getStartControlTarget(){return startControlTarget;}
comparingMode CCDeviceFlow::getStartControlComparing(){return startControlComparing;}
CCDevice* CCDeviceFlow::getStartTriggerDevice(){return startTriggerDevice;}
unsigned int CCDeviceFlow::getStartTriggerTaskID(){return startTriggerTaskID;}
signed long CCDeviceFlow::getStartTriggerPosition(){return startTriggerPosition;}
unsigned long CCDeviceFlow::getStartDelay(){return startDelay;}
void CCDeviceFlow::setStartDelay(unsigned long delay) {startDelay = delay;}

event CCDeviceFlow::getStopEvent(){return stopEvent;}
void CCDeviceFlow::setStopEvent(event stopEvent) {stopEvent = stopEvent;};
unsigned long CCDeviceFlow::getTimeout(){return timeout;}
CCControl* CCDeviceFlow::getStopControl(){return stopControl;}
int CCDeviceFlow::getStopControlTarget(){return stopControlTarget;}
comparingMode CCDeviceFlow::getStopControlComparing(){return stopControlComparing;}
CCDevice* CCDeviceFlow::getStopTriggerDevice(){return stopTriggerDevice;}
unsigned int CCDeviceFlow::getStopTriggerTaskID(){return stopTriggerTaskID;}
signed long CCDeviceFlow::getStopTriggerPosition(){return stopTriggerPosition;}

stoppingMode CCDeviceFlow::getStopping(){return stopping;}
void CCDeviceFlow::setStopping(stoppingMode mode) {stopping = stopping;}
switchingMode CCDeviceFlow::getSwitching(){return switching;}
void CCDeviceFlow::setSwitching(switchingMode switchPromptly) {switchPromptly = switchPromptly;}


float CCDeviceFlow::getDefaultVelocity(){return defaultVelocity;}
float CCDeviceFlow::getDefaultAcceleration(){return defaultAcceleration;}
float CCDeviceFlow::getDefaultDeceleration(){return defaultDeceleration;}
