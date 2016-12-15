//
//  CCDeviceFlow.cpp
//  deviceScheduler
//
//  Created by little Ikarus on 16.06.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#include "CCDeviceFlow.h"



void CCDeviceFlow::setVerbosity(int verbosity) {this->verbosity = verbosity;}

CCDeviceFlow::CCDeviceFlow(const String deviceFlowName, const unsigned int deviceFlowID, const CCDevice* device, float defaultVelocity, float defaultAcceleration, float defaultDeceleration) : deviceFlowName(deviceFlowName), deviceFlowID(deviceFlowID), device(device), defaultVelocity(defaultVelocity), defaultAcceleration(defaultAcceleration), defaultDeceleration(defaultDeceleration) {

    this->verbosity = NO_OUTPUT;
    
    this->countOfTasks = 0;
    
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
    if (countOfTasks >= MAX_TASKS_PER_DEVICEFLOW - 1) return NULL;

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

CCDevice* CCDeviceFlow::getDevice() {return (CCDevice*)device;}
const unsigned int CCDeviceFlow::getID() const {return deviceFlowID;}
const String CCDeviceFlow::getName() const {return deviceFlowName;}
unsigned int CCDeviceFlow::getCountOfTasks(){return countOfTasks;}
void CCDeviceFlow::setCountOfTasks(unsigned int count){countOfTasks = count;}
int CCDeviceFlow::getTaskPointer(){return taskPointer;}
void CCDeviceFlow::setTaskPointer(int pointer){taskPointer = pointer;}
void CCDeviceFlow::increaseTaskPointer(){taskPointer++;}
float CCDeviceFlow::getDefaultVelocity(){return defaultVelocity;}
float CCDeviceFlow::getDefaultAcceleration(){return defaultAcceleration;}
float CCDeviceFlow::getDefaultDeceleration(){return defaultDeceleration;}
