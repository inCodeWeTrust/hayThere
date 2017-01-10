//
//  CCStepperDevice.cpp
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//



#include "CCStepperDevice.h"


CCStepperDevice::CCStepperDevice(const String deviceName, const unsigned int step_pin, const unsigned int dir_pin, const unsigned int enable_pin, const unsigned int stepsPerRotation) : CCDevice(deviceName, STEPPERDEVICE), step_pin(step_pin), dir_pin(dir_pin), enable_pin(enable_pin), stepsPerRotation(stepsPerRotation) {

    this->verbosity = NO_OUTPUT;

    this->acceleration_max = 4000;
    this->statusReportInterval = 500000;
    
    pinMode(dir_pin, OUTPUT);
    pinMode(step_pin, OUTPUT);
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, HIGH);
    

    this->steppingUnit = new unsigned int[(int)log2(HIGHEST_MICROSTEP_RESOLUTION) + 1];                         // create array for increment of microSteps according to microSteppingMode
        this->steppingUnit[0] = 1;
    
    this->highestSteppingMode = 0;

    
    this->stepsPerDegree = stepsPerRotation / 360.0;                                        // save time executing prepareNextTask()
    this->degreesPerMicroStep = 360.0 / stepsPerRotation / (1 << highestSteppingMode);      // save time when calculatin currentPosition in operateTask()
    
    
    this->currentMicroStep = 0;
    this->currentPosition = 0;
    
    
    //        Serial.print(F("[CCStepperDevice]: setup stepper "));
    //        Serial.print(deviceName);
    //        Serial.print(F(": currentPosition: "));
    //        Serial.print(currentPosition);
    //        Serial.print(F(", dir_pin: "));
    //        Serial.print(dir_pin);
    //        Serial.print(F(", step_pin: "));
    //        Serial.print(step_pin);
    //        Serial.print(F(", enable_pin: "));
    //        Serial.print(enable_pin);
    //        Serial.print(F(", stepsPerDegree: "));
    //        Serial.print(stepsPerDegree);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long) this, HEX);
    //        }
    //        Serial.println();
}

CCStepperDevice::~CCStepperDevice() {
    delete [] steppingUnit;
    steppingUnit = NULL;
}

void CCStepperDevice::attachDevice() {}
void CCStepperDevice::detachDevice() {}


void CCStepperDevice::enableDevice() {
    digitalWrite(enable_pin, LOW);
}
void CCStepperDevice::disableDevice() {
    digitalWrite(enable_pin, HIGH);
    digitalWrite(dir_pin, LOW);
    setupMicroSteppingMode(0);
}



deviceInfoCode CCStepperDevice::reviewValues(CCTask* nextTask) {
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCStepperDevice]: "));
        Serial.print(deviceName);
        Serial.print(F(" review values/calculate steps for given angle... "));
    }
    if (nextTask->getVelocity() == 0) return DEVICE_PARAMETER_ERROR;
    if (nextTask->getAcceleration() == 0) return DEVICE_PARAMETER_ERROR;
    if (nextTask->getDeceleration() == 0) nextTask->setDeceleration(nextTask->getAcceleration());

    
    // v[steps/sec] = v[angle/sec] * stepsPerAngle
    nextTask->setVelocity(nextTask->getVelocity() * stepsPerDegree);
    
    // a[steps/sec/sec] = a[angle/sec/sec] * stepsPerAngle
    nextTask->setAcceleration(nextTask->getAcceleration() * stepsPerDegree);
    nextTask->setDeceleration(nextTask->getDeceleration() * stepsPerDegree);
    
    if (verbosity & BASICOUTPUT) {
        Serial.println(F("   done"));
    }
    return DEVICE_OK;
}

