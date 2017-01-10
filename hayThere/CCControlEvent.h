//
//  CCControlEvent.h
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCControlEvent__
#define __deviceScheduler__CCControlEvent__



#include "CCControl.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCControlEvent
///
/// @brief Class for events to control the workflow
///////////////////////////////////////////////////////////////////////////////////////////////////





class CCControlEvent : public CCControl {

public:
    
 
    
    void        read();
    
    
    CCControlEvent(const String eventName, const unsigned int eventIndex);
    ~CCControlEvent();
    
    
    void        setTarget(CCDeviceFlow* targetDeviceFlow);
//    CCDeviceFlow* getTargetDeviceFlow();
//    void setTargetDeviceFlow(CCDeviceFlow* targetDeviceFlow);
//    int getTargetTaskID();
//    void setTargetTaskID(int targetTaskID);
};

#endif // defined(__deviceScheduler__CCControlEvent__)

