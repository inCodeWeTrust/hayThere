/// 
/// @mainpage	hayThere 
///
/// @details	Description of the project
/// @n 		
/// @n 
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Little Abakus
/// @author		___FULLUSERNAME___
/// @date		14.12.16 20:56
/// @version	<#version#>
/// 
/// @copyright	(c) Little Abakus, 2016
/// @copyright	GNU General Public License
///
/// @see		ReadMe.txt for references
///


///
/// @file		hayThere.ino
/// @brief		Main sketch
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Little Abakus
/// @author		___FULLUSERNAME___
/// @date		14.12.16 20:56
/// @version	<#version#>
/// 
/// @copyright	(c) Little Abakus, 2016
/// @copyright	GNU General Public License
///
/// @see		ReadMe.txt for references
/// @n
///


// Core library for code-sense
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad MSP430 G2 and F5529, Stellaris and Tiva, Experimeter Board FR5739 specific
#include "Energia.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif

// Include application, user and local libraries


// Prototypes


// Define variables and constants


// Add setup code 
void setup() 
{

}

// Add loop code 
void loop() 
{

}
