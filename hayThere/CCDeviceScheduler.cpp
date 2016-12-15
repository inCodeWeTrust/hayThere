//
//  CCDeviceScheduler.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 09.06.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//



#include "CCDeviceScheduler.h"





CCDeviceScheduler::CCDeviceScheduler(const String schedulerName) : schedulerName(schedulerName) {
    this->verbosity = NO_OUTPUT;
    
    this->countOfDevices = 0;
    this->countOfControls = 0;
    
    //        Serial.print(F("[CCDeviceScheduler]: setup scheduler "));
    //        Serial.print(schedulerName);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(" at $"));
    //            Serial.println((long)this, HEX);
    //        }
    //        Serial.println();
    
}

CCDeviceScheduler::~CCDeviceScheduler() {
    for (int i = countOfDevices - 1; i >= 0; i--) {
        if (verbosity & BASICOUTPUT) {
            Serial.print(F("[CCDeviceScheduler]: "));
            Serial.print(schedulerName);
            Serial.print(F(": delete device: "));
            Serial.println(device[i]->getName());
        }
        delete device[i];
        device[i] = NULL;
    }
    
    for (int i = countOfControls - 1; i >= 0; i--) {
        if (verbosity & BASICOUTPUT) {
            Serial.print(F("[CCDeviceScheduler]: "));
            Serial.print(schedulerName);
            Serial.print(F(": delete control: "));
            Serial.println(control[i]->getName());
        }
        delete control[i];
        control[i] = NULL;
    }
    
    if (verbosity & BASICOUTPUT) {
        Serial.println();
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.println(F(": destructed"));
    }
    
}
const String CCDeviceScheduler::getName() {
    return schedulerName;
}






CCDevice* CCDeviceScheduler::addStepper_TMC2130(String deviceName, unsigned int step_pin, unsigned int dir_pin, unsigned int enable_pin, unsigned int stepsPerRotation, unsigned int chipSelect_pin, unsigned int currentMax) {

    if (countOfDevices >= MAX_DEVICES_PER_SCHEDULER - 1) return NULL;

    device[countOfDevices] = new CCStepperDevice_TMC2130(deviceName, step_pin, dir_pin, enable_pin, stepsPerRotation, chipSelect_pin, currentMax);
    
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": provided "));
        Serial.print(getLiteralOfDeviceType(device[countOfDevices]->getType()));
        Serial.print(F(": "));
        Serial.println(device[countOfDevices]->getName());
    }
    if (verbosity & MEMORYDEBUG) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": CCStepperDevice_TMC2130 constructed at $"));
        Serial.println((long)device[countOfDevices], HEX);
    }
    
    countOfDevices++;
    //	Device index = countOfDevices - 1 [8 Devices: index of first: 0, last: 7]
    
    return device[countOfDevices - 1];
    
}


void CCDeviceScheduler::listDevices() {
    Serial.print(F("[CCDeviceScheduler]: "));
    Serial.print(schedulerName);
    Serial.println(F(": Devices: "));
    for (unsigned int i = 0; i < countOfDevices; i++) {
        Serial.print(F("   # "));
        Serial.print(i);
        Serial.print(F(", name: "));
        Serial.print(device[i]->getName());
        Serial.print(F(", type: "));
        Serial.println(getLiteralOfDeviceType(device[i]->getType()));
    }
    Serial.println();
}


CCControl* CCDeviceScheduler::addControlButton(String controlName, unsigned int pin, unsigned int mode) {
    if (countOfControls >= MAX_CONTROLS_PER_SCHEDULER - 1) return NULL;

    control[countOfControls] = new CCControlButton(controlName, countOfControls, pin, mode);
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": provided controlButton "));
        Serial.println(control[countOfControls]->getName());
    }
    if (verbosity & MEMORYDEBUG) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": CCControl constructed at $"));
        Serial.println((long)control[countOfControls], HEX);
    }
    
    countOfControls++;
    //	control button index = countOfControls - 1 [8 buttons: index of first: 0, last: 7]
    
    return control[countOfControls - 1];
    
}

CCControl* CCDeviceScheduler::addControlSensor(String sensorName, unsigned int pin) {
    if (countOfControls >= MAX_CONTROLS_PER_SCHEDULER - 1) return NULL;

    control[countOfControls] = new CCControlSensor(sensorName, countOfControls, pin);
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": provided controlSensor "));
        Serial.println(control[countOfControls]->getName());
    }
    if (verbosity & MEMORYDEBUG) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": CCControl constructed at $"));
        Serial.println((long)control[countOfControls], HEX);
    }
    
    countOfControls++;
    //	control button index = countOfControlSensors - 1 [8 buttons: index of first: 0, last: 7]
    
    return control[countOfControls - 1];
    
}

CCControl* CCDeviceScheduler::addControlEvent(String eventName) {
    if (countOfControls >= MAX_CONTROLS_PER_SCHEDULER - 1) return NULL;
    
    control[countOfControls] = new CCControlEvent(eventName, countOfControls);
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": provided controlEvent "));
        Serial.println(control[countOfControls]->getName());
    }
    if (verbosity & MEMORYDEBUG) {
        Serial.print(F("[CCDeviceScheduler]: "));
        Serial.print(schedulerName);
        Serial.print(F(": CCControl constructed at $"));
        Serial.println((long)control[countOfControls], HEX);
    }
    
    countOfControls++;
    //	control button index = countOfControls - 1 [8 buttons: index of first: 0, last: 7]
    
    return control[countOfControls - 1];
    
}