void CCStepperDevice::prepareNextTask() {}
deviceInfoCode CCStepperDevice::prepareTask(CCTask* nextTask) {
    
//    unsigned long t_prepTask = micros();
//    unsigned long t_stop, t_sum = 0;

    if (state == MOVING) {
        if (switching) {
            if ((nextTask->getTarget() > currentPosition) == directionDown) {             // XOR: when switching to move in different direction
                
                if (verbosity & BASICOUTPUT) {
                    Serial.print(F("[CCStepperDevice]: "));
                    Serial.print(deviceName);
                    Serial.print(F(": dispose task "));
                    Serial.print(nextTask->getTaskID());
                    Serial.println(F(": need to slow down first"));
                }
                
                return DISPOSE_THIS_TASK;
            }
        }
        
        if (currentMicroStep & ((1 << highestSteppingMode) - 1)) {
            
            if (verbosity & BASICOUTPUT) {
                Serial.print(F("[CCStepperDevice]: "));
                Serial.print(deviceName);
                Serial.print(F(": delay task "));
                Serial.print(nextTask->getTaskID());
                Serial.println(F(": need to wait for next fullstep"));
            }

            return DELAY_THIS_TASK;
        }
        // *** current velocity ***
        if (currentMicroStep < microStepsForAcceleration) {
            // v = a * t
            currentVelocity = elapsedTime * 0.000001 * acceleration;
        }
        else if (currentMicroStep < microStepsForAccAndConstSpeed) {
            currentVelocity = velocity;
        }
        else {
            // v - v0 = a * t ==> v = a * t + v0
            currentVelocity = (elapsedTime - timeForAccAndConstSpeed) * 0.000001 * deceleration + velocity;
        }
    }
    else {
        currentVelocity = 0;
    }

    //    this takes ca 30us

    
    startPosition = currentPosition;
    
    currentTaskID = nextTask->getTaskID();
    
    target = nextTask->getTarget();
    velocity = nextTask->getVelocity();
    acceleration = nextTask->getAcceleration();
    deceleration = nextTask->getDeceleration();
    
    moveRelativ = nextTask->getMoveRelativ();
    positionReset = nextTask->getPositionReset();
    
    switching = nextTask->getSwitching();
    stopping = nextTask->getStopping();
    approximation = nextTask->getApproximation();
    
    targetReachedCounter = 0;
    
    
    
    //    this takes ca 24us


    
    if (positionReset == RESET_ON_START) {
        currentPosition = 0.0;
    }
    
    if (moveRelativ) {
        stepsToGo = target * stepsPerDegree;
    } else {
        stepsToGo = (target - currentPosition) * stepsPerDegree;
    }
    
    //    this takes ca 4us

    
    if (stepsToGo < 0) {
        stepsToGo = -stepsToGo;
        directionDown = true;
    }
    else {
        directionDown = false;
    }
    
    //    this takes ca 4us
    
    
    veloBySquare = velocity * velocity;
    currVeloBySquare = currentVelocity * currentVelocity;
    
    
    // *** steps for deceleration: ***
    // v * v = 2 * a * n
    // ==> v * v = 2 * a * n ==> n = v * v / (2 * a)
    stepsForDeceleration = (unsigned long)(veloBySquare / deceleration) >> 1;
    stepsForDeceleration = fmax(stepsForDeceleration, 1);
    
    //    this takes ca 40us
    

    accelerateDown = (currentVelocity > velocity);
    
    // *** steps for acceleration: ***
    // v * v - v0 * v0 = 2 * a * n
    // ==> n = (v * v - v0 * v0) / (2 * a)
    if (accelerateDown) {
        stepsForAcceleration = (unsigned long)((currVeloBySquare - veloBySquare) / acceleration) >> 1;
    }
    else {
        stepsForAcceleration = (unsigned long)((veloBySquare - currVeloBySquare) / acceleration) >> 1;
    }
    stepsForAcceleration = fmax(stepsForAcceleration, 1);
    
    //    this takes ca 60us
    
    
    // *** does acceleration and deceleration fit into the move? ***
    if ((signed long)stepsForAcceleration + (signed long)stepsForDeceleration > stepsToGo - 2) {
        
        Serial.println(F("!!!! too less steps !!!!"));

        // v * v - v0 * v0 = 2 * a_acc * s_acc ==> s_acc = (v * v - v0 * v0) / (2 * a_acc)
        // v * v = 2 * a_dec * s_dec => s_dec = v * v / (2 * a_dec)
        // ==> s_acc / s_dec = (v * v - v0 * v0) / (2 * a_acc) / (v * v / (2 * a_dec)) = a_dec * (v * v - v0 * v0) / (a_acc * v * v)
        // s = s_acc + s_dec;
        // s_acc / s_dec = c ==> s_acc = c * s_dec
        // ==> s = c * s_dec + s_dec = s_dec * (c + 1) ==> s_dec = s / (c + 1)
        // ==> s_dec = s / (a_dec * (v * v - v0 * v0) / (a_acc * v * v) + 1)
        if (accelerateDown) {
            stepsForDeceleration = (float) stepsToGo / ((currVeloBySquare - veloBySquare) * deceleration / (acceleration * veloBySquare) + 1);
        }
        else {
            stepsForDeceleration = (float) stepsToGo / ((veloBySquare - currVeloBySquare) * deceleration / (acceleration * veloBySquare) + 1);
        }
        stepsForAcceleration = stepsToGo - 2 - stepsForDeceleration--;
        
    }
    
    //    this takes ca 4us (no recalculation)
    
    
    // *** recalculate a: ***
    // v * v = 2 * a * n ==> a = v * v / (2 * n)
    deceleration = -veloBySquare / (stepsForDeceleration << 1);
    
    if (deceleration < -acceleration_max) {                             // !!!! too much deceleration !!!!
        Serial.println(F("!!!! too much deceleration !!!!"));
        deceleration = -acceleration_max;
        // *** recalculate v: ***
        // v * v = 2 * a * n ==> v = sqrt(2 * a * n)
        //        veloBySquare = 2 * acceleration_max * stepsForDeceleration;
        veloBySquare = acceleration_max * (stepsForDeceleration << 1);
        velocity = sqrt(veloBySquare);
    }
    
    // *** recalculate a: ***
    if (stepsForAcceleration > 0) {                                     // !!!! too much acceleration !!!!
        // v * v - v0 * v0 = 2 * a * n ==> a = (v * v - v0 * v0) / (2 * n)
        acceleration = (veloBySquare - currVeloBySquare) / (stepsForAcceleration << 1);
        if (acceleration > acceleration_max) {
            Serial.println(F("!!!! too much acceleration !!!!"));
            acceleration = acceleration_max;
            // *** recalculate v: ***
            // v * v - v0 * v0 = 2 * a * n ==> v * v = 2 * a * n + v0 * v0
            veloBySquare = (stepsForAcceleration << 1) * acceleration_max + currVeloBySquare;
            velocity = sqrt(veloBySquare);
            
            deceleration = -veloBySquare / (stepsForDeceleration << 1);
        }
        else if (acceleration < -acceleration_max){
            Serial.println(F("!!!! too much acceleration !!!!"));
            acceleration = -acceleration_max;
            // *** recalculate v: ***
            // v * v - v0 * v0 = 2 * a * n ==> v * v = 2 * a * n + v0 * v0
            veloBySquare = (stepsForAcceleration << 1) * acceleration_max + currVeloBySquare;
            velocity = sqrt(veloBySquare);
            
            deceleration = -veloBySquare / (stepsForDeceleration << 1);
        }
        acceleration_inv = 1 / acceleration;
    }
    deceleration_inv = 1 / deceleration;
    
    
    // *** time for acceleration: ***
    // a = v0 / t0; a = v / t ==> t0 = v0 / a; t = v / a
    // tx = t - t0; ==> tx = v / a - v0 / a = (v - v0) / a
    // in case accelerateUp: (velocity - currentVelocity) > 0 ==> acceleration_inv > 0 ==> timeForAcceleration > 0
    // in case accelerateDown: (velocity - currentVelocity) < 0 ==> acceleration_inv < 0 ==> timeForAcceleration > 0
    timeForAcceleration = 1000000.0 * (velocity - currentVelocity) * acceleration_inv;
    timeForAccAndConstSpeed = 1000000.0 * (stepsToGo - stepsForAcceleration - stepsForDeceleration) / velocity + timeForAcceleration;
    

    // *** time for the next step while acceleration: ***
    // n = 0.5 * a * t * t + v0 * t
    // 0.5 * a * t * t + v0 * t - n = 0
    // ==> t = (-v0 +/- sqrt(v0 * v0 + 2 * a * n)) / a [* 1000000us/s]
    // ==> t = (-v0 +/- sqrt(v0 * v0 + 2 * a * microSteps / microStepsPerStep)) / a [* 1000000us/s]
    // ==> t = (-v0 +/- sqrt(v0 * v0 + microSteps * 2 * a / microStepsPerStep)) / a [* 1000000us/s]
    // 2 * a / microStepsPerStep = constant = c0
    // t = (-v0 +/- sqrt(v0 * v0 + microSteps * c0)) / a [* 1000000us/s]
    c0_acc = 2.0 * acceleration / (1 << highestSteppingMode);
    c0_dec = 2.0 * deceleration / (1 << highestSteppingMode);
    
    
    
    // *** time for next step while constant speed: ***
    // v = n / t ==> t = n / v
    // t = microSteps / microStepsPerStep / v
    // ==> t = microSteps / (microStepsPerStep * v) [*1000000us/s]
    // 1000000.0 / (microStepsPerStep * v) = constant = c1
    // t = microSteps * c1
    c1 = 1000000.0 / (velocity * (1 << highestSteppingMode));
        
    //    this takes ca 132us
    

    microStepsToGo = stepsToGo << highestSteppingMode;
    microStepsForAcceleration = stepsForAcceleration << highestSteppingMode;
    microStepsForAccAndConstSpeed = microStepsToGo - (stepsForDeceleration << highestSteppingMode);
    
    //    this takes ca 12us
    
    
    if (state == MOVING) {
        t0 += lastStepTime;
        currentMicroStep = steppingUnit[microSteppingMode];
        stepExpiration = 1000000.0 * (sqrt(currVeloBySquare + currentMicroStep * c0_acc) - currentVelocity) * acceleration_inv;
        
        //    this takes ca 88us
        

   }
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCStepperDevice]: "));
        Serial.print(deviceName);
        Serial.print(F(": praparing... "));
        Serial.print(F("currentPosition: "));
        Serial.print(currentPosition);
        Serial.print(F(", target: "));
        Serial.println(this->target);
    }
    if (verbosity & CALCULATIONDEBUG) {
        Serial.print(F("[CCStepperDevice]: "));
        Serial.print(deviceName);
        Serial.print(F(": praparing... "));
        Serial.print(F(", stepsToGo: "));
        Serial.print(this->stepsToGo);
        Serial.print(F(" ("));
        Serial.print(this->microStepsToGo);
        Serial.print(F("), stepsForAcceleration: "));
        Serial.print(this->stepsForAcceleration);
        Serial.print(F(" ("));
        Serial.print(this->microStepsForAcceleration);
        Serial.print(F("), stepsForAccAndConstSpeed: "));
        Serial.print(this->stepsToGo - this->stepsForDeceleration);
        Serial.print(F(" ("));
        Serial.print(this->microStepsForAccAndConstSpeed);
        Serial.print(F("), stepsForDeceleration: "));
        Serial.print(this->stepsForDeceleration);
        Serial.print(F("), currentVelocity: "));
        Serial.print(this->currentVelocity);
        Serial.print(F(", velocity: "));
        Serial.print(this->velocity);
        Serial.print(F(", acceleration: "));
        Serial.print(this->acceleration);
        Serial.print(F(", deceleration: "));
        Serial.println(this->deceleration);
    }
    return DEVICE_OK;
}


