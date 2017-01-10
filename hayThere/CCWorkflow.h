//
//  CCWorkflow.h
//  deviceScheduler
//
//  Created by little Ikarus on 16.06.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCWorkflow__
#define __deviceScheduler__CCWorkflow__


#include "deviceScheduler.h"
#include "CCDeviceFlow.h"
#include "CCFlowControl.h"
#include "CCControl.h"



///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCStepperDevice
///
/// @brief Device class for stepper motor devices
///
///////////////////////////////////////////////////////////////////////////////////////////////////




class CCWorkflow {
private:
    const String        workflowName;
    
    /// Parameter of the workflow.
    /// Value holds the number of deviceFlows to be executed within this workflow.
    unsigned int        countOfDeviceFlows;

    /// Parameter of the workflow.
    /// Value holds the number of flowControls to be considered within this workflow.
    unsigned int        countOfControls;

    /// Parameter of the workflow.
    /// Value holds the number of flowControls to be considered within this workflow.
    unsigned int        countOfFlowControls;
    

    workflowInfoCode    workflowInfo;

    
    int verbosity;
    
public:
    CCDeviceFlow*       deviceFlow[MAX_DEVICEFLOWS_PER_WORKFLOW];
    CCControl*          control[MAX_CONTROLS_PER_WORKFLOW];
    CCFlowControl*      flowControl[MAX_FLOWCONTROLS_PER_WORKFLOW];

    
    CCWorkflow(const String name);
    ~CCWorkflow();
    
    /// Function registers a task-flow for a device to be executed within this workflow.
    /// It creates an instance of [CCDeviceFlow](@ref deviceFlow) and puts it into the deviceFlow array of the workflow.
    /// @param deviceFlowName the name of this deviceFlow.
    /// @param device a pointer to the related device.
    /// @param defaultVelocity the device's default velocity within this workflow.
    /// @param defaultAcceleration the device's default acceleration within this workflow.
    /// @param defaultDeceleration the device's default deceleration within this workflow.
    /// @return the deviceFlow instance.
    CCDeviceFlow* addDeviceFlow(String deviceFlowName, CCDevice* device, float defaultVelocity = 0.0, float defaultAcceleration = 0.0, float defaultDeceleration = 0.0);
    
    /// Function registers a flow-control to be considered within this workflow.
    /// It creates an instance of [CCFlowControl](@ref flowControl) and puts it into the flowControl array of the workflow.
    /// @param controlName the name of this flowControl.
    /// @param control a pointer to the related button.
    /// @return the flowControl instance.
    CCControl* addControl(CCControl* control);

    /// Function registers a flow-control to be considered within this workflow.
    /// It creates an instance of [CCFlowControl](@ref flowControl) and puts it into the flowControl array of the workflow.
    /// @param controlName the name of this flowControl.
    /// @param control a pointer to the related button.
    /// @return the flowControl instance.
    CCFlowControl* addFlowControl(String flowControlName, CCControl* control, comparingMode comparing, int target);

    CCFlowControl* addFlowControl(String flowControlName, CCControl* control, comparingMode comparing, CCTask* targetTask);

    const String getName() const;
    unsigned int getCountOfDeviceFlows();
    unsigned int getCountOfControls();
    unsigned int getCountOfFlowControls();
    workflowInfoCode getWorkflowInfo();
    void setWorkflowInfo(workflowInfoCode c);
    
    void setVerbosity(int verbosity);

};











#endif /* defined(__deviceScheduler__CCWorkflow__) */
