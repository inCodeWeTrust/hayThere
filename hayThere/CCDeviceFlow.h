//
//  CCDeviceFlow.h
//  deviceScheduler
//
//  Created by little Ikarus on 16.06.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCDeviceFlow__
#define __deviceScheduler__CCDeviceFlow__


#include "deviceScheduler.h"
#include "CCDevice.h"
#include "CCControl.h"
#include "CCTask.h"




class CCDeviceFlow {
    
    const String        deviceFlowName;
    const unsigned int  deviceFlowID;
    
    
    CCDevice*     device;

    /// Default parameters for the device.
    /// A value for the device's default velocity is provided here.
    float               defaultVelocity;
    
    /// Default parameters for the device.
    /// A value for the device's default acceleration is provided here.
    float               defaultAcceleration;
    
    /// Default parameters for the device.
    /// A value for the device's default deceleration is provided here.
    float               defaultDeceleration;

    /// Parameters of the device as a peer of a run-loop.
    /// Value, that holds the number of tasks to be executed within this run-loop.
    unsigned int        countOfTasks;
    
    /// Parameters of the device as a peer of a run-loop.
    /// Value, that holds the index of the current task.
    int                 taskPointer;
    
    unsigned int        proposedTaskID;
    bool                startProposedTaskAutomatically;
    
    
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter describes, how the tast is started:
    ///
    /// startEvent  | behavior
    /// ------------|-------------------------------------------------------------------------------
    /// NONE        | no specific starting given
    /// DATE        | starting at a specific time
    /// CONTROL     | starting on a hardware event
    /// FOLLOW      | starting when an other task of an other device is just finished
    /// POSITION    | starting when an other task of an other device has reached a specific position
    event               startEvent;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the time to start this task (ms after the scheduler was started).
    unsigned long       startTime;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the CControl device to start this task.
    CCControl*          startControl;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the comparing mode between the given target and the state of the CControl device. [values: IS, IS_NOT, IS_GREATER_THEN, IS_SMALLER_THEN]
    comparingMode       startControlComparing;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the target value of the CControl device to start this task.
    int                 startControlTarget;
    
    CCDevice*               startTriggerDevice;
    unsigned int            startTriggerTaskID;
    signed long             startTriggerPosition;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds a delay to wait after occurance of the startEvent to start this task.
    unsigned long       startDelay;
    
    
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter describes, how the tast is started:
    ///
    /// stopEvent   | behavior
    /// ------------|-------------------------------------------------------------------------------
    /// NONE        | no specific stopping given
    /// DATE        | stopping after a timeout
    /// CONTROL     | stopping on a hardware event
    /// FOLLOW      | stopping when an other task of an other device is just finished
    /// POSITION    | stopping when an other task of an other device has reached a specific position
    event               stopEvent;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the timeout to stop this task (ms after the task was started).
    unsigned long       timeout;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the CControl device to stop this task.
    CCControl*          stopControl;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the comparing mode between the given target and the state of the CControl device. [values: IS, IS_NOT, IS_GREATER_THEN, IS_SMALLER_THEN]
    comparingMode       stopControlComparing;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the target value of the CControl device to stop this task.
    int                 stopControlTarget;
    
    CCDevice*               stopTriggerDevice;
    unsigned int            stopTriggerTaskID;
    signed long             stopTriggerPosition;
    
    
    
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter holds the the way stopping this task, when a stopping event occures.
    /// stoppingMode         | behavior
    /// ---------------------|--------------------------------------------------------------------
    /// STOP_IMMEDIATELY     | stop immediately
    /// STOP_NORMAL          | decelerate til halt with the given deceleration
    /// STOP_DYNAMIC         | approximate til a sensor shows a given value (reserved)
    stoppingMode        stopping;
    
    /// Parameter, related to the task controll of the current task.
    /// This parameter indicates if any ongoing task should be completed first, before this task is started.
    switchingMode       switching;
    
    

    /// Method, related to the device controll of the current task.
    /// Method determines if the target is already reached.
    bool                isTargetReached(CCControl* control, comparingMode comparing, int controlTarget);
    
    
    
    
    
    /// deviceFlow verbosity
    int                 verbosity;

public:
    