void CCStepperDevice::startTask() {                                 // start this move
    if (stepsToGo == 0 || velocity == 0 || acceleration == 0) {     // values valid?
        stopTask();
    }
    else {
        digitalWrite(I_AM_LATE_LED, LOW);
        
        digitalWrite(dir_pin, directionDown);                       // setup DIR-pin of stepper driver
        
        // lets start in highest stepping mode
        microSteppingMode = highestSteppingMode;                    // setup stepper driver's highest steppingMode
        setupMicroSteppingMode();
                
        if (verbosity & BASICOUTPUT) {
            Serial.print(F("[CCStepperDevice]: "));
            Serial.print(deviceName);
            Serial.println(F(": starting task"));
        }
        
        targetReachedCounter = 0;

        state = MOVING;                                             // tag device as MOVING
        currentMicroStep = 0;                                       // start counting at 0
        stepExpiration = 0;                                         // set time for next step to 0 (= now)
        t0 = micros() & 0x7fffffff;                                 // remember starting time (but be aware of overflows)
        
        nextStatusReportTime = t0 + statusReportInterval;
        
        operateTask();                                              // do first step and calculate time for next step
    }
}

void CCStepperDevice::initiateStop() {                              // irregular stopping
    if (currentMicroStep < microStepsForAcceleration) {             // if stop request appears while accelerating
                                                                    // stop acceleration immediately and decelerate symmetrical
        // what is my speed?
        // v * v [steps/s] = 2 * a * steps = 2 * a * microStep / microStepsPerFullStep
        veloBySquare = 2.0 * fabs(acceleration) * currentMicroStep / (1 << highestSteppingMode);
        velocity = sqrt(veloBySquare);
        
        // do deceleration symetrical to acceleration: take same amount of microSteps for deceleration as used for acceleration till here
        microStepsToGo = 2 * currentMicroStep;                      // stepsToGo = stepsForAcceleration + stepsForDeceleration
        microStepsForAcceleration = currentMicroStep;               // stepsForAcceleration = currentStep = stepsForDeceleration
        microStepsForAccAndConstSpeed = currentMicroStep;           // stepsForConstSpeed = 0
        timeForAccAndConstSpeed = stepExpiration;                   // time for start of deceleration is time of next step
        return;
    }
    if (currentMicroStep < microStepsForAccAndConstSpeed) {         // when going with constant speed
        microStepsToGo = currentMicroStep + (stepsForDeceleration << highestSteppingMode);  // use nominated deceration values
        microStepsForAccAndConstSpeed = currentMicroStep;           // no more stepsForConstSpeed
        timeForAccAndConstSpeed = stepExpiration;                   // time for start of deceleration is time of next step
        return;
    }
}

