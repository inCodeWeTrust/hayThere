//
//  CCDeviceScheduler.h
//  deviceScheduler
//
//  Created by Little Abakus on 09.06.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCDeviceScheduler__
#define __deviceScheduler__CCDeviceScheduler__



#include "deviceScheduler.h"
#include "hayThere.h"

#include "CCWorkflow.h"

#include "CCDevice.h"
#include "CCStepperDevice.h"
#include "CCStepperDevice_TMC2130.h"
#include "CCDcControllerDevice.h"

#include "CCControl.h"
#include "CCControlButton.h"
#include "CCControlSensor.h"
#include "CCControlEvent.h"



///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCDeviceScheduler
///
/// @brief Scheduler engine to manage all devices and their tasks.
///
/// The scheduler holds all devices and controls them by calling their startTask(), operateTask(), stopTask() etc. routines.
///
///////////////////////////////////////////////////////////////////////////////////////////////////



// min time for deviceLoop: 15, max time for deviceLoop: 18462
// min time for controls: 1, max time for controls: 12
// min time for flowControls: 1, max time for flowControls: 11

// min time for deviceLoop: 14, max time for deviceLoop: 18459
// min time for controls: 1, max time for controls: 14
// min time for flowControls: 1, max time for flowControls: 12



class CCDeviceScheduler {
    int verbosity;

    
    /// The Scheduler's name
    /// Value, set by the user to be recognized by him.
    const String   schedulerName;
    
    /// Scheduler parameter:
    /// Value holds the number of devices managed by him.
    unsigned int countOfDevices, countOfControls;
    
    unsigned long taskTime;
    
    unsigned int nextTaskID[MAX_DEVICES_PER_SCHEDULER];

    CCDeviceFlow* currentDeviceFlow;
    CCDeviceFlow* triggerDeviceFlow_start;
    CCDeviceFlow* triggerDeviceFlow_stop;
    CCTask* currentTask;
    CCDevice* currentDevice;
    CCDevice* triggerDevice_start;
    CCDevice* triggerDevice_stop;
    
    CCControl* currentButton;
    CCAction* currentAction;
    CCFlowControl* currentFlowControl;
    
    int notificationCode;
    String  notificationText;
    
    
    
    String getLiteralOfDeviceType(deviceType t);
    String getLiteralOfControllerType(controlType t);
    String getLiteralOfTaskEvent(event e);
    String getLiteralOfState(deviceState s);
    String getLiteralOfControlValueComparing(comparingMode c);
    String getLiteralOfBooleanState(bool s);
    String getLiteralOfStoppingMode(stoppingMode s);
    String getLiteralOfSwitchingMode(switchingMode s);
    String getLiteralOfDeviceAction(deviceAction d);
    String getLiteralOfWorkflowInfo(workflowInfoCode i);
    String getLiteralOfDeviceInfo(deviceInfoCode i);
    

public:
    
    
    CCDeviceScheduler(const String schedulerName);
    ~CCDeviceScheduler();
    
    const String getName();
    

    /// The Scheduler's device array.
    /// Array of all devices.
    CCDevice *device[MAX_DEVICES_PER_SCHEDULER];
    
    
    /// Function adds a stepper device to the device array, that is driven by da driver like the TMC2130, and returns the index of the device.
    /// Device-specific parameters are passed.
    /// @param deviceName the human-readable name of the device (used for verbose output).
    /// @param dir_pin the pin number of the stepper driver's direction pin.
    /// @param step_pin the pin number of the stepper driver's step pin.
    /// @param enable_pin the pin number of the stepper driver's enable pin.
    /// @param chipSelect_pin the number of the drivers chip select pin (CS) for the SPI communication.
    /// @param currentMax the maximum current, that is applied to the motor coils in mA RMS.
    /// @param stepsPerRotation the number of steps needed to make a full rotation.
    /// @return the device index.
    CCDevice* addStepper_TMC2130(String deviceName, unsigned int step_pin, unsigned int dir_pin, unsigned int enable_pin, unsigned int stepsPerRotation, unsigned int chipSelect_pin, unsigned int currentMax);
    
    
    
    /// Function adds a switching device to the device array and returns the index of the device.
    /// A switching device is a device, that is simply switched on or off. Device-specific parameters are passed.
    /// @param deviceName the human-readable name of the device (used for verbose output).
    /// @param switching_pin the pin number of the device's controll pin.
    /// @param switchingPin_active the state of the switching pin, where the device is active.
    /// @return the device index.
    CCDevice* addDcController(String deviceName, unsigned int switching_pin, bool switchingPin_active);


    
    /// Function lists all registered devices.
    /// A list with all devices and bare informations are presented.
    void listDevices();
    void printMessage(unsigned long taskTime, CCDeviceFlow* df, schedulersJob job);

    
    /// Array of all control-inputs.
    CCControl *control[MAX_CONTROLS_PER_SCHEDULER];
    
    /// Function adds a control button to the control button array and returns the index of the button.
    /// A control button is a input device, that can provide either a HIGH or a LOW level at a input pin or simply connect the pin and GND using the internal inputPullup-function. Specific parameters are passed.
    /// @param buttonName the human-readable name of the device (used for verbose output).
    /// @param button_pin the pin number of the button's pin.
    /// @param buttonActive the state of the pin, where the button should trigger actions.
    /// @param the pinMode macro is passed (INPUT or INPUT_PULLUP).
    /// @return the button index.
    CCControl* addControlButton(String controlName, unsigned int pin, unsigned int mode = INPUT);
 
    
    CCControl* addControlSensor(String sensorName, unsigned int pin);
    
    CCControl* addControlEvent(String eventName);
    
    /// Function lists all registered control buttons.
    /// A list with all buttons and bare informations are presented.
    void listControls();
    
    void evaluateButtons();


    void listAllTasks(CCWorkflow* workflow);
    void listAllActions(CCWorkflow* workflow);
    
    
    /// Function calls the <reviewValues(CCTask* nextTask)> function of all devices.
    /// This causes the devices to approve, if all tasks are possible and do some basic calculations.
    /// This function needs to be called before the run loop is started.
    void reviewTasks(CCWorkflow* currentWorkflow);
    
    
    /// Function starts the scheduler, all registered tasks are driven as specified.
    /// @image html scheduler__run.png
    int run(CCWorkflow* currentWorkflow);
    
    void setVerbosity(int verbosity);

};




#endif // defined(__deviceScheduler__CCDeviceScheduler__)


