//
//  CCDevice.h
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCDevice__
#define __deviceScheduler__CCDevice__


#include "deviceScheduler.h"
#include "CCTask.h"
#include "CCControl.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCDevice
///
/// @brief base class for all devices
/// This baseclass holds all parameters, that are common for all devices, including the array of tasks to be executed by this device
///
///
///////////////////////////////////////////////////////////////////////////////////////////////////

class CCDevice {
protected:
    int verbosity;
    
    
    /// Parameters of the device
    /// Value, set by the user to be recognized by him.
    const String               deviceName;
    
    /// Parameters of the device
    /// Parameter that holds the type of the device.
    ///
    /// type                | description
    /// --------------------|--------------------------------------------------------------------------
    /// SERVODEVICE         | modell servo
    /// STEPPERDEVICE       | stepper motor with stepper driver
    /// DCCONTROLLERDEVICE  | any device to be pwm driven
    const deviceType           type;

    
    unsigned int         currentTaskID;

    /// Device parameter:
    /// The device's current position.
    float                currentPosition;
    
    /// Device parameter:
    /// This value indicates, if device's position values increase (´false´) or decrease (`true`) while the task is executed.
    bool                 directionDown;
    
    /// Device parameter:
    /// This value indicates the device's current state.
    ///
    /// deviceState         | description
    /// --------------------|--------------------------------------------------------------------------
    /// SLEEPING            | device is idle, no pending moves
    /// MOVING              | device is performing a task
    /// MOVE_DONE           | device just finished a task
    /// PENDING_MOVES       | device is idle, tasks have to be executed
    deviceState          state;
    
    
    /// Parameter, related to the current task operation.
    /// The value means different things, depending on the device type.
    float               target;
    
    /// Parameter, related to the current task operation.
    /// The value means different things, depending on the device type.
    float               velocity;
    
    /// Parameter, related to the current task operation.
    /// The value means different things, depending on the device type.
    float               acceleration;
    
    /// Parameter, related to the current task operation.
    /// The value means different things, depending on the device type.
    float               deceleration;
    
    /// Parameter, related to the current task operation.
    /// The value determines, if the provided target value is meant absolute or relativ.
    bool             moveRelativ;
    
    /// Parameter, related to the current task operation.
    /// The value determines, if current position is set to zero before job start.
    positionResetMode   positionReset;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds a delay to wait after occurance of the startEvent to start this task.
    unsigned long       startDelay;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter describes, how the tast is started:
    ///
    /// startEvent  | behavior
    /// ------------|-------------------------------------------------------------------------------
    /// NONE        | no specific starting given
    /// DATE        | starting at a specific time
    /// CONTROL      | starting on a hardware event
    /// FOLLOW      | starting when an other task of an other device is just finished
    /// POSITION    | starting when an other task of an other device has reached a specific position
    event                startEvent;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter describes, how the tast is started:
    ///
    /// stopEvent   | behavior
    /// ------------|-------------------------------------------------------------------------------
    /// NONE        | no specific stopping given
    /// DATE        | stopping after a timeout
    /// CONTROL      | stopping on a hardware event
    /// FOLLOW      | stopping when an other task of an other device is just finished
    /// POSITION    | stopping when an other task of an other device has reached a specific position
    event                stopEvent;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the time to start this task (ms after the scheduler was started).
    unsigned long        startTime;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the timeout to stop this task (ms after the task was started).
    unsigned long        timeout;

    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the the way stopping this task, when a stopping event occures.
    /// stoppingMode         | behavior
    /// ---------------------|--------------------------------------------------------------------
    /// STOP_IMMEDIATELY     | stop immediately
    /// STOP_NORMAL          | decelerate til halt with the given deceleration
    /// STOP_DYNAMIC         | approximate til a sensor shows a given value (reserved)
    stoppingMode         stopping;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter indicates if any ongoing task should be completed first, before this task is started.
    switchingMode       switchTaskPromptly;
    
    
    CCControl*              startControl;
    CCControl*              stopControl;
    comparingMode           startControlComparing;
    comparingMode           stopControlComparing;
    int                     startControlTarget;
    int                     stopControlTarget;

    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the control device of a required hardware event to stop this task.
//    CCControl*           sensor;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the numerical value of the sensor pin, required to initiate the stopping of this task.
    signed int           initiatePerformanceValue;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the numerical value of the sensor pin, on whitch this task comletes.
//    signed int           targetValue;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the feedback amplification while approximating the target position.
    float                stopPerformance;
    
    /// Parameter, related to the device controll of the current task.
    /// This value is for calculating the velocity while dynamical approximation. It's Value is > 0. The smaller the value, the sharper the stop.
    unsigned int         approximationCurve;
    
    /// Parameter, related to the device controll of the current task.
    /// Value holds the width of the target area, where the target is seen to be reached. It is @c targetValue +/- @c gap.
    unsigned int         gap;
    
