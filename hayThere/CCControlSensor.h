//
//  CCControlSensor.h
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCControlSensor__
#define __deviceScheduler__CCControlSensor__



#include "CCControl.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCControlSensor
///
/// @brief Class for buttons to control the workflow
///////////////////////////////////////////////////////////////////////////////////////////////////




class CCDeviceFlow;

class CCControlSensor : public CCControl {
public:
    
    
    
    void        read();
    
    
    CCControlSensor(const String controlName, const unsigned int controlIndex, const unsigned int pin);
    ~CCControlSensor();
    
    
};

#endif // defined(__deviceScheduler__CCControlSensor__)

