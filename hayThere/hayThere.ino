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


//#define ARDUINO_SIMULATION


#ifdef ARDUINO_SIMULATION
#include "SIMPatchRunner.h"
#else
#include <Arduino.h>
#include <malloc.h>
#endif




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

    turnStepper->setCurrent(20);
    
    CCDevice* liftStepper = scheduler->addStepper_TMC2130(STEPPER_LIFT_NAME,
                                                         STEPPER_LIFT_STEP_PIN,
                                                         STEPPER_LIFT_DIR_PIN,
                                                         STEPPER_LIFT_ENABLE_PIN,
                                                         STEPPER_LIFT_STEPS_PER_ROTATION,
                                                         STEPPER_LIFT_CHIPSELECT_PIN,
                                                         STEPPER_LIFT_CURRENT);
    
    liftStepper->setCurrent(31);
    
    
    CCDevice* fan = scheduler->addDcController(FAN_NAME,
                                               FAN_PIN,
                                               FAN_ACTIV);
    
    
//
//    
//    CCControl* liftRepeatControl = scheduler->addControlEvent("liftRepeatControl");
//    CCControl* turnRepeatControl = scheduler->addControlEvent("turnRepeatControl");
    
    // ================================================================================================================================
    //=================================================================================================================================
    // ============= register sensors: ================================================================================================
    // ================================================================================================================================
    // ================================================================================================================================
    
    Serial.print(F("register controls of scheduler "));
    Serial.println(scheduler->getName());

    
    CCControl* operationButton = scheduler->addControlButton(OPERATION_BUTTON_NAME,
                                                             OPERATION_BUTTON_PIN,
                                                             OPERATION_BUTTON_MODE);
    
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
        
        CCDeviceFlow* liftStepperFlow = initTheMachine->addDeviceFlow("liftStepperFlow", liftStepper, 160, 10);
        CCDeviceFlow* turnStepperFlow = initTheMachine->addDeviceFlow("turnStepperFlow", turnStepper, 4, 1);
        CCDeviceFlow* fanFlow = initTheMachine->addDeviceFlow("fanFlow", fan, 10, 1000, 1000);
        
        CCControl* operationControl = initTheMachine->addControl(operationButton);
        CCFlowControl* operationFlowControl = initTheMachine->addFlowControl("operationFlowControl", operationControl, IS_NOT, OPERATION_BUTTON_ON);

        CCControl* overtemperatureReadOut = scheduler->addControlReadOut("TMC2130-readOut", liftStepperFlow, OVERTEMPERATURE_WARNING);
        CCControl* overTemperatureControl = initTheMachine->addControl(overtemperatureReadOut);
        CCFlowControl* overTemperatureFlowControl = initTheMachine->addFlowControl("overTemperatureFlowControl", overTemperatureControl, IS, true);
        
        // ============= tasks of initMachine =========================================================================================
        
        //  switch fan on
        CCTask* runFan;
        runFan = fanFlow->addTask(1.0);
        runFan->startByDate(5);
        
        //  drive head (left) to parking position
        CCTask* liftLeft;
        liftLeft = liftStepperFlow->addTaskWithPositionReset(3000);
        liftLeft->startByDate(10);
        
         CCTask* liftRight;
        liftRight = liftStepperFlow->addTask(0);
        liftRight->startAfterCompletion();

        
        //  drive head (right) to parking position
        CCTask* turnLeft;
        turnLeft = turnStepperFlow->addTaskWithPositionReset(120);
        turnLeft->startByDate(20);
        
        CCTask* turnRight;
        turnRight = turnStepperFlow->addTask(0);
        turnRight->startAfterCompletion();
        
        runFan->stopAfterCompletionOf(turnStepper, turnRight, STOP_NORMAL);
        
        CCAction* stopOperation = operationFlowControl->addAction("stopOperationAction", WORKFLOW_STOPPED);
        stopOperation->evokeBreak();
        
        
//        liftRepeatControl->setTarget(liftStepperFlow);
//        CCFlowControl* repeatLiftingControl = initTheMachine->addFlowControl("repeatLiftingControl", liftRepeatControl, IS_GREATER_THEN, liftRight);
//        CCAction* liftRepeatAction;
//        liftRepeatAction = repeatLiftingControl->addAction("liftRepeatAction", EVERYTHING_OK);
//        liftRepeatAction->evokeJumpToTask(liftStepperFlow, NULL, SWITCH_AFTER_COMPLETION, liftLeft);
        
//        turnRepeatControl->setTarget(turnStepperFlow);
//        CCFlowControl* repeatTurningControl = initTheMachine->addFlowControl("repeatTurningControl", turnRepeatControl, IS_GREATER_THEN, turnLeft);
//        CCAction* turnRepeatAction_01;
//        turnRepeatAction_01 = repeatTurningControl->addAction("turnRepeatAction", EVERYTHING_OK);
//        turnRepeatAction_01->evokeJumpToTask(turnStepperFlow, turnRight, SWITCH_AFTER_COMPLETION, turnLeft);

//        CCAction* turnRepeatAction_02;
//        turnRepeatAction_02 = repeatTurningControl->addAction("turnRepeatAction", EVERYTHING_OK);
//        turnRepeatAction_02->evokeTaskStart(turnStepperFlow, turnLeft);
        
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
        if (digitalRead(OPERATION_BUTTON_PIN) == OPERATION_BUTTON_ON) {
            scheduler->run(initTheMachine);
        }
        
        delay(500);
        
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



    
    