    /// Parameter, related to the device controll of the current task.
    /// This value indicates if a dynamical stop is performed on increasing or decreasing sensor values.
    bool              reversedApproximation;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the approximaton behavior, attaining the target position.
    /// approximationMode                         | behavior
    /// ------------------------------------------|--------------------------------------------------------------------
    /// SKIP_APPROXIMATION_IMMEDIATELY = 0x00     | stop immediately when the first sensor value raised the target
    /// SKIP_APPROXIMATION_VERY_FAST = 0x01       | stop when one value is within a luxuriantly tolerance
    /// SKIP_APPROXIMATION_FAST = 0x40            | ...
    /// SKIP_APPROXIMATION_PRECISE = 0x80         | ...
    /// SKIP_APPROXIMATION_VERY_PRECISE = 0xC0    | stop not until a huge amount of values are within a small tolerance
    /// SKIP_APPROXIMATION_NEVER = 0xFF           | keep approximating
    /// (skip when ´appriximationMode´ times the sensor value is within the tolerance of ´ +/- 256 / approximationMode´)
    approximationMode   approximation;
    
    /// The number of times, the target was reached in series
    int         targetReachedCounter;
    

    
    
    
    
     //        startTime, startDelay & startEvent could be changed by scheduler, so they need to exist aswell outside of the onEventTask
    
    
    bool isTargetReached(CCControl* control, comparingMode comparing, int controlTarget);

    
    
public:

    CCDevice(const String deviceName, const deviceType type);
     virtual ~CCDevice() = 0;


    void setVerbosity(int verbosity);
    
    virtual void attachDevice() = 0;
    virtual void detachDevice() = 0;
    virtual void enableDevice() = 0;
    virtual void disableDevice() = 0;
    
    virtual deviceInfoCode reviewValues(CCTask* nextTask) = 0;
    virtual void prepareNextTask() = 0;
    virtual deviceInfoCode prepareTask(CCTask* nextTask) = 0;
    virtual void startTask() = 0;
    virtual void operateTask() = 0;
    virtual void initiateStop() = 0;
    virtual void stopTask() = 0;
    virtual void finishTask() = 0;
    
    bool isStartTargetReached();
    bool isStopTargetReached();



    virtual void getReadOut(unsigned int theReadOut) = 0;

    
    
    
    /// Getter method for getting the name of the device
    /// @sa deviceName;
    const String getName();
    
    /// Getter method for getting the type of the device
    /// @sa deviceType;
    deviceType getType();
    
    
    unsigned int getCurrentTaskID();
    void setCurrentTaskID(unsigned int taskID);
    
    
    /// Getter method for getting the current position of the device
    /// @sa currentPosition;
    float getCurrentPosition();
    
    /// Setter method for setting the current position of the device
    /// @sa currentPosition;
    void setCurrentPosition(float position);
    
    /// Getter method for getting the target of the device
    /// @sa target;
    float getTarget();
    
    /// Getter method for getting the velocity of the device
    /// @sa velocity;
    float getVelocity();
    
    /// Getter method for getting the acceleration of the device
    /// @sa acceleration;
    float getAcceleration();
    
    /// Getter method for getting the deceleration of the device
    /// @sa deceleration;
    float getDeceleration();
    
    /// Getter method for getting the counting direction of the moving device
    /// @sa directionDown;
    bool getDirectionDown();
    
    /// Getter method for getting the state of the device
    /// @sa deviceState;
    deviceState getState();
    
    /// Setter method for Setting the state of the device
    /// @sa deviceState;
    void setState(deviceState state);
    
    /// Getter method for getting the startDelay of the device
    /// @sa startDelay;
    unsigned long getStartDelay();
    
    /// Setter method for setting the startDelay of the device
    /// @sa startDelay;
    void setStartDelay(unsigned long delay);
    
    /// Getter method for getting the startEvent of the device
    /// @sa startEvent, event;
    event getStartEvent();
    
    /// Setter method for setting the startEvent of the device
    /// @sa startEvent, event;
    void setStartEvent(event startEvent);
    
    /// Getter method for getting the stopEvent of the device
    /// @sa stopEvent, event;
    event getStopEvent();
    
    /// Setter method for setting the stopEvent of the device
    /// @sa stopEvent, event;
    void setStopEvent(event stopEvent);
    
    /// Getter method for getting the startTime of the device
    /// @sa startTime;
    unsigned long getStartTime();
    
    /// Setter method for setting the startTime of the device
    /// @sa startTime;
    void setStartTime(unsigned long startTime);
    
    /// Getter method for getting the timeOut of the device
    /// @sa timeOut;
    unsigned long getTimeout();

    void setTimeout(unsigned long timeout);

//    bool getDisposedTaskWaiting();
//    void setDisposedTaskWaiting(bool disposed);
    
    
    /// Getter method for getting the stopping of the device
    /// @sa stopping, stoppingMode;
    stoppingMode getStopping();
    
