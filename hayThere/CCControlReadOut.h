//
//  CCControlReadOut.h
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCControlReadOut__
#define __deviceScheduler__CCControlReadOut__



#include "CCControl.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCControlReadOut
///
/// @brief Class for buttons to control the workflow
///////////////////////////////////////////////////////////////////////////////////////////////////





class CCControlReadOut : public CCControl {

public:
    
    
    
    void        read();
    
    
    CCControlReadOut(const String controlName, const unsigned int controlIndex, const unsigned int pin);
    ~CCControlReadOut();
    
    
    void        setTarget(CCDeviceFlow* targetDeviceFlow);

};

#endif // defined(__deviceScheduler__CCControlReadOut__)

