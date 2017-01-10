//
//  CCControlButton.h
//  deviceScheduler
//
//  Created by Little Abakus on 01.04.16.
//  Copyright (c) 2016 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCControlButton__
#define __deviceScheduler__CCControlButton__



#include "CCControl.h"




///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCControlButton
///
/// @brief Class for buttons to control the workflow
///////////////////////////////////////////////////////////////////////////////////////////////////





class CCControlButton : public CCControl {

public:
    
 
    
    void        read();
    
    
    CCControlButton(const String buttonName, const unsigned int buttonIndex, const unsigned int button_pin, const int mode);
    ~CCControlButton();
    

};

#endif // defined(__deviceScheduler__CCControlButton__)

