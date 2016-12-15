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


#include "Arduino.h"

// Include application, user and local libraries
#include "hayThere.h"

// Prototypes


// Define variables and constants


//#include <malloc.h>




#include "hayThere.h"
#include "CCDeviceScheduler.h"





// ------------- globals -----------------------------------------------------------------------------------------------------

/// The Scheduler and it's device array

CCDeviceScheduler   *scheduler;






// ------------- prototypes --------------------------------------------------------------------------------------------------
void setup();



// ------------- main loop --------------------------------------------------------------------------------------------------
void loop() {
    
    Serial.println(), Serial.println(), Serial.println();
    for (int i = 0; i < 56; i++) Serial.print(F("#"));
    Serial.println(), Serial.println(), Serial.println();
    
    
        
    // ================================================================================================================================
    // ================================================================================================================================
    // ============= create scheduler object: =========================================================================================
    // ================================================================================================================================
    // ================================================================================================================================
    
    Serial.print(F("create scheduler: "));
    
    scheduler = new CCDeviceScheduler("scheduler");
    scheduler->setVerbosity(SHOW_TASK_VIEW | BASICOUTPUT);
    
    Serial.println(scheduler->getName());
    
    
    //      
    // ================================================================================================================================
    // ================================================================================================================================
    // ============= register devices: ================================================================================================
    // ================================================================================================================================
    // ================================================================================================================================
    
    Serial.print(F("register devices of scheduler "));
    Serial.println(scheduler->getName());
    
    
    
    
    CCDevice* turnStepper = scheduler->addStepper_TMC2130(STEPPER_TURN_NAME,
                                                        STEPPER_TURN_STEP_PIN,
                                                        STEPPER_TURN_DIR_PIN,
                                                        STEPPER_TURN_ENABLE_PIN,
                                                        STEPPER_TURN_STEPS_PER_ROTATION,
                                                        STEPPER_TURN_CHIPSELECT_PIN,
                                                        STEPPER_TURN_CURRENT);

    
    CCDevice* liftStepper = scheduler->addStepper_TMC2130(STEPPER_LIFT_NAME,
                                                         STEPPER_LIFT_STEP_PIN,
                                                         STEPPER_LIFT_DIR_PIN,
                                                         STEPPER_LIFT_ENABLE_PIN,
                                                         STEPPER_LIFT_STEPS_PER_ROTATION,
                                                         STEPPER_LIFT_CHIPSELECT_PIN,
                                                         STEPPER_LIFT_CURRENT);
    
    
    CCControl* liftRepeatControl = scheduler->addControlEvent("liftRepeatControl");
    CCControl* turnRepeatControl = scheduler->addControlEvent("turnRepeatControl");
    
    // ================================================================================================================================
    //=================================================================================================================================
    // ============= register sensors: ================================================================================================
    // ================================================================================================================================
    // ================================================================================================================================
    
    Serial.print(F("register controls of scheduler "));
    Serial.println(scheduler->getName());

    
    
    scheduler->listDevices();
    scheduler->listControls();
    
    
    
    // =================================================================================================================================================
    // =================================================================================================================================================
    // ============= register workflows ===========================================================================================================
    // =================================================================================================================================================
    // =================================================================================================================================================
    
    Serial.print(F("register workflows of scheduler "));
    Serial.println(scheduler->getName());
    
    
    // =================================================================================================================================================
    // ============= initialisation workflow ===========================================================================================================
    // =================================================================================================================================================
    
    CCWorkflow* initTheMachine = new CCWorkflow("initMachine");
    
    
    {
        // ============= devices of initMachine =======================================================================================
        
        CCDeviceFlow* liftStepperFlow = initTheMachine->addDeviceFlow("liftStepperFlow", liftStepper, 1000, 100);
        CCDeviceFlow* turnStepperFlow = initTheMachine->addDeviceFlow("turnStepperFlow", turnStepper, 1000, 400);
        

        
        // ============= tasks of initMachine =========================================================================================
        
        //  drive head (left) to parking position
        CCTask* liftLeft;
        liftLeft = liftStepperFlow->addTask(3600);
        liftLeft->startByDate(10);
        
        //  drive head (right) to parking position
        CCTask* turnLeft;
        turnLeft = turnStepperFlow->addTask(3600);
        turnLeft->startByDate(20);
        
         CCTask* liftRight;
        liftRight = liftStepperFlow->addTask(0);
        liftRight->startAfterCompletion();
        
        CCTask* turnRight;
        turnRight = turnStepperFlow->addTask(0);
        turnRight->startAfterCompletion();
        
        
//        liftRepeatControl->setTarget(liftStepperFlow);
//        CCFlowControl* repeatLiftingControl = initTheMachine->addFlowControl("repeatLiftingControl", liftRepeatControl, IS_GREATER_THEN, liftRight);
//        CCAction* liftRepeatAction;
//        liftRepeatAction = repeatLiftingControl->addAction("liftRepeatAction", EVERYTHING_OK);
//        liftRepeatAction->evokeJumpToTask(liftStepperFlow, NULL, SWITCH_AFTER_COMPLETION, liftLeft);
        
        turnRepeatControl->setTarget(turnStepperFlow);
        CCFlowControl* repeatTurningControl = initTheMachine->addFlowControl("repeatTurningControl", turnRepeatControl, IS_GREATER_THEN, turnRight);
        CCAction* turnRepeatAction;
        turnRepeatAction = repeatTurningControl->addAction("turnRepeatAction", EVERYTHING_OK);
        turnRepeatAction->evokeJumpToTask(turnStepperFlow, NULL, SWITCH_AFTER_COMPLETION, turnLeft);
        
        scheduler->reviewTasks(initTheMachine);
        scheduler->listAllTasks(initTheMachine);
        scheduler->listAllActions(initTheMachine);
        
        
    }

    
    
    // ============================================================================================================================
    // ============================================================================================================================
    // ============= operation: ===================================================================================================
    // ============================================================================================================================
    // ============================================================================================================================
    
    
    Serial.println(F("initialisation complete!"));
    
    
    bool initNeeded = false;
    long blinkTimer = millis();
    
    while (!initNeeded) {
        
        scheduler->run(initTheMachine);
        
        // ============= or just blink =======================================================================================================
        
        if (millis() - blinkTimer > 2000) {
            if (millis() - blinkTimer > 2200) {
                blinkTimer = millis();
                Serial.print(F("."));
                
            }
        }
    }
    
    
    Serial.println(F("exectuion is terminated!"));
    
    delay(2000);
    
        
    Serial.println();
    Serial.print(F("delete all objects ... "));
    
    delete initTheMachine;
    initTheMachine = NULL;
    
    
    delete scheduler;
    scheduler = NULL;
    
    Serial.println(F("done!"));
    
        
    delay(2000);
    
}


void setup() {
    // ============= prepare output =======================================================================================================
    
    Serial.begin(115200);
    Serial.println(), Serial.println(), Serial.println();
    Serial.println(), Serial.println(), Serial.println();

}



    
    