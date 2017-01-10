//
//  deviceScheduler.h
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__deviceScheduler__
#define __deviceScheduler__deviceScheduler__


//#define ARDUINO_SIMULATION

#ifdef ARDUINO_SIMULATION
#include "SIMPatchRunner.h"
static serialViewer Serial;
#else
#include <Arduino.h>
#include <avr/pgmspace.h>
#endif




//  ################## verbosity: #############################################################################

#define NO_OUTPUT                   0x00
#define BASICOUTPUT                 0x01
#define MEMORYDEBUG                 0x02
#define CALCULATIONDEBUG            0x04
#define MOVEMENTDEBUG               0x08

//  deviceScheduler:
#define SHOW_TASK_VIEW              0x10

//  stepper tmc260:
#define TMC260_SPIDEBUG             0x10
#define TMC260_SETUPDEBUG           0x20

//  stepper TMC2130:
#define TMC2130_SPIDEBUG             0x10
#define TMC2130_SETUPDEBUG           0x20



//  ################## memory management: #############################################################################

#define MAX_DEVICES_PER_SCHEDULER           14
#define MAX_CONTROLS_PER_SCHEDULER          14

#define MAX_DEVICEFLOWS_PER_WORKFLOW        10
#define MAX_CONTROLS_PER_WORKFLOW           10
#define MAX_FLOWCONTROLS_PER_WORKFLOW       10

#define MAX_TASKS_PER_DEVICEFLOW            10

#define MAX_ACTIONS_PER_FLOWCONTROL         10


//  ################## PROGRAMMATICAL DEFINITIONS AND CONSTANTS #############################################################################

//typedef signed int schedulerDevice, unsigned int, schedulerWorkflow, schedulerDeviceflow, unsigned int;

//  device type
enum deviceType {
    SERVODEVICE,
    STEPPERDEVICE,
    DCCONTROLLERDEVICE
};

//  control type
enum controlType {
    BUTTON,
    SENSOR,
    EVENT,
    READOUT
};

// schedulers job
enum schedulersJob {
    TRIGGER_TASK_PREPARATION,
    TRIGGER_TASK_START,
    TRIGGER_TASK_STOP,
    NOTICE_TASK_STOP,
    NOTICE_ALL_TASKS_FINISHED
};

// start/stopEvent:
enum event {
    NONE,
    DATE,
    CONTROL,
    FOLLOW,
    FOLLOW_ME,
    POSITION,
    MY_POSITION,
};


//  device state:
enum deviceState {
    SLEEPING,
    MOVING,
    MOVE_DONE,
    PENDING_MOVES
};


//  device action:
enum deviceAction {
    BREAK_LOOP_ACTION = -1,
    DO_NOTHING = 0,
    TASK_START_ACTION,
    TASK_SWITCH_ACTION,
    TASK_STOP_ACTION
};

//  stop mode:
enum stoppingMode {
    STOP_IMMEDIATELY,
    STOP_NORMAL,
    STOP_DYNAMIC
};
//  switch mode:
enum switchingMode {
    NO_SWITCHING,
    SWITCH_AFTER_STOP,
    SWITCH_AFTER_COMPLETION,
    SWITCH_PROMPTLY
};

//  position reset mode:
enum positionResetMode {
    NO_RESET,
    RESET_ON_START,
    RESET_ON_COMPLETION
};

//  mode of comparing sensor values
enum comparingMode {
    IS,
    IS_NOT,
    IS_GREATER_THEN,
    IS_SMALLER_THEN
};

//  notification code:
enum notificationCode {
    BREAK_ON_ENDBUTTON_REACHED = -0x22,
    CONTINUE_ON_ENDBUTTON_REACHED = 0x22
};

