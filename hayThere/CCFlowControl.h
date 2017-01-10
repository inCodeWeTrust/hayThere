//
//  CCFlowControl.h
//  deviceScheduler
//
//  Created by little Ikarus on 12.07.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCFlowControl__
#define __deviceScheduler__CCFlowControl__

#include "CCControlButton.h"
#include "CCControlSensor.h"
#include "CCAction.h"
#include "deviceScheduler.h"




class CCDeviceFlow;

class CCFlowControl {
    
    const String              controlName;
    const CCControl*          control;
    const comparingMode       comparing;
    const int                 target;
    
    unsigned int        countOfActions;

    CCAction*           action[MAX_ACTIONS_PER_FLOWCONTROL];
    
    int                 verbosity;
    
public:
    
    CCFlowControl(const String controlName, const CCControl* control, const comparingMode comparing, const int target);
    ~CCFlowControl();
    
    
    CCAction*           addAction(String actionName, workflowInfoCode workflowInfo);

    bool                needsToFire();
    
    
    const String        getName() const;
    CCControl*          getControl() const;
    comparingMode       getComparing() const;
    int                 getTarget() const;

    CCAction*           getAction(int a);
    unsigned int        getCountOfActions();
    
    void                setVerbosity(int verbosity);

};


#endif /* defined(__deviceScheduler__CCFlowControl__) */
