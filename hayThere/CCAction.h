//
//  CCAction.h
//  deviceScheduler
//
//  Created by little Ikarus on 10.07.16.
//  Copyright (c) 2016 little Ikarus. All rights reserved.
//

#ifndef __deviceScheduler__CCAction__
#define __deviceScheduler__CCAction__

#include "deviceScheduler.h"
#include "CCTask.h"




class CCDeviceFlow;

class CCAction {
private:
    int verbosity;

    const String              actionName;
    const workflowInfoCode    workflowInfo;

    int                 validTaskID;
    deviceAction        targetAction;
    stoppingMode        stopping;
    switchingMode       switching;
    int                 followingTaskID;
    int                 notificationCode;
    String              notificationText;
    bool                actionDone;


public:
    
    CCDeviceFlow*       targetDeviceFlow;

    CCAction(const String actionName, const workflowInfoCode workflowInfo);
//    ~CCAction();


    void evokeTaskStop(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, stoppingMode stopping);

    void evokeJumpToTask(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, switchingMode switching, CCTask* followingTask);
//    , int notificationCode = 0, String notificationText = "");
    void evokeJumpToNextTask(CCDeviceFlow* targetDeviceFlow, CCTask* validTask, switchingMode switching);
//    , int notificationCode = 0, String notificationText = "");
    
    void evokeBreak(CCDeviceFlow* targetDeviceFlow, CCTask* validTask);
//    , int notificationCode = 0, String notificationText = "");
    void evokeBreak();
//    int notificationCode = 0, String notificationText = "");
    

    const String              getName();
    workflowInfoCode    getWorkflowInfo();

    stoppingMode        getStopping();
    switchingMode       getSwitching();
    int                 getValidTaskID();
    deviceAction        getTargetAction();
    int                 getFollowingTaskID();
    int                 getNotificationCode();
    String              getNotificationText();
    bool                getActionDone();
    void                setActionDone(bool d);
    
    void setVerbosity(int verbosity);

    
};

#endif /* defined(__deviceScheduler__CCAction__) */