//  workflow info code:
enum workflowInfoCode {
    WORKFLOW_CANCELED_ON_ERROR = -0x66,
    WORKFLOW_CANCELED_ON_ENDBUTTON_REACHED = -0x42,
    WORKFLOW_CANCELED_ON_BUTTON_NOT_REACHED = -0x41,
    WORKFLOW_CANCELED_ON_PARAMETER_ERROR = -0x40,
    WORKFLOW_CANCELED_ON_FLOWCONTROL_ERROR = -0x39,
    WORKFLOW_CANCELED = -0x38,
    WORKFLOW_DISABLED_ON_ERROR = -0x36,
    WORKFLOW_DISABLED_ON_ENDBUTTON_REACHED = -0x22,
    WORKFLOW_DISABLED_ON_BUTTON_NOT_REACHED = -0x21,
    EVERYTHING_OK = 0x00,
    WORKFLOW_STOPPED = 0x02,
    SONGCANCELBUTTON_PRESSED = 0x10,
    SONGENDBUTTON_PRESSED = 0x11,
    STOCKBOTTOM_BUTTON_REACHED = 0x12,
    STOCKTOP_BUTTON_REACHED = 0x13,
    BUTTON_NOT_REACHED = 0x21,
    ENDBUTTON_REACHED = 0x22,
};
//  device info code:
enum deviceInfoCode {
    DEVICE_PARAMETER_ERROR = -0x44,
    DISPOSE_THIS_TASK = -0x08,
    DELAY_THIS_TASK = -0x06,
    DEVICE_OK = 0x00,
    TASK_PREPARATION_OK = 0x04,
    TASK_START_OK = 0x05,
    TASK_DISPOSED = 0x06,
    TASK_DELAYED = 0x07,
    TASK_STOP_OK = 0x08,
    ALL_TASKS_DONE = 0x0F
};

//  appriximation mode:
enum approximationMode {
    SKIP_APPROXIMATION_IMMEDIATELY = 0x00,
    SKIP_APPROXIMATION_VERY_FAST = 0x10,
    SKIP_APPROXIMATION_FAST = 0x20,
    SKIP_APPROXIMATION_MODERATE = 0x30,
    SKIP_APPROXIMATION_PRECISE = 0x40,
    SKIP_APPROXIMATION_VERY_PRECISE = 0x60,
    SKIP_APPROXIMATION_NEVER = 0xFF
};

//  motorDriverStatusInfo:
enum motorDriverStatusInfo {
    STANDSTILL,
    OPENLOAD,
    SHORT_TO_GROUND,
    OVERTEMPERATURE_WARNING,
    OVERTEMPERATURE_SHUTDOWN,
    MOTORSTALL,
    MOTOR_CURRENT,
    STALLGUARD_VALUE
};


#define STEPPINGPERIOD_TO_KICK_UP           150        //150
#define STEPPINGPERIOD_TO_KICK_DOWN         300        //300



#define I_AM_LATE_LED                       2




