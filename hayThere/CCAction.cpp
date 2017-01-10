//
//  CCAction.cpp
//  deviceScheduler
//
//  Created by little Ikarus on 10.07.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#include "CCAction.h"



CCAction::CCAction(const String actionName, workflowInfoCode workflowInfo) : actionName(actionName) {
    this->verbosity = NO_OUTPUT;
    
    this->workflowInfo = workflowInfo;
    this->targetDeviceFlow = NULL;
    this->validTaskID = -1;
    this->targetAction = DO_NOTHING;
    this->followingTaskID = -1;
    this->actionDone = false;
    
    //        Serial.print(F("[CCAction]: setup action "));
    //        Serial.print(actionName);
    //        Serial.print(F(" with notification: "));
    //        Serial.print(notificationText);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long)this, HEX);
    //        }
    //        Serial.println();

}

void CCAction::evokeTaskStart(CCDeviceFlow* targetDeviceFlow, CCTask* validTask) {
    this->targetDeviceFlow = targetDeviceFlow;
    this->validTaskID = validTask->getTaskID();
    this->followingTaskID = -1;
    this->targetAction = TASK_START_ACTION;
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeTaskStop on task "));
        Serial.print(this->validTaskID);
        Serial.println();
    }
}
void CCAction::evokeTaskStartOfTask(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, CCTask* followingTask) {
    this->targetDeviceFlow = targetDeviceFlow;
    if (validTask == NULL) {
        this->validTaskID = -1;
    } else {
        this->validTaskID = validTask->getTaskID();
    }
    this->followingTaskID = followingTask->getTaskID();
    this->targetAction = TASK_START_ACTION;
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeTaskStop on task "));
        Serial.print(this->validTaskID);
        Serial.println();
    }

}



void CCAction::evokeTaskStop(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, stoppingMode stopping) {
    this->targetDeviceFlow = targetDeviceFlow;
    this->validTaskID = validTask->getTaskID();
    this->stopping = stopping;
    
    this->targetAction = TASK_STOP_ACTION;

    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeTaskStop on task "));
        Serial.print(this->validTaskID);
        Serial.println();
    }
}

void CCAction::evokeJumpToNextTask(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, switchingMode switching) {
    this->targetDeviceFlow = targetDeviceFlow;
    this->validTaskID = validTask->getTaskID();
    this->switching = switching;
    
    this->followingTaskID = -1;
    this->targetAction = TASK_SWITCH_ACTION;

    //    this->notificationCode = notificationCode;
    //    this->notificationText = notificationText;
    
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeJumpToNextTask on task "));
        Serial.print(this->validTaskID);
        Serial.print(F(", targetAction: "));
        Serial.print(this->targetAction);
        Serial.print(F(", followingTaskID: "));
        Serial.print(this->followingTaskID);
        Serial.println();
    }
    
}
void CCAction::evokeJumpToTask(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, switchingMode switching, CCTask* followingTask) {
    this->targetDeviceFlow = targetDeviceFlow;
    if (validTask == NULL) {
        this->validTaskID = -1;
    } else {
        this->validTaskID = validTask->getTaskID();
    }
    this->switching = switching;
    this->followingTaskID = followingTask->getTaskID();
    this->targetAction = TASK_SWITCH_ACTION;
    
//    this->notificationCode = notificationCode;
//    this->notificationText = notificationText;
    
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeJumpToTask on task "));
        Serial.print(this->validTaskID);
        Serial.print(F(", targetAction: "));
        Serial.print(this->targetAction);
        Serial.print(F(", followingTaskID: "));
        Serial.println(this->followingTaskID);
    }
    
}


void CCAction::evokeBreak(CCDeviceFlow* targetDeviceFlow, CCTask* validTask) {
    this->targetDeviceFlow = targetDeviceFlow;
    if (validTask == NULL) {
        this->validTaskID = -1;
    } else {
        this->validTaskID = validTask->getTaskID();
    }
    this->targetAction = BREAK_LOOP_ACTION;
    this->followingTaskID = 1;
    
//    this->notificationCode = notificationCode;
//    this->notificationText = notificationText;
    
    
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCAction]: "));
        Serial.print(actionName);
        Serial.print(F(": evokeBreak on task "));
        Serial.println(this->validTaskID);
    }
    
    
}
void CCAction::evokeBreak() {
    evokeBreak(NULL, NULL);
}


const String CCAction::getName() const {return actionName;}

workflowInfoCode CCAction::getWorkflowInfo(){return workflowInfo;}
stoppingMode CCAction::getStopping() {return stopping;}
switchingMode CCAction::getSwitching() {return switching;}
int CCAction::getValidTaskID(){return validTaskID;}
deviceAction CCAction::getTargetAction(){return targetAction;}
int CCAction::getFollowingTaskID(){return followingTaskID;}
int CCAction::getNotificationCode(){return notificationCode;}
String CCAction::getNotificationText(){return notificationText;}
bool CCAction::getActionDone(){return actionDone;}
void CCAction::setActionDone(bool d){actionDone = d;}
bool CCAction::getRiseActionRepeatedly(){return riseActionRepeatedly;}
void CCAction::setRiseActionRepeatedly(bool riseActionRepeatedly){this->riseActionRepeatedly = riseActionRepeatedly;}

void CCAction::setVerbosity(int verbosity) {this->verbosity = verbosity;}