void CCDeviceScheduler::listControls() {
    Serial.print(F("[CCDeviceScheduler]: "));
    Serial.print(schedulerName);
    Serial.println(F(": Controls: "));
    for (unsigned int i = 0; i < countOfControls; i++) {
        Serial.print(F("   # "));
        Serial.print(i);
        Serial.print(F(", name: "));
        Serial.print(control[i]->getName());
        Serial.print(F(", type: "));
        Serial.println(getLiteralOfControllerType(control[i]->getType()));
    }
    Serial.println();
}


void CCDeviceScheduler::reviewTasks(CCWorkflow* currentWorkflow) {
    for (unsigned int df = 0; df < currentWorkflow->getCountOfDeviceFlows(); df++) {
        for (unsigned int t = 0; t < currentWorkflow->deviceFlow[df]->getCountOfTasks(); t++) {
            
            deviceInfoCode i = currentWorkflow->deviceFlow[df]->getDevice()->reviewValues(currentWorkflow->deviceFlow[df]->task[t]);
            
            if (i != DEVICE_OK) {
                Serial.print(currentWorkflow->getName());
                Serial.print(F(" > "));
                Serial.print(currentWorkflow->deviceFlow[df]->getName());
                Serial.print(F(" task #"));
                Serial.print(t);
                Serial.print(F(": "));
                Serial.println(getLiteralOfDeviceInfo(i));
                
            }
        }
    }
}