#endif // defined(__deviceScheduler__deviceScheduler__)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @page coding Coding Tutorial
/// @section createScheduler Creating a new scheduler
/// Zentrales Objekt ist der Scheduler, eine Instanz der Klasse mit dem Namen CCDeviceScheduler. Er steuert und koordiniert einen Arbeitsablauf, d.h. verschiedene Geräte führen einen Block von Aufgaben aus. Es können beliebig viele dieser Arbeitsabläufe definiert und in beliebiger Reihenfolge abgerufen werden. Zum Beispiel beim Plattenschneideautomat @c fetchNewRecord, @c cuttingProcess, @c ejectRecord etc.
/// @code {.cpp}
/// CCDeviceScheduler *cuttingProcess = new CCDeviceScheduler("cutting");
/// @endcode
/// Ein neues Scheduler-Objekt mit dem Namen @c cuttingProcess wird erzeugt. Im logging erscheint ess mit dem Namen @c cutting. Es soll z.B. die komplette Steuerung des Schneideprozesses übernehmen.
/// <br><br><br>
/// @section registerDevice Registering Devices
/// Für die Geräte existiert eine Basisklasse CCDevice. Der Scheduler enthält ein Array dieser Basisklasse mit dem namen @c device, in das alle Geräte eingetragen werden. Es können beliebig viele Geräte zum Scheduler hinzugefügt, d.h. in dieses Array eingetragen werden. Dies geschieht durch die Funktionen @c addServo(...), @c addStepper(...), @c addDcController(...) etc. des Schedulers, in der sämtliche Parameter dieses Geräts übergeben werden:
/// @code{.cpp}
/// cuttingProcess->addServo("lift", 12, 800, 2100, 1800);
/// @endcode
/// Ein neues Device-Objekt vom Typ Modellbauservo wird dem Device-Array des Schedulers hinzugefügt. Übergeben wird ein Name, mit dem dieses Servo im Ablaufprotokoll erscheint, hier @c lift, der Pin, mit dem es gesteuert wird, ein minimaler und ein maximaler Positionswert und die Parkposition.
/// Rückgabewert dieser Funktion ist der Index dieses Gerätes im Geräte-Array @c device des Schedulers. Es ist ein ganzzahliger Wert, sein Typname ist @c schedulerDevice. Mit diesem Geräteindex ist das Gerät dann eindeutig identifizierbar.
/// Zur besseren Übersichtlichkeit können die Parameter freilich als Makros in einem Headerfile notiert werden.
/// Die Registrierung eines Geräts kann dann zB. so aussehen:
/// @code{.cpp}
/// cuttingProcess->addServo(SERVO_NAME, SERVO_PIN, SERVO_MIN_POSITION, SERVO_MAX_POSITION, SERVO_PARK_POSITION);
/// @endcode
/// Wird dieser Wert in einer aussagekräftigen Variable gespeichert, ist es sehr einfach, dieses Gerät anzusprechen:
/// @code{.cpp}
/// schedulerDevice liftServo = cuttingProcess->addServo(LIFTSERVO_NAME, LIFTSERVO_PIN, LIFTSERVO_MIN_POSITION, LIFTSERVO_MAX_POSITION, LIFTSERVO_PARK_POSITION);
///
/// cuttingProcess->device[liftServo]...
/// @endcode
/// Die implementierten Methoden sind:
/// - @code addServo(...) @endcode
/// - @code addServoWithCounterServo(...) @endcode
/// - @code addDcController(...) @endcode
/// - @code addStepper_A4988(...) @endcode
/// - @code addStepper_TMC2130(...) @endcode
/// <br><br>
/// @section registerTask Registering Tasks
/// Zu diesen Geräten des Schedulers können nun beliebig viele Aufgaben hinzugefügt werden. Für die Aufgaben existiert eine Klasse mit dem Namen CCTask. Die Klasse CCDevice enthält ein Aufgaben-Array mit dem Namen @c task. Die Eintragung von Aufgaben in dieses Array bewerkstelligt die Funktion @c addTask(...) des jeweiligen Gerätes: Übergeben werden sämtliche Parameter dieser Aufgabe:
/// @code{.cpp}
/// unsigned int lowerHeadForCutting = cuttingProcess->device[liftServo]->addTask(CUTING_POSITION, SPEED_SLOW, ACCELERATION_NORMAL, DECELERATION_SLOW);
/// @endcode
/// Dem Gerät @c liftServo wird eine neue Aufgabe hinzugefügt: Fahre zur Position @c CUTTING_POSITION mit der Geschwindigkeit @c SPEED_SLOW, der Beschleunigung @c ACCELERATION_NORMAL bzw. @c DECELERATION_SLOW. Die Aufgabe startet ohne Startverzögerung.
/// Rückgabewert dieser Funktion ist wiederum der Index dieser Aufgabe im Aufgaben-Array, mit dem diese Aufgabe eindeutig identifiziert ist. Dieser Wert wird wiederum in einer aussagekräftigen Variable gespeichert, damit sie später leicht angesprochen werden kann. <br>
/// Die implementierten Methoden sind:
/// - @code addTask(...) @endcode
/// - @code addTaskMoveRelativ(...) @endcode
/// - @code addTaskWithPositionReset(...) @endcode
/// - @code addTaskWithPositionResetOnCompletion(...) @endcode
/// <br><br>
/// @section LinkTask Linking Tasks Together
/// Gestartet, gestoppt und manipuliert werden können die Geräte durch Zeitablauf, durch die Zustandsänderung angeschlossener Sensoren, durch das Erreichen eines Zustandes bzw. einer Position dieses Gerätes sowie durch das Erreichen eines Zustandes bzw. einer Position jedes anderen Gerätes.
/// @code{.cpp}
/// cuttingProcess->device[catStepper]->task[driveCatToCuttingStartPosition]->startByDate(100);
/// @endcode
/// Die Aufgabe @c driveCatToCuttingStartPosition des Gerätes @c catStepper soll 100 ms nachdem der Scheduler gestartet wurde, ausgeführt werden.
/// @code{.cpp}
/// cuttingProcess->device[liftServo]->task[lowerHeadForCutting]->startAfterCompletionOf(catStepper, driveCatToCuttingStartPosition);
/// @endcode
/// Die Aufgabe @c lowerHeadForCutting wird gestartet, nachdem die Aufgabe @c driveCatToCuttingStart des Schneideschlittenantriebs (@c catStepper) abgeschlossen ist.
/// @code{.cpp}
/// cuttingProcess->device[liftServo]->task[lowerHeadForCutting]->stopDynamicallyBySensor(DISTANCE_SENSOR, INITIATE_STOP_VALUE, TARGET_VALUE, STOP_PERFORMANCE, SKIP_APPROXIMATION_PRECISE);
/// @endcode
/// Die Aufgabe "lowerHeadForCutting" wird entsprechend der Sensorwerte des Distanzsensors im Schneidekopf dynamisch gestoppt. Die Fahrgeschwindigkeit wird entsprechend der Sensorwerte geregelt. <br>
/// Die implementierten Methoden sind:
/// - @code startByDate(...) @endcode
/// - @code startByControl(...) @endcode
/// - @code startAfterCompletionOf(...) @endcode
/// - @code startByTriggerpositionOf(...) @endcode
/// - @code switchToNextTaskByDate(...) @endcode
/// - @code switchToNextTaskByControl(...) @endcode
/// - @code switchToNextTaskAfterCompletionOf(...) @endcode
/// - @code switchToNextTaskByTriggerpositionOf(...) @endcode
/// - @code stopByTimeout(...) @endcode
/// - @code stopByControl(...) @endcode
/// - @code stopAfterCompletionOf(...) @endcode
/// - @code stopByTriggerpositionOf(...) @endcode
/// - @code stopDynamicallyBySensor(...) @endcode
/// <br><br>
/// @section registerActions Registering Action Buttons
/// Zu dem Scheduler können nun noch beliebig viele Bedienungselemente hinzugefügt werden. Mittels dieser Schalter kann die Abarbeitung der Aufgaben manipuliert werden. Aufgaben können  gestoppt, übersprungen oder wiederholt werden. In der Klasse CCDeviceScheduler existiert ein Array der Klasse CCControlButton. Dort werden die Schalter eingetragen:
/// @code{.cpp}
/// unsigned int songEndButton = cuttingProcess->addControlButton(SONG_END_BUTTON_NAME, SONG_END_PIN, SONG_END_ACTIV);
/// @endcode
/// Jedes Element dieser Klasse enthält ein Array von @c actions, die das Stoppen, Überspringen oder Wiederholen von Aufgaben veranlassen. Die Eintragung dieser @c actions bewerkstelligt die Funktion @c evokeJumpToNextTask(...) bzw. @c evokeJumpToTask(...). Übergeben werden sämtliche Parameter dieser Aufgabe:
/// @code{.cpp}
/// cuttingProcess->controlButton[songEndButton]->evokeJumpToTask(catStepper, makeMainGroove, TASK_STOP_ACTION_IMMEDIATELY_AND_SWITCH, makeEndGroove);
/// @endcode
/// Die Aufgabe @c makeMainGroove des Schneideschlittenantriebs @c catStepper, also der Schneideprozess, soll abgebrochen werden, wenn der aufzunehmende Song bereits vor dem Ablauf der eingestellten Aufnahmedauer zu ende ist. Dies wird mit dem Schalter @c songEndButton signalisiert. Der Schneideschlittenantrieb soll dann die Aufgabe @c makeMainGroove beenden und direkt zur Aufgabe @c makeEndGroove wechseln, also übergangslos zu der Geschwindigkeit beschleunigen, die zum Schneiden der Endrille notwendig ist. <br>
/// Die implementierten Methoden sind:
/// - @code evokeJumpToNextTask(...) @endcode
/// - @code evokeJumpToTask(...) @endcode
/// <br><br>
/// @section run Run the Process
/// Um den nun fertig konfigurierten Prozess zu starten dient der Befehl @c run.
/// @code{.cpp}
/// cuttingProcess->run();
/// @endcode
/// <br><br>