    /// Array of tasks to be performed.
    /// @see CCTask
    CCTask*             task[MAX_TASKS_PER_DEVICEFLOW];
    
    
    
    
    CCDeviceFlow(const String deviceFlowName, const unsigned int deviceFlowID, CCDevice* device, float defaultVelocity, float defaultAcceleration, float defaultDeceleration);
    ~CCDeviceFlow();
  
    

    /// Function sets up device-specific default values for velocity, acceleration and deceleration.
    /// Call this function to be prepared for overloading [addTask(...)](@ref addTask) function.
    /// @param defaultVelocity the device's default velocity.
    /// @param defaultAcceleration the device's default acceleration.
    /// @param defaultDeceleration the device's default deceleration.
    void defineDefaults(float defaultVelocity, float defaultAcceleration, float defaultDeceleration = 0.0);
    
    
    /// Function declares a task to be executed and returns its index.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param target this task's target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration value.
    /// @param deceleration this task's deceleration value.
    /// @return the task index.
    CCTask* addTask(float target, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);
    
    /// Function declares a task to be executed and returns its index.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param relativTarget this task's relativ target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration.
    /// @param deceleration this task's deceleration.
    /// @return the task index.
    CCTask* addTaskMoveRelativ(float relativTarget, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);
    
    /// Function declares a task to be executed and returns its index.
    /// Before the task is executed, a position reset is performed.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param target this task's target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration.
    /// @param deceleration this task's deceleration.
    /// @return the task index.
    CCTask* addTaskWithPositionReset(float target, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);
    
    /// Function declares a task to be executed and returns its index.
    /// After the task is executed, a position reset is performed.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param target this task's target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration.
    /// @param deceleration this task's deceleration.
    /// @return the task index.
    CCTask* addTaskWithPositionResetOnCompletion(float target, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);
    
    /// Function declares a task to be executed and returns its index.
    /// After the task is executed, a position reset is performed.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param relativTarget this task's relativ target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration.
    /// @param deceleration this task's deceleration.
    /// @return the task index.
    CCTask* addTaskMoveRelativWithPositionResetOnCompletion(float relativTarget, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);
    
    /// Function declares a task to be executed and returns its index.
    /// Before the task is executed, a position reset is performed.
    /// It creates an instance of [CCTask](@ref task) and puts it into the task array of the device.
    /// @param relativTarget this task's relativ target.
    /// @param velocity this task's velocity.
    /// @param acceleration this task's acceleration.
    /// @param deceleration this task's deceleration.
    /// @return the task index.
    CCTask* addTaskMoveRelativWithPositionReset(float relativTarget, float velocity = 0.0, float acceleration = 0.0, float deceleration = 0.0);

    
    CCTask* registerTask(float target, float velocity, float acceleration, float deceleration, bool moveRelativ, positionResetMode positionReset);

    
    deviceInfoCode handlePreparation(long taskTime);
    deviceInfoCode handleStartEvent(unsigned long taskTime);
    deviceInfoCode handleStopEvent(unsigned long taskTime);
    
    /// Getter method for getting the id of this deviceFlow
    /// @sa deviceFlowName;
    const unsigned int  getID() const;
    
    /// Getter method for getting the name of this deviceFlow
    /// @sa deviceFlowName;
    const String        getName() const;
    
    /// Getter method for getting the device of this deviceFlow
    /// @sa deviceFlowName;
    CCDevice*           getDevice();
    

    /// Getter method for getting the device's default velocity within this deviceFlow.
    /// @sa defaultVelocity;
    float           getDefaultVelocity();
    
    /// Getter method for getting the device's default acceleration within this deviceFlow.
    /// @sa defaultAcceleration;
    float           getDefaultAcceleration();
    
    /// Getter method for getting the device's default deceleration within this deviceFlow.
    /// @sa defaultDeceleration;
    float           getDefaultDeceleration();
    

    /// Getter method for getting the number of tasks of the device
    /// @sa countOfTasks;
    unsigned int    getCountOfTasks();
    
    /// Setter method for manipulating the number of tasks of the device
    /// @sa countOfTasks;
    void            setCountOfTasks(unsigned int count);
    
    /// Getter method for getting the taskPointer of the device
    /// @sa taskPointer;
    int             getTaskPointer();
    
    /// Setter method for setting the taskPointer of the device
    /// @sa taskPointer;
    void            setTaskPointer(int pointer);
    
