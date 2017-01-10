//
//  CCStepperDevice.h
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCStepperDevice__
#define __deviceScheduler__CCStepperDevice__


#include "CCDevice.h"


#define HIGHEST_MICROSTEP_RESOLUTION        256


///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCStepperDevice
///
/// @brief Device class for stepper motor devices
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ARDUINO_SIMULATION
static SPICommunication SPI;
#else
#include <SPI.h>
#endif



class CCStepperDevice : public CCDevice {
protected:
    
    signed long         stepsToGo;
    unsigned long       stepsForAcceleration, stepsForDeceleration, stepsForAccAndConstSpeed;
    unsigned long       timeForAcceleration, timeForAccAndConstSpeed;
    unsigned long       microStepsToGo, microStepsForAcceleration, microStepsForAccAndConstSpeed;
    bool                accelerateDown;
    float               currentVelocity, currVeloBySquare, veloBySquare;
    signed long         t0;
    long                nextStatusReportTime;
    int                 statusReportInterval;
    unsigned long       stepExpiration, elapsedTime, lastStepTime;
    float               c0_acc, c0_dec, c1, acceleration_inv, deceleration_inv;
    unsigned int        microSteppingMode;       // 0: none, 1: halfStep, 2: quarterStep, 3: eigthStep, 4: sixteenthStep
    unsigned int        *steppingUnit;
    float               stepsPerDegree, degreesPerMicroStep;
    unsigned long       currentMicroStep;
    float               startPosition;
    bool                prepareAndStartNextTaskWhenFinished, switchAtNextFullStep;

    void                kickDown();
    void                kickUp();
    
    virtual void        setupMicroSteppingMode() = 0;
    virtual void        setupMicroSteppingMode(unsigned int data) = 0;
    
    
    unsigned int        highestSteppingMode;
    int                 acceleration_max;
    
    
    const unsigned int        step_pin;
    const unsigned int        dir_pin;
    const unsigned int        enable_pin;
    const unsigned int        stepsPerRotation;
    

    
    virtual void                readStatus();
    
    
public:
    CCStepperDevice(const String deviceName, const unsigned int step_pin, const unsigned int dir_pin, const unsigned int enable_pin, const unsigned int stepsPerRotation);
    virtual ~CCStepperDevice();
    
    
    void                attachDevice();
    void                detachDevice();
    void                enableDevice();
    void                disableDevice();
    
    deviceInfoCode      reviewValues(CCTask* nextTask);
    deviceInfoCode      prepareTask(CCTask* nextTask);
    void                prepareNextTask();
    void                startTask();
    void                operateTask();
    void                initiateStop();
    void                stopTask();
    void                finishTask();

    virtual void        getReadOut(unsigned int theReadOut) = 0;
    virtual uint16_t            getDeviceDriverStatus(deviceDriverStatusInfo info);
    
    
};


#endif // defined(__deviceScheduler__CCStepperDevice__)
