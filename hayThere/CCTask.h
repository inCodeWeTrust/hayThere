//
//  CCTask.h
//  deviceScheduler
//
//  Created by little Ikarus on 07.01.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCTask__
#define __deviceScheduler__CCTask__


#include "deviceScheduler.h"



///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCTask
///
/// @brief Class for tasks to be executed
///////////////////////////////////////////////////////////////////////////////////////////////////

class CCDevice;
class CCControl;


class CCTask {

    const unsigned int      taskID;

    float                   target;
    float                   velocity;
    float                   acceleration;
    float                   deceleration;
    bool                    moveRelativ;
    positionResetMode       positionReset;

    
    event                   startEvent;
    unsigned long           startTime;
    CCControl*              startControl;
    comparingMode           startControlComparing;
    int                     startControlTarget;
    CCDevice*               startTriggerDevice;
    unsigned int            startTriggerTaskID;
    signed long             startTriggerPosition;
    unsigned long           startDelay;

    event                   stopEvent;
    unsigned long           timeout;
    CCControl*              stopControl;
    comparingMode           stopControlComparing;
    int                     stopControlTarget;
    CCDevice*               stopTriggerDevice;
    unsigned int            stopTriggerTaskID;
    signed long             stopTriggerPosition;

    stoppingMode            stopping;
    switchingMode           switching;
    
    signed int              initiatePerformanceValue;
    float                   stopPerformance;
    unsigned int            approximationCurve;
    unsigned int            gap;
    bool                    reversedApproximation;
    approximationMode       approximation;
    
    
    int                     verbosity;

   
public:
    
    /// |              | startEvent                                           | stopEvent
    /// | -------------|-------------------------------------------------------------------------------
    /// | NONE         | no specific starting given                           | no specific stopping given
    /// |              |                                                      |
    /// | DATE         | start at a specific time                             | stop after a timeout
    /// |              | @c startByDate()                                     | @c stopByTimeout()
    /// |              |                                                      | @c switchToNextTaskByDate()
    /// |              |                                                      |
    /// | CONTROL      | start on a hardware event                            | stop on a hardware event
    /// |              | @c startByControl()                                  | @c stopByControl()
    /// |              |                                                      | @c switchToNextTaskByControl()
    /// |              |                                                      |
    /// | FOLLOW       | start when task of other device finished             | stop when task of other device finished
    /// |              | @c startAfterCompletionOf()                          | @c stopAfterCompletionOf()
    /// |              |                                                      | @c switchToNextTaskAfterCompletionOf()
    /// |              |                                                      |
    /// | FOLLOW_ME    | start when previous task finished                    |
    /// |              | @c startAfterCompletion()                            |
    /// |              |                                                      |
    /// | POSITION     | start when other device reached a specific position  | stop when other device reached a specific position
    /// |              | @c startByTriggerpositionOf()                        | @c stopByTriggerpositionOf()
    /// |              |                                                      | @c switchToNextTaskByTriggerpositionOf()
    /// |              |                                                      |
    /// | MY_POSITION  |                                                      | stop when previous reached a specific position
    /// |              |                                                      | @c stopAtPosition()
    /// |              |                                                      | @c switchToNextTaskAtPosition()
    
    
    
    
    void setStartDelay(unsigned long startDelay);
    
    void startByDate(unsigned long startTime);
    void startByControl(CCControl* startControl, comparingMode comparing, int target, approximationMode approximation = SKIP_APPROXIMATION_IMMEDIATELY);
    void startAfterCompletion();
    void startAfterCompletionOf(CCDevice* startTriggerDevice, CCTask* startTriggerTask);
    void startByTriggerpositionOf(CCDevice* startTriggerDevice, CCTask* startTriggerTask, signed long startTriggerPosition);
    
    void switchToNextTaskByDate(unsigned long switchingTimeout);
    void switchToNextTaskByControl(CCControl* switchingButton, comparingMode comparing, int target, approximationMode approximation = SKIP_APPROXIMATION_IMMEDIATELY);
    void switchToNextTaskAfterCompletionOf(CCDevice* switchingTriggerDevice, CCTask* switchingTriggerTask);
    void switchToNextTaskAtPosition(signed long switchingTriggerPosition);
    void switchToNextTaskByTriggerpositionOf(CCDevice* switchingTriggerDevice, CCTask* switchingTriggerTask, signed long switchingTriggerPosition);
    
    void stopByTimeout(unsigned long timeout, stoppingMode stopping);
    void stopByControl(CCControl* stopControl, comparingMode comparing, int target, stoppingMode stopping = STOP_NORMAL, approximationMode approximation = SKIP_APPROXIMATION_IMMEDIATELY);
    void stopAfterCompletionOf(CCDevice* stopTriggerDevice, CCTask* stopTriggerTask, stoppingMode stopping);
    void stopByTriggerpositionOf(CCDevice* stopTriggerDevice, CCTask* stopTriggerTask, signed long stopTriggerPosition, stoppingMode stopping);
    void stopAtPosition(signed long stopTriggerPosition, stoppingMode stopping);
    
