//
//  CCDcControllerDevice.h
//  deviceScheduler
//
//  Created by little Ikarus on 17.03.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCDcControllerDevice__
#define __deviceScheduler__CCDcControllerDevice__


#include "CCDevice.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCDcControllerDevice
///
/// @brief Device class for devices controlled by PWM-signals or spimply switched on and off
/// [target](@ref target): duty cycle of pwm signal.
/// [velocity](@ref velocity): pwm frequency in n/sec = Hz.
/// [acceleration](@ref acceleration): time for ramping up.
/// [deceleration](@ref deceleration): time for ramping down.
///
///////////////////////////////////////////////////////////////////////////////////////////////////


class CCDcControllerDevice : public CCDevice {
    
protected:
    
    const unsigned int        switching_pin;
    const bool                switchingPin_active;
    
    // target: dutycycle [0 ... 1.0]
    // velocity: pwm frequency [Hz]
    // acceleration: time for ramping up [ms]
    // deceleration: time for ramping down [ms]
    
    /// The current task's starting time.
    unsigned long       t0;
    
    /// The elapsed time since this task was started or switched.
    signed long         elapsedTime, elapsedTime_previous;
    
    /// The time for ramping up @<= acceleration@>.
    signed long         timeForAcceleration;
    
    /// The time for ramping down @<= deceleration@>.
    signed long         timeForDeceleration;
    
    /// This variable could be used when the task should be terminated automatically.
    signed long         timeForAccAndConstSpeed;
    
    /// The switching time interval @<1 / pwmFrequency = 1 / velocity@>.
    unsigned int        switchingInterval;
    
    /// The duration of onTime @< = target * switchingInterval@>.
    unsigned int        targetOnDuration;
    
    /// The effectiv date for switching on.
    signed long         switchOnTime;
    
    /// The effectiv date for switching off.
    signed long         switchOffTime;
    
    /// Variable is @c true when switched on, @c false when switched off.
    bool                isActive;
    
    /// The initial rate of duty cycle, when the task is switched, like v0.
    float               currentRatio;
    
    /// The sensor's value
    unsigned int        sensorValue;

    /// The distance from the target position
    signed int          relativePosition, relativPosition_previous;
    
    
public:
    
    CCDcControllerDevice(const String deviceName, const unsigned int switching_pin, const bool switchingPin_active);
    ~CCDcControllerDevice();
    
    
    void                attachDevice();
    void                detachDevice();
    void                enableDevice();
    void                disableDevice();
    
    deviceInfoCode      reviewValues(CCTask* nextTask);
    void                prepareNextTask();
    deviceInfoCode      prepareTask(CCTask* nextTask);
    void                startTask();
    void                operateTask();
    void                initiateStop();
    void                stopTask();
    void                finishTask();
    
    void                getReadOut(unsigned int theReadOut);
    
};


#endif // defined(__deviceScheduler__CCDcControllerDevice__)