    /// Setter method for increasing the taskPointer of the device
    /// @sa taskPointer;
    void            increaseTaskPointer();
    
    
    unsigned int getProposedTaskID();
    void setProposedTaskID(unsigned int proposedTaskID);
    bool getStartProposedTaskAutomatically();
    void setStartProposedTaskAutomatically(bool startProposedTaskAutomatically);
    
    
    
    /// Getter method for getting the startEvent of the device
    /// @sa startEvent, event;
    event               getStartEvent();
    
    /// Setter method for setting the startEvent of the device
    /// @sa startEvent, event;
    void                setStartEvent(event startEvent);
    
    /// Getter method for getting the startTime of the device
    /// @sa startTime;
    unsigned long       getStartTime();
    
    /// Setter method for setting the startTime of the device
    /// @sa startTime;
    void                setStartTime(unsigned long startTime);


    /// Getter method for getting the startControl of the device
    /// @sa startControl;
    CCControl*          getStartControl();
    
    /// Getter method for getting the comparing mode between the target value and the state of startControl
    /// @sa startControlTarget;
    comparingMode       getStartControlComparing();
    
    /// Getter method for getting the target value of startControl
    /// @sa startControlTarget;
    int                 getStartControlTarget();
    
    /// Getter method for getting the stopControl of the device
    /// @sa stopControl;
    CCControl*          getStopControl();
    
    /// Getter method for getting the target value of stopControl
    /// @sa startControlTarget;
    int                 getStopControlTarget();
    
    /// Getter method for getting the comparing mode between the target value and the state of stopControl
    /// @sa startControlTarget;
    comparingMode       getStopControlComparing();
    

    
    /// Getter method for getting the startTriggerDevice of the device
    /// @sa startTriggerDevice;
    CCDevice*           getStartTriggerDevice();
    
    /// Getter method for getting the startTriggerTask of the device
    /// @sa startTriggerTask;
    unsigned int        getStartTriggerTaskID();
    
    /// Getter method for getting the startTriggerPosition of the device
    /// @sa startTriggerPosition;
    signed long         getStartTriggerPosition();
    
    /// Getter method for getting the startDelay of the device
    /// @sa startDelay;
    unsigned long       getStartDelay();
    
    /// Setter method for setting the startDelay of the device
    /// @sa startDelay;
    void                setStartDelay(unsigned long delay);
    

    /// Getter method for getting the stopEvent of the device
    /// @sa stopEvent, event;
    event               getStopEvent();
    
    /// Setter method for setting the stopEvent of the device
    /// @sa stopEvent, event;
    void                setStopEvent(event stopEvent);
    
    /// Getter method for getting the timeOut of the device
    /// @sa timeOut;
    unsigned long       getTimeout();
    
    /// Setter method for setting the timeOut of the device
    /// @sa timeOut;
    void                setTimeout(unsigned long timeout);

    /// Getter method for getting the stopTriggerDevice of the device
    /// @sa stopTriggerDevice;
    CCDevice*           getStopTriggerDevice();
    
    /// Getter method for getting the stopTriggerTask of the device
    /// @sa stopTriggerTask;
    unsigned int        getStopTriggerTaskID();
    
    /// Getter method for getting the stopTriggerPosition of the device
    /// @sa stopTriggerPosition;
    signed long         getStopTriggerPosition();
    


    
    /// Getter method for getting the stopping of the device
    /// @sa stopping, stoppingMode;
    stoppingMode        getStopping();
    
    /// Setter method for setting the stopping of the device
    /// @sa stopping, stoppingMode;
    void                setStopping(stoppingMode mode);
    
    /// Getter method for getting the switching bit of the device
    /// @sa switching;
    switchingMode       getSwitching();
    
    /// Setter method for setting the switching bit of the device
    /// @sa switching;
    void                setSwitching(switchingMode switchPromptly);
    


    bool isStartTimeReached(unsigned long taskTime);
    bool isStopTimeReached(unsigned long taskTime);

    bool                    isStartTargetReached();
    bool                    isStopTargetReached();
    
    
    


    
    
    
    /// Function lists specified task of  registered devices.
    /// A list with all tasks and bare informations are presented.
    void            getTask(unsigned int t);
    
    /// Function lists all tasks of all registered devices.
    /// A list with all tasks and bare informations are presented.
    void            getAllTasks();
    
    

    void            setVerbosity(int verbosity);

};

#endif /* defined(__deviceScheduler__CCDeviceFlow__) */