    void stopDynamicallyBySensor(CCControl* sensor, unsigned int initiatePerformanceValue, unsigned int targetValue, float stopPerformance, approximationMode approximation);
    
    void stopDynamicallyBySensor_new(CCControl* stopControl, unsigned int stopControlTarget, float approximationCurve, float gap, approximationMode approximation);
    
    

    CCTask(const unsigned int taskID, float target, float velocity, float acceleration, float deceleration, bool moveRelativ, positionResetMode positionReset);
    
    
    
    
    
    

    /// Getter method for getting the id of the task
    /// @sa taskID;
    unsigned int        getTaskID() const;
    
    /// Getter method for getting the target of the device
    /// @sa target;
    float               getTarget();
    
    /// Setter method for setting the target of the device
    /// @sa target;
    void                setTarget(float target);
    
    /// Getter method for getting the velocity of the device
    /// @sa velocity;
    float               getVelocity();
    
    /// Setter method for setting the velocity of the device
    /// @sa velocity;
    void                setVelocity(float velocity);
    
    /// Getter method for getting the acceleration of the device
    /// @sa acceleration;
    float               getAcceleration();
    
    /// Setter method for setting the acceleration of the device
    /// @sa acceleration;
    void                setAcceleration(float acceleration);
    
    /// Getter method for getting the deceleration of the device
    /// @sa deceleration;
    float               getDeceleration();
    
    /// Setter method for setting the deceleration of the device
    /// @sa deceleration;
    void                setDeceleration(float deceleration);
    
    /// Getter method for getting the moveRelativ bit of the device
    /// @sa moveRelativ;
    bool                getMoveRelativ();

    /// Getter method for getting the positionReset mode of the device
    /// @sa positionReset;
    positionResetMode   getPositionReset();
    
    /// Getter method for getting the startDelay of the device
    /// @sa startDelay;
    unsigned long       getStartDelay();
    
    /// Getter method for getting the startEvent of the device
    /// @sa startEvent, event;
    event               getStartEvent();
    
    /// Getter method for getting the stopEvent of the device
    /// @sa stopEvent, event;
    event               getStopEvent();
    
    /// Getter method for getting the startTime of the device
    /// @sa startTime;
    unsigned long       getStartTime();
    
    /// Getter method for getting the timeOut of the device
    /// @sa timeOut;
    unsigned long       getTimeout();
    
    /// Getter method for getting the startControl of the device
    /// @sa startControl;
    CCControl*          getStartControl();
    
    /// Getter method for getting the stopControl of the device
    /// @sa stopControl;
    CCControl*          getStopControl();
    
    /// Getter method for getting the target value of startControl
    /// @sa startControlTarget;
    int                 getStartControlTarget();

    /// Getter method for getting the target value of stopControl
    /// @sa startControlTarget;
    int                 getStopControlTarget();

    /// Getter method for getting the comparing mode between the target value and the state of startControl
    /// @sa startControlTarget;
    comparingMode       getStartControlComparing();

    /// Getter method for getting the comparing mode between the target value and the state of stopControl
    /// @sa startControlTarget;
    comparingMode       getStopControlComparing();

    /// Getter method for getting the startTriggerDevice of the device
    /// @sa startTriggerDevice;
    CCDevice*           getStartTriggerDevice();
    
    /// Getter method for getting the stopTriggerDevice of the device
    /// @sa stopTriggerDevice;
    CCDevice*           getStopTriggerDevice();
    
    /// Getter method for getting the startTriggerTask of the device
    /// @sa startTriggerTask;
    unsigned int        getStartTriggerTaskID();
    
    /// Getter method for getting the stopTriggerTask of the device
    /// @sa stopTriggerTask;
    unsigned int        getStopTriggerTaskID();
    
    /// Getter method for getting the startTriggerPosition of the device
    /// @sa startTriggerPosition;
    signed long         getStartTriggerPosition();
    
    /// Getter method for getting the stopTriggerPosition of the device
    /// @sa stopTriggerPosition;
    signed long         getStopTriggerPosition();
    
    /// Getter method for getting the stopping of the device
    /// @sa stopping, stoppingMode;
    stoppingMode        getStopping();
    
    /// Getter method for getting the switching bit of the device
    /// @sa switching;
    switchingMode       getSwitching();
    
    /// Getter method for getting the sensor value to initiate the dynamical stop of the device
    /// @sa initiatePerformanceValue;
    signed int          getInitiatePerformanceValue();
    
    /// Getter method for getting the stopping performance of the device
    /// @sa stopPerformance;
    float               getStopPerformance();
    
    /// Getter method for getting the approximation value of the device
    /// @sa approximation;
    approximationMode   getApproximation();
    
    /// Getter method for getting the approximationCurve value of this task
    /// @sa approximationCurve;
    unsigned int        getApproximationCurve();
    
    /// Getter method for getting the approximation value of this task
    /// @sa gap;
    unsigned int        getGap();
    
    /// Getter method for getting the direction of the approximation value of this task
    /// @sa approximationCurve;
    bool                getReversedApproximation();
    
    /// the verbosity of the device
    void                setVerbosity(int verbosity);

    
};







#endif // defined(__deviceScheduler__CCTask__)