    /// Setter method for setting the stopping of the device
    /// @sa stopping, stoppingMode;
    void setStopping(stoppingMode mode);
    
    /// Getter method for getting the switchTaskPromptly bit of the device
    /// @sa switchTaskPromptly;
    switchingMode getSwitchTaskPromptly();
    
    /// Setter method for setting the switchTaskPromptly bit of the device
    /// @sa switchTaskPromptly;
    void setSwitchTaskPromptly(switchingMode switchPromptly);
    
    
//    /// Getter method for getting the startControl of the device
//    /// @sa startControl;
//    CCControl* getControl();

    /// Getter method for getting the sensor of the device
    /// @sa sensor;
//    CCControl* getSensor();
    
    /// Getter method for getting the sensor value to initiate the dynamical stop of the device
    /// @sa initiatePerformanceValue;
    signed int getInitiatePerformanceValue();
    
    /// Getter method for getting the target sensor value of the device
    /// @sa targetValue;
//    signed int getTargetValue();
    
    /// Getter method for getting the stopping performance of the device
    /// @sa stopPerformance;
    float getStopPerformance();
    
    /// Getter method for getting the approximationCurve value of this task
    /// @sa approximationCurve;
    unsigned int getApproximationCurve();
    
    /// Getter method for getting the approximation value of this task
    /// @sa gap;
    unsigned int getGap();
    
    /// Getter method for getting the direction of the approximation value of this task
    /// @sa approximationCurve;
    bool getReversedApproximation();
    
    /// Getter method for getting the approximation value of the device
    /// @sa approximation;
    approximationMode getApproximation();
//    
//    /// The number of times, the target was reached in series
//    int getTargetReachedCounter();
//    
//    /// The number of times, the target was reached in series
//    void setTargetReachedCounter(int t);
    
    

    
};

#endif // defined(__deviceScheduler__CCDevice__)


//    /// Getter method for getting the startTriggerDevice of the device
//    /// @sa startTriggerDevice;
//    CCDevice* getStartTriggerDevice();
//
//    /// Getter method for getting the stopTriggerDevice of the device
//    /// @sa stopTriggerDevice;
//    CCDevice* getStopTriggerDevice();
//
//    /// Getter method for getting the startTriggerTask of the device
//    /// @sa startTriggerTask;
//    unsigned int getStartTriggerTaskID();
//
//    /// Getter method for getting the stopTriggerTask of the device
//    /// @sa stopTriggerTask;
//    unsigned int getStopTriggerTaskID();
//
//   /// Getter method for getting the startTriggerPosition of the device
//    /// @sa startTriggerPosition;
//    signed long getStartTriggerPosition();
//
//    /// Getter method for getting the stopTriggerPosition of the device
//    /// @sa stopTriggerPosition;
//    signed long getStopTriggerPosition();



/// \startuml
/// class CCDevice {
/// .. public members ..
///     {abstact} ~CCDevice()
///     {abstact} void attachDevice() = 0;
///     {abstact} void detachDevice() = 0;
///     {abstact} void enableDevice() = 0;
///     {abstact} void disableDevice() = 0;
///
///     {abstact} deviceInfoCode reviewValues(CCTask* nextTask) = 0;
///     {abstact} void prepareNextTask() = 0;
///     {abstact} void prepareTask(CCTask* nextTask) = 0;
///     {abstact} void startTask() = 0;
///     {abstact} void operateTask() = 0;
///     {abstact} void initiateStop() = 0;
///     {abstact} void stopTask() = 0;
///     {abstact} void finishTask() = 0;
///
///     {abstact} void getReadOut(unsigned char theReadOut) = 0;
///     +String getName();
///     +deviceType getType();
///     +unsigned int getCurrentTaskID();
///     +float getCurrentPosition();
///     +void setCurrentPosition(float position);
///     +float getTarget();
///     +float getVelocity();
///     +float getAcceleration();
///     +float getDeceleration();
///     +bool getDirectionDown();
///     +deviceState getState();
///     +void setState(deviceState state);
///     +unsigned long getStartDelay();
///     +void setStartDelay(unsigned long delay);
///     +event getStartEvent();
///     +void setStartEvent(event startEvent);
///     +event getStopEvent();
///     +void setStopEvent(event stopEvent);
///     +unsigned long getStartTime();
///     +void setStartTime(unsigned long startTime);
///     +unsigned long getTimeout();
///     +stoppingMode getStopping();
///     +void setStopping(stoppingMode mode);
///     +bool getSwitchTaskPromptly();
///     +void setSwitchTaskPromptly(bool switchPromptly);
///     +unsigned char getSensor();
///     +signed int getInitiatePerformanceValue();
///     +signed int getTargetValue();
///     +float getStopPerformance();
///     +unsigned int getApproximationCurve();
///     +unsigned int getGap();
///     +bool getReversedApproximation();
///     +approximationMode getApproximation();
/// \enduml