int CCDeviceScheduler::run(CCWorkflow* currentWorkflow) {
    
    unsigned long loopCounter = 0;
    unsigned int ongoingOperations = 0;
    bool breakLoop = false;
    notificationCode = 0;
    notificationText = "";
    
    int n = 0;
    
    if ((char)currentWorkflow->getWorkflowInfo() >= 0) {
        
        // prepare the loop
        
        if (verbosity & SHOW_TASK_VIEW) {
            Serial.print(F("[CCDeviceScheduler]: "));
            Serial.print(schedulerName);
            Serial.print(F(": prepare workflow "));
            Serial.println(currentWorkflow->getName());
        }
        
#ifdef ARDUINO_SIMULATION
        clearDeviceDisplay();
#endif
        
        for (unsigned int df = 0; df < currentWorkflow->getCountOfDeviceFlows(); df++) {
            currentDeviceFlow = currentWorkflow->deviceFlow[df];
            currentDeviceFlow->setTaskPointer(0);
            
            currentDevice = currentDeviceFlow->getDevice();
            currentDevice->setState(PENDING_MOVES);
            currentDevice->attachDevice();
            
            //	prepare first Tasks
            nextTaskID[df] = 0;
            currentDevice->prepareTask(currentDeviceFlow->task[nextTaskID[df]]);
            nextTaskID[df]++;
            
            if (verbosity & SHOW_TASK_VIEW) {
                Serial.print(currentWorkflow->getName());
                Serial.print(F("->"));
                Serial.print(currentDeviceFlow->getName());
                Serial.print(F(": setup first Task: current Position: "));
                Serial.print(currentDevice->getCurrentPosition());
                Serial.print(F(", target: "));
                Serial.println(currentDevice->getTarget());
            }
            
#ifdef ARDUINO_SIMULATION
            copyDeviceName(df, currentDeviceFlow->getDevice()->getName());
#endif
            
        }
     
        
        for (unsigned int fc = 0; fc < currentWorkflow->getCountOfFlowControls(); fc++) {
            for (unsigned int a = 0; a < currentWorkflow->flowControl[fc]->getCountOfActions(); a++) {
                currentWorkflow->flowControl[fc]->getAction(0)->setActionDone(false);
            }
        }

        
        for (unsigned int c = 0; c < currentWorkflow->getCountOfControls(); c++) {
            currentWorkflow->control[c]->read();
        }
        
        
        //  start the workflow
        if (verbosity & SHOW_TASK_VIEW) {
            Serial.print(F("[CCDeviceScheduler]: "));
            Serial.print(schedulerName);
            Serial.print(F(": run workflow "));
            Serial.println(currentWorkflow->getName());
        }
        
        unsigned long loopTime;
        unsigned long loopMicros, elapsedLoopMicros;
        unsigned long deviceLoopPeriod_min = 100000;
        unsigned long deviceLoopPeriod_max = 0;
        unsigned long controlPeriod_min = 100000;
        unsigned long controlPeriod_max = 0;
        unsigned long flowControlPeriod_min = 100000;
        unsigned long flowControlPeriod_max = 0;
        unsigned long taskStartTime = millis();
        
        //    unsigned long lastPrintTime = taskStartTime;
        
        unsigned int theFlowControlID = 0;
        unsigned int theControlID = 0;
        
        do {
           ongoingOperations = 0;
            
            loopMicros = micros();
            
            for (unsigned int df = 0; df < currentWorkflow->getCountOfDeviceFlows(); df++) {
                loopTime = millis();
                taskTime = loopTime - taskStartTime;
                
                currentDeviceFlow = currentWorkflow->deviceFlow[df];
                currentDevice = currentDeviceFlow->getDevice();
                currentTask = currentDeviceFlow->task[currentDeviceFlow->getTaskPointer()];
                
                if (currentDevice->getState() > SLEEPING) {                                                             // (== MOVING || MOVE_DONE || PENDING_MOVES)
                    if (currentDevice->getState() == MOVING) {                                                          // if device is moving...
                        currentDevice->operateTask();                              // so: move on!
                        
                        
#ifdef ARDUINO_SIMULATION
                        copyDevicePosition(df, currentDevice->getCurrentPosition());
                        if (currentDevice->getType() == SERVODEVICE) {
                            copyAnalogInValue(df, 2000.0 - currentDevice->getCurrentPosition());
                        }
#endif
                        
                        switch (currentDevice->getStopEvent()) {                                                        // is there a stopEvent defined?
                            case DATE:                                                                              // yes, device shall stop by date
                                if (taskTime > currentDevice->getStartTime() + currentDevice->getStartDelay() + currentDevice->getTimeout()) { // it's time to stop!
                                    handleStopEvent(taskTime, currentDeviceFlow);
                                }
                                break;
                                
                            case CONTROL:                                                                            // device shall stop by button
                                if (currentDevice->isStopTargetReached()) {
                                    handleStopEvent(taskTime, currentDeviceFlow);
                                }
                                break;
                                
                            case FOLLOW:                                                                // device shall stop when a device finished a task
                                if (currentTask->getStopTriggerDevice()->getCurrentTaskID() > currentTask->getStopTriggerTaskID()) { //  trigger device on trigger move?
                                    handleStopEvent(taskTime, currentDeviceFlow);
                                }
                                break;
                                
                            case FOLLOW_ME:                                                                // device shall stop when a device finished a task
                                    handleStopEvent(taskTime, currentDeviceFlow);
                                break;
                                
                            case POSITION:                                                           // device shall stop when a device reached a certain position
                                triggerDevice_stop = currentTask->getStopTriggerDevice();
                                if (triggerDevice_stop->getCurrentTaskID() >= currentTask->getStopTriggerTaskID()) {          //  trigger device on trigger move?
                                    if (triggerDevice_stop->getDirectionDown() == (triggerDevice_stop->getCurrentPosition() <= currentTask->getStopTriggerPosition())) {
                                        handleStopEvent(taskTime, currentDeviceFlow);
                                    }
                                }
                                break;
                                
                            case MY_POSITION:                                                           // device shall stop when it reached a certain position
                                if (currentDevice->getDirectionDown() == (currentDevice->getCurrentPosition() <= currentTask->getStopTriggerPosition())) {
                                    handleStopEvent(taskTime, currentDeviceFlow);
                                }
                                
                            default:
                                break;
                        }
                    }                                                                                               // (currentDevice->getState() == MOVING)
                    else {                                                                                          // if device is stopped
                        if (currentDevice->getState() == MOVE_DONE) {                                                   // finished right now?
                            
                            if (verbosity & SHOW_TASK_VIEW) {
                                Serial.print(taskTime);
                                Serial.print(F(": "));
                                Serial.print(currentDeviceFlow->getName());
                                Serial.print(F(" Task "));
                                Serial.print(currentDeviceFlow->getTaskPointer());
                                Serial.println(F(" done"));
                            }
                            
                            if (currentDevice->getSwitchTaskPromptly() > NO_SWITCHING) {
                                handlePreparation(taskTime, currentDeviceFlow);
                                currentDevice->setStartEvent(DATE);
                                currentDevice->setStartTime(taskTime);
   
                            } else {
                                currentDevice->finishTask();                                                        // no position reset in this case
                                currentDevice->disableDevice();                                                     // no device disableing in this case
                                if (handlePreparation(taskTime, currentDeviceFlow) < ALL_TASKS_DONE) {
                                    currentDevice->setState(PENDING_MOVES);
                                }
                            }
                            
                        }
                        else {    //  (currentDevice->getState() == MOVE_DONE)                                          //  device is idle
                            switch (currentDevice->getStartEvent()) {                                                   //  what kind of startEvent is given?
                                case DATE:                                                                          //  start the next move by date
                                    if (taskTime > currentDevice->getStartTime()) {
                                        handleStartEvent(taskTime, currentDevice);
                                    }
                                    break;
                                    
                                case CONTROL:                                                                        //  start the next move by button
                                    if (currentDevice->isStartTargetReached()) {
                                        handleStartEvent(taskTime, currentDevice);
                                    }
                                    break;
                                    
                                case FOLLOW:                                                                        //  start the next move when a device reached a certain
                                    if (currentTask->getStartTriggerDevice()->getCurrentTaskID() > currentTask->getStartTriggerTaskID()) {          //  is the trigger servo doing the trigger move?
                                        
                                        handleStartEvent(taskTime, currentDevice);
                                    }
                                    break;
                                    
                                case FOLLOW_ME:                                                                        //  start the next move when a device reached a certain
                                        handleStartEvent(taskTime, currentDevice);
                                    break;
                                    
                                case POSITION:                                                                      //  start the next move when a device reached a certain
                                    triggerDevice_start = currentTask->getStartTriggerDevice();
                                    if (triggerDevice_start->getCurrentTaskID() >= currentTask->getStartTriggerTaskID()) {        //  is the trigger servo doing the trigger move?
                                        
                                        if ((triggerDevice_start->getDirectionDown() && triggerDevice_start->getCurrentPosition() <= currentTask->getStartTriggerPosition()) || (!triggerDevice_start->getDirectionDown() && triggerDevice_start->getCurrentPosition() >= currentTask->getStartTriggerPosition())) {
                                            //  did the trigger servo pass the trigger position?
                                            handleStartEvent(taskTime, currentDevice);
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
                ongoingOperations += currentDevice->getState();                                                           // ongoing operations on any device?
                
            }

            elapsedLoopMicros = micros() - loopMicros;
            deviceLoopPeriod_min = min(deviceLoopPeriod_min, elapsedLoopMicros);
            deviceLoopPeriod_max = max(deviceLoopPeriod_max, elapsedLoopMicros);
            

            //        if (taskTime - lastPrintTime > 1000) {
            //            device[6]->getReadOut(0);
            //            lastPrintTime = taskTime;
            //        }

            loopMicros = micros();
            if (theControlID < currentWorkflow->getCountOfControls()) {
                currentWorkflow->control[theControlID]->read();
                theControlID++;
            } else {
                theControlID = 0;
            }
            elapsedLoopMicros = micros() - loopMicros;
            controlPeriod_min = min(controlPeriod_min, elapsedLoopMicros);
            controlPeriod_max = max(controlPeriod_max, elapsedLoopMicros);
            
            
            loopMicros = micros();
            if (theFlowControlID < currentWorkflow->getCountOfFlowControls()) {
                currentFlowControl = currentWorkflow->flowControl[theFlowControlID];
                n++;
                if (currentFlowControl->getName() == "repeatTurningControl") {
                    if (n > 100) {
                        currentFlowControl->getControl()->setVerbosity(BASICOUTPUT);
                        n = 0;
                    } else {
                        currentFlowControl->getControl()->setVerbosity(NO_OUTPUT);
                    }
                }
                if ((currentFlowControl->needsToFire())) {
                    for (unsigned int actionIndex = 0; actionIndex < currentFlowControl->getCountOfActions(); actionIndex++) {
                        currentAction = currentFlowControl->getAction(actionIndex);
                        if (!currentAction->getActionDone()) {
                            if ((currentAction->getValidTaskID() == -1) || (currentAction->getValidTaskID() == currentAction->targetDeviceFlow->getTaskPointer())) {
                                
                                currentWorkflow->setWorkflowInfo(currentAction->getWorkflowInfo());
                                
                                //                            notificationCode = min(notificationCode, currentAction->notificationCode);
                                //                            notificationText = notificationText + currentAction->notificationText + "\n";
                                
                                if (verbosity & SHOW_TASK_VIEW) {
                                    Serial.print(taskTime);
                                    Serial.print(F(": "));
                                    Serial.print(currentFlowControl->getName());
                                    Serial.print(F(" rise Action "));
                                    Serial.print(currentAction->getName());
                                    Serial.print(F(" with notification: "));
                                    Serial.println(currentAction->getNotificationText());
                                }
                                
                                switch (currentAction->getTargetAction()) {
                                    case BREAK_LOOP_ACTION:
                                        breakLoop = true;
                                        break;
                                    case TASK_START_ACTION:
                                        handleStartEvent(taskTime, currentAction->targetDeviceFlow->getDevice());
                                        break;
                                    case TASK_SWITCH_ACTION:
                                        currentDevice->setSwitchTaskPromptly(currentAction->getSwitching());
                                        nextTaskID[currentAction->targetDeviceFlow->getID()] = currentAction->getFollowingTaskID();
                                        if (currentAction->getSwitching() == SWITCH_PROMPTLY) {
                                            handleStopEvent(taskTime, currentAction->targetDeviceFlow);
                                        }
                                    case TASK_STOP_ACTION:
                                        currentDevice->setStopping(currentAction->getStopping());
                                        handleStopEvent(taskTime, currentAction->targetDeviceFlow);
                                        break;
                                    default: ;
                                }
                               }
                            currentAction->setActionDone(true);
                        }
                    }
                }
                theFlowControlID++;
            } else {
                theFlowControlID = 0;
            }
            elapsedLoopMicros = micros() - loopMicros;
            flowControlPeriod_min = min(flowControlPeriod_min, elapsedLoopMicros);
            flowControlPeriod_max = max(flowControlPeriod_max, elapsedLoopMicros);
            
            
            if (breakLoop) {
                break;
            }
            
            
            loopCounter++;
            
            
#ifdef ARDUINO_SIMULATION
            checkIfThreadIsStopped();
#endif
            
        } while (ongoingOperations > 0);
        
        
        for (unsigned int df = 0; df < currentWorkflow->getCountOfDeviceFlows(); df++) {
            currentDevice = currentWorkflow->deviceFlow[df]->getDevice();
            currentDevice->disableDevice();
            currentDevice->detachDevice();
        }
        
        if (breakLoop) {
            Serial.println(F("workflow canceled"));
        } else {
            Serial.println(F("workflow ended normally:"));
        }
        
        Serial.print(F("notificationCode:"));
        Serial.println(notificationCode);
        Serial.print(F("notificationText:"));
        Serial.println(notificationText);
        Serial.println();
        Serial.print(F("loops: "));
        Serial.print((signed long)loopCounter);
        Serial.print(F(", elapsed time: "));
        Serial.print((int)((millis() - taskStartTime) / 1000));
        Serial.print(F(", loops/second: "));
        Serial.println((float)1000.0 * loopCounter / (millis() - taskStartTime));
        
        Serial.print(F("min time for deviceLoop: "));
        Serial.print(deviceLoopPeriod_min);
        Serial.print(F(", max time for deviceLoop: "));
        Serial.print(deviceLoopPeriod_max);
        Serial.print(F(", min time for controls: "));
        Serial.print(controlPeriod_min);
        Serial.print(F(", max time for controls: "));
        Serial.print(controlPeriod_max);
        Serial.print(F(", min time for flowControls: "));
        Serial.print(flowControlPeriod_min);
        Serial.print(F(", max time for flowControls: "));
        Serial.print(flowControlPeriod_max);
        
        Serial.println();
        Serial.println();
        
    } else {
        Serial.println(F("workflow was disabled"));
    }
    
    
    return notificationCode;
}

deviceInfoCode CCDeviceScheduler::handleStartEvent(unsigned long taskTime, CCDevice* currentDevice) {
    
    if (verbosity & SHOW_TASK_VIEW) {
        Serial.print(taskTime);
        Serial.print(F(": "));
        Serial.print(currentDeviceFlow->getName());
        Serial.print(F(" start task "));
        Serial.print(currentDeviceFlow->getTaskPointer());
        Serial.print(" by ");
        Serial.println(getLiteralOfTaskEvent(currentDevice->getStartEvent()));
    }
    
    if (currentDevice->getStartDelay() > 0) {                                               // startDelay given?
        if (currentDevice->getStartEvent() == DATE) {
            currentDevice->setStartTime(currentDevice->getStartTime() + currentDevice->getStartDelay());
        } else {
            currentDevice->setStartTime(taskTime + currentDevice->getStartDelay());             // so start the move by date
            currentDevice->setStartEvent(DATE);
        }
        currentDevice->setStartDelay(0);
    }
    else {
        currentDevice->setStartTime(taskTime);
        currentDevice->enableDevice();
        currentDevice->startTask();
    }
    return TASK_START_OK;
}
deviceInfoCode CCDeviceScheduler::handleStopEvent(unsigned long taskTime, CCDeviceFlow* currentDeviceFlow) {
    currentDevice = currentDeviceFlow->getDevice();
    
    switch (currentDevice->getSwitchTaskPromptly()) {
        case SWITCH_PROMPTLY:
            switch (handlePreparation(taskTime, currentDeviceFlow)) {
                case DISPOSE_THIS_TASK:
                    currentDevice->initiateStop();
                    currentDevice->setStopEvent(NONE);
                    currentDevice->setSwitchTaskPromptly(SWITCH_AFTER_COMPLETION);
                    return TASK_DISPOSED;
                    
                case DELAY_THIS_TASK:
                    currentDevice->setStopEvent(DATE);
                    currentDevice->setTimeout(0);
                    return TASK_DELAYED;

                case TASK_PREPARATION_OK:
                    currentDeviceFlow->setTaskPointer(currentDevice->getCurrentTaskID());
                    nextTaskID[currentDeviceFlow->getID()] = currentDeviceFlow->getTaskPointer() + 1;
                    
                    if (verbosity & SHOW_TASK_VIEW) {
                        Serial.print(taskTime);
                        Serial.print(F(": "));
                        Serial.print(currentDeviceFlow->getName());
                        Serial.print(F(" stop task "));
                        Serial.print(currentDeviceFlow->getTaskPointer());
                        Serial.print(" by ");
                        Serial.print(getLiteralOfTaskEvent(currentDevice->getStopEvent()));
                        Serial.println(", switching promptly");
                    }
                    return TASK_PREPARATION_OK;
                    
                default:
                    return ALL_TASKS_DONE;
            }

        case SWITCH_AFTER_COMPLETION:
            currentDevice->initiateStop();
            currentDevice->setStopEvent(NONE);
            return TASK_DELAYED;
            
        default:
            if (currentDevice->getStopping() == STOP_IMMEDIATELY) {
                currentDevice->stopTask();
                
                if (verbosity & SHOW_TASK_VIEW) {
                    Serial.print(taskTime);
                    Serial.print(F(": "));
                    Serial.print(currentDeviceFlow->getName());
                    Serial.print(F(" stop task "));
                    Serial.print(currentDeviceFlow->getTaskPointer());
                    Serial.print(" by ");
                    Serial.print(getLiteralOfTaskEvent(currentDevice->getStopEvent()));
                    Serial.println(", stopping immediately");
                }
            } else {
                currentDevice->initiateStop();
                currentDevice->setStopEvent(NONE);
                
                if (verbosity & SHOW_TASK_VIEW) {
                    Serial.print(taskTime);
                    Serial.print(F(": "));
                    Serial.print(currentDeviceFlow->getName());
                    Serial.print(F(" initiate stop of task "));
                    Serial.print(currentDeviceFlow->getTaskPointer());
                    Serial.print(" by ");
                    Serial.println(getLiteralOfTaskEvent(currentDevice->getStopEvent()));
                }
            }
            break;
    }
    return TASK_PREPARATION_OK;
}
deviceInfoCode CCDeviceScheduler::handlePreparation(unsigned long taskTime, CCDeviceFlow* currentDeviceFlow) {
    currentDevice = currentDeviceFlow->getDevice();
    int currentDeviceFlowID = currentDeviceFlow->getID();
    deviceInfoCode info;
    
    if (nextTaskID[currentDeviceFlowID] < currentDeviceFlow->getCountOfTasks()) {                       //  all tasks done? no!
        
        info = currentDevice->prepareTask(currentDeviceFlow->task[nextTaskID[currentDeviceFlowID]]);
        currentDeviceFlow->setTaskPointer(currentDevice->getCurrentTaskID());
        nextTaskID[currentDeviceFlowID] = currentDeviceFlow->getTaskPointer() + 1;
        
        if (info >= DEVICE_OK) {
            if (verbosity & SHOW_TASK_VIEW) {
                Serial.print(taskTime);
                Serial.print(F(": "));
                Serial.print(currentDeviceFlow->getName());
                Serial.print(F(" prepare Task "));
                Serial.print(currentDeviceFlow->getTaskPointer());
                Serial.print(F(": current Position: "));
                Serial.print(currentDevice->getCurrentPosition());
                Serial.print(F(" target: "));
                Serial.print(currentDevice->getTarget());
                Serial.print(F("; result: "));
                Serial.println(getLiteralOfDeviceInfo(info));
            }
        }
        return info;
        
    }                                                                                   // all tasks are done
    
    nextTaskID[currentDeviceFlowID] = currentDeviceFlow->getTaskPointer() + 1;
    currentDeviceFlow->setTaskPointer(nextTaskID[currentDeviceFlowID]);
    currentDevice->setCurrentTaskID(currentDeviceFlow->getTaskPointer());
    
    if (currentDevice->getState() != SLEEPING) {
        currentDevice->finishTask();
        currentDevice->disableDevice();
    }
    
    if (verbosity & SHOW_TASK_VIEW) {
        Serial.print(taskTime);
        Serial.print(F(": "));
        Serial.print(currentDeviceFlow->getName());
        Serial.println(F(" finished all Tasks"));
        
    }
    
    return ALL_TASKS_DONE;
    
}

void CCDeviceScheduler::listAllTasks(CCWorkflow* workflow) {
    Serial.print(F("tasks of workflow "));
    Serial.println(workflow->getName());
    for (unsigned int df = 0; df < workflow->getCountOfDeviceFlows(); df++) {
        Serial.print(F("   deviceFlow "));
        Serial.print(workflow->deviceFlow[df]->getName());
        Serial.print(F(": with device: "));
        Serial.print((workflow->deviceFlow[df]->getDevice())->getName());
        Serial.print(F(", defaultVelocity: "));
        Serial.print(workflow->deviceFlow[df]->getDefaultVelocity());
        Serial.print(F(", defaultAcceleration: "));
        Serial.print(workflow->deviceFlow[df]->getDefaultAcceleration());
        Serial.print(F(", defaultDeceleration: "));
        Serial.print(workflow->deviceFlow[df]->getDefaultDeceleration());
        Serial.print(F(": tasks: "));
        Serial.println(workflow->deviceFlow[df]->getCountOfTasks());
        
        for (unsigned int i = 0; i < workflow->deviceFlow[df]->getCountOfTasks(); i++) {
            Serial.print(F("      # "));
            Serial.print(i);
            Serial.print(F(": target: "));
            Serial.print(workflow->deviceFlow[df]->task[i]->getTarget());
            //            Serial.print(F(", velocity: "));
            //            Serial.print(workflow->deviceFlow[df]->task[i]->getVelocity());
            //            Serial.print(F(", acceleration: "));
            //            Serial.print(workflow->deviceFlow[df]->task[i]->getAcceleration());
            if (workflow->deviceFlow[df]->task[i]->getMoveRelativ()) {
                Serial.print(F(", move relativ"));
            }
            if (workflow->deviceFlow[df]->task[i]->getPositionReset() == RESET_ON_START) {
                Serial.print(F(", position reset on start"));
            } else if (workflow->deviceFlow[df]->task[i]->getPositionReset() == RESET_ON_COMPLETION) {
                Serial.print(F(", position reset on completion"));
            }
            if (workflow->deviceFlow[df]->task[i]->getStartDelay() > 0) {
                Serial.print(F(", startDelay: "));
                Serial.print(workflow->deviceFlow[df]->task[i]->getStartDelay());
            }
            //            Serial.print(F(", started by "));
            //            Serial.print(getLiteralOfTaskEvent(workflow->deviceFlow[df]->task[i]->getStartEvent()));
            switch (workflow->deviceFlow[df]->task[i]->getStartEvent()) {
                case DATE:
                    Serial.print(F(", startTime: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTime());
                    break;
                case CONTROL:
                    Serial.print(F(", startControl: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartControl()->getName());
                    Serial.print(F(", "));
                    Serial.print(getLiteralOfControlValueComparing(workflow->deviceFlow[df]->task[i]->getStartControlComparing()));
                    Serial.print(F(", "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartControlTarget());
                    break;
                case FOLLOW:
                    Serial.print(F(", follows: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTriggerDevice()->getName());
                    Serial.print(F(", after move: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTriggerTaskID());
                    break;
                case FOLLOW_ME:
                    Serial.print(F(", running after me"));
                    break;
                case POSITION:
                    Serial.print(F(", started by position of: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTriggerDevice()->getName());
                    Serial.print(F(", on move: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTriggerTaskID());
                    Serial.print(F(", at position: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStartTriggerPosition());
                    break;
                case MY_POSITION:
                    Serial.print(F(", [THIS MAKES NO SENSE!]"));
                    break;
                default: ;
            }
            //            Serial.print(F(", terminated by: "));
            //            Serial.print(getLiteralOfTaskEvent(workflow->deviceFlow[df]->task[i]->getStopEvent()));
            
            switch (workflow->deviceFlow[df]->task[i]->getStopEvent()) {
                case DATE:
                    Serial.print(F(", timeout: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getTimeout());
                    Serial.print(", stopping: ");
                    Serial.print(getLiteralOfStoppingMode(workflow->deviceFlow[df]->task[i]->getStopping()));
                    if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() > NO_SWITCHING) {
                        if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() == SWITCH_AFTER_COMPLETION) {
                            Serial.print(F(", switch after completion"));
                        } else {
                            Serial.print(F(", switch promptly to next task"));
                        }
                    }
                    break;
                case CONTROL:
                    Serial.print(F(", stopControl: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopControl()->getName());
                    Serial.print(F(", "));
                    Serial.print(getLiteralOfControlValueComparing(workflow->deviceFlow[df]->task[i]->getStopControlComparing()));
                    Serial.print(F(", "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopControlTarget());
                    Serial.print(", stopping: ");
                    Serial.print(getLiteralOfStoppingMode(workflow->deviceFlow[df]->task[i]->getStopping()));
                    if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() > NO_SWITCHING) {
                        if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() == SWITCH_AFTER_COMPLETION) {
                            Serial.print(F(", switch after completion"));
                        } else {
                            Serial.print(F(", switch promptly to next task"));
                        }
                    }
                  break;
                case FOLLOW:
                    Serial.print(F(", follows: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerDevice()->getName());
                    Serial.print(F(", after move: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerTaskID());
                    Serial.print(", stopping: ");
                    Serial.print(getLiteralOfStoppingMode(workflow->deviceFlow[df]->task[i]->getStopping()));
                    if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() > NO_SWITCHING) {
                        if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() == SWITCH_AFTER_COMPLETION) {
                            Serial.print(F(", switch after completion"));
                        } else {
                            Serial.print(F(", switch promptly to next task"));
                        }
                    }
                    break;
                case FOLLOW_ME:
                    Serial.print(F(", [THIS MAKES NO SENSE!]"));
                    break;
                case POSITION:
                    Serial.print(F(", stop by position of: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerDevice()->getName());
                    Serial.print(F(", on move: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerTaskID());
                    Serial.print(F(", at position: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerPosition());
                    Serial.print(", stopping: ");
                    Serial.print(getLiteralOfStoppingMode(workflow->deviceFlow[df]->task[i]->getStopping()));
                    if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() > NO_SWITCHING) {
                        if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() == SWITCH_AFTER_COMPLETION) {
                            Serial.print(F(", switch after completion"));
                        } else {
                            Serial.print(F(", switch promptly to next task"));
                        }
                    }
                    break;
                case MY_POSITION:
                    Serial.print(F(", stop at position: "));
                    Serial.print(workflow->deviceFlow[df]->task[i]->getStopTriggerPosition());
                    Serial.print(", stopping: ");
                    Serial.print(getLiteralOfStoppingMode(workflow->deviceFlow[df]->task[i]->getStopping()));
                    if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() > NO_SWITCHING) {
                        if (workflow->deviceFlow[df]->task[i]->getSwitchTaskPromptly() == SWITCH_AFTER_COMPLETION) {
                            Serial.print(F(", switch after completion"));
                        } else {
                            Serial.print(F(", switch promptly to next task"));
                        }
                    }
                    break;
                default: ;
            }
            Serial.println();
        }
        Serial.println();
    }
}
void CCDeviceScheduler::listAllActions(CCWorkflow* workflow) {
    Serial.print(F("actions of workflow "));
    Serial.println(workflow->getName());
    for (unsigned int fc = 0; fc < workflow->getCountOfFlowControls(); fc++) {
        Serial.print(F("   flowControl "));
        Serial.print(workflow->flowControl[fc]->getName());
        Serial.print(F(", with device: "));
        Serial.print(workflow->flowControl[fc]->getControl()->getName());
        Serial.print(F(", actions: "));
        Serial.println(workflow->flowControl[fc]->getCountOfActions());
        
        for (unsigned int i = 0; i < workflow->flowControl[fc]->getCountOfActions(); i++) {
            Serial.print(F("      # "));
            Serial.print(i);
            Serial.print(F(": action: "));
            Serial.print(workflow->flowControl[fc]->getAction(i)->getName());
            Serial.print(F(", on deviceFlow: "));
            if (workflow->flowControl[fc]->getAction(i)->targetDeviceFlow == NULL) {
                Serial.print(F(" any"));
            } else {
                Serial.print(workflow->flowControl[fc]->getAction(i)->targetDeviceFlow->getName());
            }
            Serial.print(F(", on task: "));
            if (workflow->flowControl[fc]->getAction(i)->getValidTaskID() < 0) {
                Serial.print(F(" any"));
            } else {
                Serial.print(workflow->flowControl[fc]->getAction(i)->getValidTaskID());
            }
            Serial.print(F(", following: "));
            if (workflow->flowControl[fc]->getAction(i)->getFollowingTaskID() < 0) {
                Serial.print(F(" any"));
            } else {
                Serial.print(workflow->flowControl[fc]->getAction(i)->getFollowingTaskID());
            }
            Serial.print(F(", do: "));
            Serial.print(getLiteralOfDeviceAction(workflow->flowControl[fc]->getAction(i)->getTargetAction()));
            Serial.print(F(", infoCode: ["));
            Serial.print(getLiteralOfWorkflowInfo(workflow->flowControl[fc]->getAction(i)->getWorkflowInfo()));
            Serial.println(F("]"));
            
        }
        Serial.println();
    }
}


void CCDeviceScheduler::setVerbosity(int verbosity) {this->verbosity = verbosity;}


String CCDeviceScheduler::getLiteralOfDeviceType(deviceType t) {
    if (t == SERVODEVICE) return "Servo";
    if (t == STEPPERDEVICE) return "Stepper";
    if (t == DCCONTROLLERDEVICE) return "Switch";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfControllerType(controlType t) {
    if (t == BUTTON) return "Button";
    if (t == SENSOR) return "Sensor";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfTaskEvent(event e) {
    if (e == NONE) return "none";
    if (e == DATE) return "date";
    if (e == CONTROL) return "button or sensor";
    if (e == FOLLOW) return "follow";
    if (e == FOLLOW_ME) return "follow me";
    if (e == POSITION) return "position";
    if (e == MY_POSITION) return "my position";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfState(deviceState s) {
    if (s == SLEEPING) return "sleeping";
    if (s == MOVING) return "moving";
    if (s == MOVE_DONE) return "move done";
    if (s == PENDING_MOVES) return "pending moves";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfStoppingMode(stoppingMode s) {
    if (s == STOP_IMMEDIATELY) return "immediately";
    if (s == STOP_NORMAL) return "normal";
    if (s == STOP_DYNAMIC) return "dynamic";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfSwitchingMode(switchingMode s) {
    if (s == NO_SWITCHING) return "none";
    if (s == SWITCH_AFTER_COMPLETION) return "after completion";
    if (s == SWITCH_PROMPTLY) return "promptly";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfBooleanState(bool s) {
    if (s) return "HIGH";
    return "LOW";
}
String CCDeviceScheduler::getLiteralOfControlValueComparing(comparingMode c) {
    if (c == IS) return "is";
    if (c == IS_NOT) return "is not";
    if (c == IS_GREATER_THEN) return "is greater then";
    if (c == IS_SMALLER_THEN) return "is smaller then";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfDeviceAction(deviceAction d) {
    if (d == BREAK_LOOP_ACTION) return "break loop";
    if (d == TASK_START_ACTION) return "start";
    if (d == TASK_SWITCH_ACTION) return "switch";
    if (d == TASK_STOP_ACTION) return "stop";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfDeviceInfo(deviceInfoCode i) {
    if (i == DISPOSE_THIS_TASK) return "dispose task";
    if (i == DELAY_THIS_TASK) return "delay task";
    if (i == DEVICE_OK) return "everything ok";
    if (i == TASK_PREPARATION_OK) return "task preparation ok";
    return "unknown";
}
String CCDeviceScheduler::getLiteralOfWorkflowInfo(workflowInfoCode i) {
    if (i == WORKFLOW_CANCELED_ON_ERROR) return "workflow canceled on error";
    if (i == WORKFLOW_CANCELED_ON_ENDBUTTON_REACHED) return "workflow canceled on endpoint reached";
    if (i == WORKFLOW_CANCELED_ON_BUTTON_NOT_REACHED) return "workflow canceled on target not reached";
    if (i == WORKFLOW_CANCELED_ON_PARAMETER_ERROR) return "workflow canceled on parameter error";
    if (i == WORKFLOW_CANCELED_ON_FLOWCONTROL_ERROR) return "workflow canceled on flowcontrol error";
    if (i == WORKFLOW_DISABLED_ON_ERROR) return "workflow disabled on error";
    if (i == WORKFLOW_DISABLED_ON_ENDBUTTON_REACHED) return "workflow disabled on endpoint reached";
    if (i == WORKFLOW_DISABLED_ON_BUTTON_NOT_REACHED) return "workflow disabled on target not reached";
    if (i == EVERYTHING_OK) return "everything ok";
    if (i == SONGCANCELBUTTON_PRESSED) return "songCancelButtonPressed";
    if (i == SONGENDBUTTON_PRESSED) return "songEndButtonPressed";
    if (i == BUTTON_NOT_REACHED) return "target not reached";
    if (i == ENDBUTTON_REACHED) return "endpoint reached";
    return "unknown";
}