void CCStepperDevice::stopTask() {
    state = MOVE_DONE;

    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCStepperDevice]: "));
        Serial.print(deviceName);
        Serial.println(F(": stopping task"));
    }
}
void CCStepperDevice::finishTask() {
    state = SLEEPING;
        
    if (positionReset == RESET_ON_COMPLETION) {
        currentPosition = 0.0;
    }

    digitalWrite(I_AM_LATE_LED, LOW);

    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCStepperDevice]: "));
        Serial.print(deviceName);
        Serial.println(F(": task done"));
    }
}

void CCStepperDevice::operateTask() {
    long now = micros() & 0x7fffffff;
    if (t0 > now) t0 -= 0x80000000;
    
    elapsedTime = now - t0;
    
    signed long timeTillNextStep = stepExpiration - elapsedTime;                                    // timeTillNextStep > 0 time for step will come soon
                                                                                                    // timeTillNextStep < 0 time for step already expired
    
    if (timeTillNextStep <= 0) {                                                                    // time for a step already expired?
        digitalWrite(step_pin, LOW);
        digitalWrite(step_pin, HIGH);
        
        if (timeTillNextStep < -200) {
            if (currentMicroStep < microStepsToGo) {
                digitalWrite(I_AM_LATE_LED, HIGH);
            }
        }
        else {
            digitalWrite(I_AM_LATE_LED, LOW);
        }
        
        currentMicroStep += steppingUnit[microSteppingMode];
        
        if (directionDown) {
            currentPosition = startPosition - currentMicroStep * degreesPerMicroStep;
        }
        else {
            currentPosition = startPosition + currentMicroStep * degreesPerMicroStep;
        }
        
        
        // while ramping up
        if (currentMicroStep <= microStepsForAcceleration) {
            lastStepTime = stepExpiration;
            stepExpiration = 1000000.0 * (sqrt(currVeloBySquare + currentMicroStep * c0_acc) - currentVelocity) * acceleration_inv;
            
            if (accelerateDown) {
                if (stepExpiration - lastStepTime > STEPPINGPERIOD_TO_KICK_DOWN) kickDown();
            }
            else {
                if (stepExpiration - lastStepTime < STEPPINGPERIOD_TO_KICK_UP) kickUp();
            }
            
            if (verbosity & MOVEMENTDEBUG) {
                //        Serial.print(F("[CCStepperDevice]: "));
                //        Serial.print(deviceName);
                Serial.print((unsigned long)elapsedTime);
                Serial.print(F(": up   , # "));
                Serial.print(currentMicroStep);
                Serial.print(F(", Pos: "));
                Serial.print(currentPosition);
                Serial.print(F(", next step: "));
                Serial.println((unsigned long)stepExpiration);
            }
            return;
        }
        
        // while going with constant speed
        if (currentMicroStep < microStepsForAccAndConstSpeed) {
            lastStepTime = stepExpiration;
            stepExpiration = timeForAcceleration + (currentMicroStep - microStepsForAcceleration) * c1;
            
            
            if (verbosity & MOVEMENTDEBUG) {
                //        Serial.print(F("[CCStepperDevice]: "));
                //        Serial.print(deviceName);
                Serial.print((unsigned long)elapsedTime);
                Serial.print(F(": const, # "));
                Serial.print(currentMicroStep);
                Serial.print(F(", Pos: "));
                Serial.print(currentPosition);
                Serial.print(F(", next step: "));
                Serial.println((unsigned long)stepExpiration);
            }
            return;
        }
        
        // while ramping down
        if (currentMicroStep < microStepsToGo) {
            lastStepTime = stepExpiration;
            stepExpiration = timeForAccAndConstSpeed + (sqrt((currentMicroStep - microStepsForAccAndConstSpeed) * c0_dec + veloBySquare) - velocity) * deceleration_inv * 1000000.0;
                        
            if (stepExpiration - lastStepTime > STEPPINGPERIOD_TO_KICK_DOWN) kickDown();
            
            if (verbosity & MOVEMENTDEBUG) {
                //        Serial.print(F("[CCStepperDevice]: "));
                //        Serial.print(deviceName);
                Serial.print((unsigned long)elapsedTime);
                Serial.print(F(": down , # "));
                Serial.print(currentMicroStep);
                Serial.print(F(", Pos: "));
                Serial.print(currentPosition);
                Serial.print(F(", next step: "));
                Serial.println((unsigned long)stepExpiration);
            }
            return;
        }
        
        // very last step
        if (currentMicroStep == microStepsToGo) {
            stepExpiration = timeForAccAndConstSpeed - velocity * deceleration_inv;         // deceleration_inv < 0
            
            if (verbosity & MOVEMENTDEBUG) {
                //        Serial.print(F("[CCStepperDevice]: "));
                //        Serial.print(deviceName);
                Serial.print((unsigned long)elapsedTime);
                Serial.print(F(": last , # "));
                Serial.print(currentMicroStep);
                Serial.print(F(", Pos: "));
                Serial.print(currentPosition);
                Serial.print(F(", very last: "));
                Serial.println((unsigned long)stepExpiration);
            }
            return;
        }
        
        // if we didnt return up to here, we are done!
        
            stopTask();
    }
    
    if (elapsedTime > nextStatusReportTime) {
        readStatus();
        nextStatusReportTime += statusReportInterval;
    }
    

}

void CCStepperDevice::readStatus() {}

void CCStepperDevice::kickUp() {
    if (microSteppingMode > 0) {
        if ((currentMicroStep & ((1 << highestSteppingMode) - 1)) == 0) {
            microSteppingMode--;
            
            setupMicroSteppingMode();
            
        }
    }
}

void CCStepperDevice::kickDown() {
    if (microSteppingMode < highestSteppingMode) {
        if ((currentMicroStep & ((1 << highestSteppingMode) - 1)) == 0) {
            microSteppingMode++;
            
            setupMicroSteppingMode();
            
        }
    }
}

uint16_t CCStepperDevice::getDeviceDriverStatus(deviceDriverStatusInfo info) {}


