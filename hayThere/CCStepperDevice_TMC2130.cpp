//
// CCStepperDevice_TMC2130.cpp
// C++ code
// ----------------------------------
// Developed with embedXcode+
// http://embedXcode.weebly.com
//
// Project 		deviceScheduler
//
// Created by 	Little Abakus, 15.04.16 11:56
// 				Little Abakus
//
// Copyright 	(c) Little Abakus, 2016
// License		<#license#>
//
// See 			CCStepperDevice_TMC2130.h and ReadMe.txt for references
//




#include "CCStepperDevice_TMC2130.h"

#include "Trinamic_TMC2130_registers.h"






CCStepperDevice_TMC2130::CCStepperDevice_TMC2130(const String deviceName, const unsigned int step_pin, const unsigned int dir_pin, const unsigned int enable_pin, const unsigned int stepsPerRotation, const unsigned int chipSelect_pin, unsigned int currentMax) : CCStepperDevice(deviceName, step_pin, dir_pin, enable_pin, stepsPerRotation), chipSelect_pin(chipSelect_pin) {
   
    driverStatus = 0;
    debugHelp="";

    
    pinMode(chipSelect_pin, OUTPUT);
    digitalWrite(chipSelect_pin, HIGH);


    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setDataMode(SPI_MODE3);
    
    SPI.begin();
    
    read_STAT();

    
    this->highestSteppingMode = 8;

    this->stepsPerDegree = stepsPerRotation / 360.0;                                        // save time executing prepareNextTask()
    this->degreesPerMicroStep = 360.0 / stepsPerRotation / (1 << highestSteppingMode);      // save time when calculatin currentPosition in operateTask()
    
    this->acceleration_max = 4000;
    
    this->state = SLEEPING;
    
    this->currentMicroStep = 0;
    this->currentPosition = 0;
    
    this->prepareAndStartNextTaskWhenFinished = false;

    
    this->currentMax = currentMax;
    

    for (unsigned int codeIndex = 0; codeIndex <= highestSteppingMode; codeIndex++) {
        this->steppingUnit[codeIndex] = (1 << (highestSteppingMode - codeIndex));
    }
    
    set_internal_Rsense(0);
    set_I_scale_analog(0);
    set_vsense(0);
    set_IHOLD_IRUN(18, 18, 8);
    set_TPOWERDOWN(200);
    set_en_pwm_mode(1);
    set_pwm_autoscale(1);
    set_PWM_GRAD(1);
    set_PWM_AMPL(255);
    set_pwm_freq(0x01);
    
    set_toff(4); // ([0-15]) 0: driver disable, 1: use only with TBL>2, 2-15: off time setting during slow decay phase

    set_tbl(2); // ([0-3]) set comparator blank time to 16, 24, 36 or 54 clocks, 1 or 2 is recommended

    set_hstrt(4);
    set_hend(0);
    
//    set_IHOLD_IRUN(9,9,5); // ([0-31],[0-31],[0-5]) sets all currents to maximum
//    set_I_scale_analog(0); // ({0,1}) 0: I_REF internal, 1: sets I_REF to AIN
    
    
/*
    //        Serial.print(F("[CCStepperDevice_TMC2130]: setup stepper "));
    //        Serial.print(deviceName);
    //        Serial.print(F(": chipSelect_pin: "));
    //        Serial.print(chipSelect_pin);
    //        Serial.print(F(", currentMax: "));
    //        Serial.print(currentMax);
    //        Serial.print(F(", steppingModes: "));
    //        Serial.print(highestSteppingMode + 1);
    //        if (verbosity & MEMORYDEBUG) {
    //            Serial.print(F(", at $"));
    //            Serial.print((long) this, HEX);
    //        }
    //        Serial.println();
    
    
    setCurrent(currentMax);
    
    
    // #################### DriverControlRegister: ####################
    
    // Enable STEP interpolation (INTPOL):
    //  0: Disable STEP pulse interpolation.
    //  1: Enable STEP pulse multiplication by 16.
    bool stepInterpolation = 0;
    
    // Enable double edge STEP pulses (DEDGE):
    //  0: Rising STEP pulse edge is activee, falling edge is inactivee.
    //  1: Both rising and falling STEP pulse edges are activee.
    bool doubleEdgeStepPulses = 0;
    
    // Microstep resolution for STEP/DIR mode (MRES3):
    //  Microsteps per 90°:
    //  %0000 (0): 256
    //  %0001 (1): 128
    //  %0010 (2): 64
    //  %0011 (3): 32
    //  %0100 (4): 16
    //  %0101 (5): 8
    //  %0110 (6): 4
    //  %0111 (7): 2 (halfstep)
    //  %1000 (8): 1 (fullstep)
    unsigned int microSteppingMode = 8;
    
    setDriverControlRegister(stepInterpolation, doubleEdgeStepPulses, microSteppingMode);
    
    
    // #################### ChopperControlRegister: ####################
    // blanking time (TBL):
    //  Blanking time interval, in system clock periods:
    //  %00: 16; %01: 24; %10: 36; %11: 54
    //  internal clock: 15 MHz ==> 1 systemClockPeriod is 67 ns
    //  16: 1.06 us [= 0]; 24: 1.6 us [= 1]; 36: 2.4 us [= 2]; 54: 3.6 us [= 3]
    unsigned int blankingTimeValue = 0;
    
    // chopper mode (CHM):
    //  This mode bit affects the interpretation of the HDEC, HEND, and HSTRT parameters shown below.
    //  0: Standard mode (spreadCycle)
    //  1: Constant tOFF with fast decay time. Fast decay time is also terminated when the negative nominal current is reached. Fast decay is after on time.
    bool chopperMode = 0;
    
    // random Toff time (RNDTF):
    //  Enable randomizing the slow decay phase duration:
    //  0: Chopper off time is fixed as set by bits tOFF
    //  1: Random mode, tOFF is random modulated by dNCLK= -12 ... +3 clocks.
    bool randomTOffTime = 0;
    
    // Off time/MOSFET disable (TOFF):
    //  Duration of slow decay phase. If TOFF is 0, the MOSFETs are shut off. If TOFF is nonzero, slow decay time is a multiple of system clock periods: NCLK= 12 + (32 x TOFF) (Minimum time is 64clocks.)
    //  %0000: Driver disable, all bridges off
    //  %0001: 1 (use with TBL of minimum 24 clocks) %0010 ... %1111: 2 ... 15
    //  internal clock: 15 MHz ==> 1 systemClockPeriod is 67 ns, clocks = 12 + 32 * offTime
    //  offTime 0: MOSFET shut off; 2: (76 cycles): 5.1 us; 3: (108 cycles): 7.2 us; 15 (492 cycles): 32.8 us
    unsigned int offTime = 15;
    
    if (chopperMode == 0) {
        // spreadCycleMode
        
        // hysteresis decrement interval (HDEC):
        //  Hysteresis decrement period setting, in system clock periods:
        //  %00: 16; %01: 32; %10: 48; %11: 64
        unsigned int hysteresisDecrementPeriodValue = 1;
        
        // hysteresis end (low) value (HEND):
        //  %0000 ... %1111: Hysteresis is -3, -2, -1, 0, 1, ..., 12
        //  (1/512 of this setting adds to current setting) This is the hysteresis value which becomes used for the hysteresis chopper.
        int hysteresisEnd = 12;
        
        // hysteresis start value (HSTRT)
        //  hysteresis start offset from HEND
        //  %000: 1; %001: 2; %010: 3; %011: 4; ... %111: 8
        //  Effective: HEND + HSTRT must be ≤ 15
        unsigned int hysteresisStart = 4;
        
        setChopperControlRegister_spreadCycle(blankingTimeValue, randomTOffTime, hysteresisDecrementPeriodValue, hysteresisEnd, hysteresisStart, offTime);
        
    } else {
        // fastDecayMode
        
        // Fast decay mode (with constant offTime) (HDEC):
        //  0: current comparator can terminate the fast decay phase before timer expires.
        //  1: only the timer terminates the fast decay phase.
        bool onlyTimerTerminatesDecayPhase = 0;
        
        // sinwave offset (HEND):
        //  %0000 ... %1111; Offset is -3, -2, -1, 0, 1, ..., 12
        //  This is the sine wave offset and 1/512 of the value becomes added to the absolute value of each sine wave entry.
        int sinwaveOffset = 3;
        
        // fast decay time (HSTRT):
        //  duration of the fast decay phase. The MSB is HDEC0.
        //  Fast decay time is a multiple of system clock periods: NCLK= 32 x (HDEC0+HSTRT)
        unsigned int fastDecayTime = 3;
        
        // Off time/MOSFET disable (TOFF):
        //  Duration of slow decay phase. If TOFF is 0, the MOSFETs are shut off. If TOFF is nonzero, slow decay time is a multiple of system clock periods: NCLK= 12 + (32 x TOFF) (Minimum time is 64clocks.)
        //  %0000: Driver disable, all bridges off
        //  %0001: 1 (use with TBL of minimum 24 clocks) %0010 ... %1111: 2 ... 15
        //  internal clock: 15 MHz ==> 1 systemClockPeriod is 67 ns, clocks = 12 + 32 * offTime
        //  offTime 0: MOSFET shut off; 2: (76 cycles): 5.1 us; 3: (108 cycles): 7.2 us; 15 (492 cycles): 32.8 us
        unsigned int offTime = 3;
        
        setChopperControlRegister_fastDecay(blankingTimeValue, randomTOffTime, onlyTimerTerminatesDecayPhase, sinwaveOffset, fastDecayTime, offTime);
    }
    
    
    
    // #################### CoolStepControlRegister: ####################
    
    // Minimum coolStep current (SEIMIN):
    // 0: 1⁄2 CS current setting; 1: 1⁄4 CS current setting
    bool minCoolStepCurrentValue = 0;
    
    // current decrerment speed (SEDN):
    //  Number of times that the stallGuard2 value must be sampled equal to or above the upper threshold for each decrement of the coil current:
    //  %00: 32; %01: 8; %10: 2; %11: 1
    unsigned int currentDecrementSpeedValue = 0x00;
    
    // upper cool step treshold as an offset from the lower threshold (SEMAX):
    //  If the stallGuard2 measurement value SG is sampled equal to or above (SEMIN+SEMAX+1) x 32 enough times, then the coil current scaling factor is decremented.
    unsigned int upperCoolStepThreshold = 2;
    
    // current increment size (SEUP):
    // Number of current increment steps for each time that the stallGuard2 value SG is sampled below the lower threshold:
    //  %00: 1; %01: 2; %10: 4; %11: 8
    unsigned int currentIncrementStepsValue = 0x01;
    
    // lower coolStep threshold / coolStep disable (SEMIN)
    // If SEMIN is 0, coolStep is disabled. If SEMIN is nonzero and the stallGuard2 value SG falls below SEMIN x 32, the coolStep current scaling factor is increased.
    unsigned int lowerCoolStepThreshold = 1; // 1
    
    setCoolStepRegister(minCoolStepCurrentValue, currentDecrementSpeedValue, upperCoolStepThreshold, currentIncrementStepsValue, lowerCoolStepThreshold);
    
    
    
    
    // #################### StallGuard2ControlRegister: ####################
    
    // stallGuard2 filter enable (SFILT):
    //  0: Standard mode, fastest response time.
    //  1: Filtered mode, updated once for each four fullsteps to compensate for variation in motor construction, highest accuracy.
    bool stallGuard2FilterEnable = false;
    
    // stallGuard2 threshold value (SGT):
    // The stallGuard2 threshold value controls the optimum measurement range for readout. A lower value results in a higher sensitivity and requires less torque to indicate a stall. The value is a two’s complement signed integer. Values below -10 are not recommended. Range: -64 to +63
    int stallGuard2Threshold = 10;
    
    // Current scale (scales digital currents A and B) (CS)
    //  Current scaling for SPI and step/direction operation. %00000 ... %11111: 1/32, 2/32, 3/32, ... 32/32
    //  This value is biased by 1 and divided by 32, so the range is 1/32 to 32/32. Example: CS=0 is 1/32 current
    // calculate it:
    calculateCurrentSetup(currentMax);
    
    setStallGuard2Register(stallGuard2FilterEnable, stallGuard2Threshold);
    
    
    
    // #################### DriverConfigurationRegister: ####################
    
    
    // Slope control, high side (SLPH):
    //  %00: Minimum;
    //  %01: Minimum temperature compensation mode.
    //  %10: Medium temperature compensation mode.
    //  %11: Maximum
    //  In temperature compensated mode (tc), the MOSFET gate driver strength is increased if the overtemperature warning temperature is reached. This compensates for temperature dependency of high-side slope control.
    unsigned int slopeControlHighSide = 0x10;
    
    // Slope control, low side (SLPL):
    //  %00: Minimum. %01: Minimum. %10: Medium. %11: Maximum.￼
    unsigned int slopeControlLowSide = 0x10;
    
    // Short to GND protection disable (DISS2G):
    //  0: Short to GND protection is enabled. 1: Short to GND protection is disabled.
    bool shortToGndProtectionDisable = 0;
    
    // Short to GND detection timer (TS2G1):
    //  %00: 3.2μs. %01: 1.6μs. %10: 1.2μs. %11: 0.8μs.
    unsigned int shortToGndDetectionTimerValue = 0x01;
    
    // STEP/DIR interface disable (SDOFF):
    //  0: Enable STEP and DIR interface; 1: Disable STEP and DIR interface. SPI interface is used to move motor.
    bool stepDirInterfaceDisable = false;
    
    // Sense resistor voltage-based current scaling (VSENSE):
    //  0: Full-scale sense resistor voltage is 305mV.
    //  1: Full-scale sense resistor voltage is 165mV. (Full-scale refers to a current setting of 31 and a DAC value of 255.)
    // senseResistorVoltage165mV is calculated in calculateCurrentSetup() called in stallGuard2Register setup
    
    // Select value for read out (RD bits) (RDSEL):
    //  %00: Microstep position read back
    //  %01: stallGuard2 level read back
    //  %10: stallGuard2 and coolStep current level read back
    //  %11: Reserved, do not use
    unsigned int selectReadOut = 0x10;
    
    setDriverConfigurationRegister(slopeControlHighSide, slopeControlLowSide, shortToGndProtectionDisable, shortToGndDetectionTimerValue, stepDirInterfaceDisable, selectReadOut);
    
*/
}

CCStepperDevice_TMC2130::~CCStepperDevice_TMC2130() {
    digitalWrite(enable_pin, HIGH);
    
    pinMode(dir_pin, INPUT);
    pinMode(step_pin, INPUT);
    pinMode(enable_pin, INPUT_PULLUP);
    pinMode(chipSelect_pin, INPUT_PULLUP);
    
    if (verbosity & BASICOUTPUT) {
        Serial.print(F("[CCStepperDevice_TMC2130]: device "));
        Serial.print(deviceName);
        Serial.println(F(" destructed"));
    }
}


void CCStepperDevice_TMC2130::setupMicroSteppingMode(unsigned int data) {
    microSteppingMode = data;
    CCStepperDevice_TMC2130::setupMicroSteppingMode();
}
void CCStepperDevice_TMC2130::setupMicroSteppingMode() {
/*
    //delete the old value
    this->driverControl &= 0xFFFF0ul;
    //set the new value
    this->driverControl |= (8 - microSteppingMode);
    
    doTransaction(driverControl);

    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(" set microStepping: "));
        Serial.println(8 - microSteppingMode);
    }
 */
   
    set_CHOPCONF(TMC_CHOPCONF_MRES, 8 - microSteppingMode);
    

}





void CCStepperDevice_TMC2130::calculateCurrentSetup(unsigned int current) {
    // the mesurement full scale on the chip can eiter be set to 165mV or 310mV
    // the shunt on the shield (R_SENSE) is 150mOhm
    // VsenseMax = R_SENSE * coilCurrentMax [/ 1000]
    //  ==> coilCurrentMax = VsenseMax / R_SENSE:
    // VsenseMax = 165mV ==> coilCurrentMax = 165mV / 150mOhm = 1100mA
    // VsenseMax = 310mV ==> coilCurrentMax = 310mV / 150mOhm = 2066mA
    
    unsigned int VSenseFullScale;                        // voltage in mV - it can either be 165mV or 310MV
    if ((long) R_SENSE * current <= 165000) {
        VSenseFullScale = 165;
        senseResistorVoltage165mV = true;
    }
    else {
        VSenseFullScale = 310;
        senseResistorVoltage165mV = false;
    }
    //  from datasheet:  I = (CS + 1) / 32 * Vfs / Rs
    //  here:  CS + 1 = currentScaleOf32; Vfs = VSenseFullScale / 1000; Rs = R_SENSE / 1000
    //  so: I / 1000 = currentScaleOf32 / 32 * VSenseFullScale / R_SENSE
    //  ==>  currentScaleOf32 = (32 * R_SENSE * I) / VSenseFullScale;
    currentScaleOf32 = (float)(32.0 * R_SENSE * current) / VSenseFullScale / 1000.0 + 0.5;               // (+ 0.5 is for better rounding)
    //   300 mA: 32 * 0.15 * 0.3 / 0.165 =  8.73 -->  9
    //   600 mA: 32 * 0.15 * 0.6 / 0.165 = 17.45 --> 17
    //  1000 mA: 32 * 0.15 * 1.0 / 0.165 = 29.09 --> 29
    //  1100 mA: 32 * 0.15 * 1.1 / 0.165 = 32    --> 32
    //  1200 mA: 32 * 0.15 * 1.1 / 0.310 = 18.58 --> 19
    //  2060 mA: 32 * 0.15 * 1.0 / 0.310 = 31.89 --> 32

    currentScaleOf32 = fmin(currentScaleOf32, 32);

    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        if (senseResistorVoltage165mV) {
            Serial.print(F(": currentScaling full scale, "));
        } else {
            Serial.print(F(": currentScaling halfed, "));
        }
        Serial.print(F(" currentScaleOf32: "));
        Serial.println(currentScaleOf32);
    }

}




void CCStepperDevice_TMC2130::setCurrent(unsigned int current) {
    calculateCurrentSetup(current);
    
    //delete the old value
	stallGuard2Control &= ~0x1F;
	//set the new current scaling
	stallGuard2Control |= (currentScaleOf32 - 1) & 0x1F;
	doTransaction(stallGuard2Control);

    bitWrite(driverConfiguration, 6, senseResistorVoltage165mV);
    doTransaction(driverConfiguration);
	
}
void CCStepperDevice_TMC2130::setCurrentScale(unsigned int currentScale) {
    currentScaleOf32 = currentScale;
    
    //delete the old value
	stallGuard2Control &= ~0x1F;
	//set the new current scaling
	stallGuard2Control |= (currentScaleOf32 - 1) & 0x1F;
	doTransaction(stallGuard2Control);
}


void CCStepperDevice_TMC2130::getReadOut(unsigned int dataToRead) {
    
    driverConfiguration &= ~0x30;
    driverConfiguration |= (dataToRead & 0x3) << 4;
    
    doTransaction(driverConfiguration);
    
    switch (dataToRead) {
        case READOUT_MICROSTEP_POSITION:
//            microstepPosition = (this->resultDatagram >> 10) & 0x1FF;
//            if (verbosity & TMC2130_SETUPDEBUG) {
//                Serial.print(F("[CCStepperDevice_TMC2130]: "));
//                Serial.print(deviceName);
//                Serial.print(": microstepPosition: ");
//                Serial.println(microstepPosition);
//            }
            break;
        case READOUT_STALLGUARD_LEVEL:
//            stallGuard2Value = (this->resultDatagram >> 10) & 0x3FF;
//            if (verbosity & TMC2130_SETUPDEBUG) {
//                Serial.print(F("[CCStepperDevice_TMC2130]: "));
//                Serial.print(deviceName);
//                Serial.print(": stallGuard2: ");
//                Serial.println(stallGuard2Value);
//            }
            break;
        default:
//            coolStepScalingValue = (this->resultDatagram >> 10) & 0x1F;
//            stallGuard2Value_upper = (this->resultDatagram >> 15) & 0x1F;
//            
//            if (verbosity & TMC2130_SETUPDEBUG) {
//                Serial.print(F("[CCStepperDevice_TMC2130]: "));
//                Serial.print(deviceName);
//                Serial.print(": coolStepScaling: ");
//                Serial.print(coolStepScalingValue);
//                Serial.print(", stallGuard2: ");
//                Serial.println(stallGuard2Value_upper);
//            }
            ;
    }
    /*
    standStil = bitRead(this->resultDatagram, 7);
    
    openLoad = (this->resultDatagram & 0x60) >> 5;
    
    shortToGnd = (this->resultDatagram & 0x18) >> 3;
    
    overTemperatureWarning = this->resultDatagram & 0x4;
    
    overTemperatureShutdown = this->resultDatagram & 0x2;
    
    stallGuard2Status = this->resultDatagram & 0x1;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print("  standStil: ");
        Serial.print(standStil);
        Serial.print("  openLoad: ");
        Serial.print(openLoad);
        Serial.print("  shortToGnd: ");
        Serial.print(shortToGnd);
        Serial.print("  overTemperatureWarning: ");
        Serial.print(overTemperatureWarning);
        Serial.print("  overTemperatureShutdown: ");
        Serial.print(overTemperatureShutdown);
        Serial.print(": stallGuard2Status: ");
        Serial.println(stallGuard2Status);
    }
    */

}

void CCStepperDevice_TMC2130::doTransaction(unsigned long datagram) {

    if (verbosity & TMC2130_SPIDEBUG) {
        Serial.print("##### TMC2130 SPI: Sending: ");
        Serial.println(datagram, HEX);
    }
    
    //select the TMC2130
    digitalWrite(chipSelect_pin, LOW);
    
    //write/read the values
    resultDatagram = SPI.transfer((datagram >> 16) & 0xff);
    resultDatagram <<= 8;
    resultDatagram |= SPI.transfer((datagram >>  8) & 0xff);
    resultDatagram <<= 8;
    resultDatagram |= SPI.transfer((datagram) & 0xff);
    resultDatagram >>= 4;
    
    //deselect the TMC chip
    digitalWrite(chipSelect_pin, HIGH);
    
    if (verbosity & TMC2130_SPIDEBUG) {
        Serial.print("##### TMC2130 SPI: Receiving: ");
        Serial.println(resultDatagram, HEX);
    }
}


void CCStepperDevice_TMC2130::setDriverControlRegister(bool stepInterpolation, bool doubleEdgeStepPulses, unsigned int microSteppingMode) {
    
    driverControl = 0x00000;
    
    bitWrite(driverControl, 9, stepInterpolation);
    bitWrite(driverControl, 8, doubleEdgeStepPulses);
    driverControl |= microSteppingMode & 0x0f;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup Driver: stepInterpolation: ")) , Serial.print(stepInterpolation);
        Serial.print(F(", doubleEdgeStepPulses: ")) , Serial.print(doubleEdgeStepPulses);
        Serial.print(F(", microSteppingValue: ")) , Serial.print(8 - microSteppingMode);
        Serial.print(F(" = mode ")) , Serial.println(microSteppingMode);
    }
    
    
    doTransaction(driverControl);
    
}
void CCStepperDevice_TMC2130::setChopperControlRegister_spreadCycle(unsigned int blankingTimeValue, bool randomTOffTime, unsigned int hysteresisDecrementPeriodValue, int hysteresisEnd, unsigned int hysteresisStart, unsigned int offTime) {
    
    chopperControl = 0x80000;
    
    chopperControl |= (blankingTimeValue & 0x03) << 15;
    bitWrite(chopperControl, 14, 0);
    bitWrite(chopperControl, 13, randomTOffTime);
    chopperControl |= (hysteresisDecrementPeriodValue & 0x3) << 11;
    chopperControl |= ((hysteresisEnd + 3) & 0xF) << 7;
    chopperControl |= ((hysteresisStart - 1) & 0x7) << 4;
    chopperControl |= offTime & 0xF;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup Chopper: Spread Cycle: blankingTimeValue: ")) , Serial.print(blankingTimeValue);
        Serial.print(F(", randomTOffTime: ")) , Serial.print(randomTOffTime);
        Serial.print(F(", hysteresisDecrementPeriodValue: ")) , Serial.print(hysteresisDecrementPeriodValue);
        Serial.print(F(", hysteresisEnd: ")) , Serial.print(hysteresisEnd);
        Serial.print(F(", hysteresisStart: ")) , Serial.print(hysteresisStart);
        Serial.print(F(", offTime: ")) , Serial.println(offTime);
    }

    doTransaction(chopperControl);
    
}
void CCStepperDevice_TMC2130::setChopperControlRegister_fastDecay(unsigned int blankingTimeValue, bool randomTOffTime, bool onlyTimerTerminatesDecayPhase, int sinwaveOffset, unsigned int fastDecayTime, unsigned int offTime) {
    
    chopperControl = 0x80000;
    
    chopperControl |= (blankingTimeValue & 0x03) << 15;
    bitWrite(chopperControl, 14, 1);
    bitWrite(chopperControl, 13, 0);
    bitWrite(chopperControl, 12, onlyTimerTerminatesDecayPhase);
    chopperControl |= ((fastDecayTime & 0x8) >> 3) << 11;
    chopperControl |= ((sinwaveOffset + 3) & 0xF) << 7;
    chopperControl |= (fastDecayTime & 0x7) << 4;
    chopperControl |= offTime & 0xF;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup Chopper: Fast Decay: blankingTimeValue: ")) , Serial.print(blankingTimeValue);
        Serial.print(F(", randomTOffTime: ")) , Serial.print(randomTOffTime);
        Serial.print(F(", onlyTimerTerminatesDecayPhase: ")) , Serial.print(onlyTimerTerminatesDecayPhase);
        Serial.print(F(", sinwaveOffset: ")) , Serial.print(sinwaveOffset);
        Serial.print(F(", fastDecayTime: ")) , Serial.print(fastDecayTime);
        Serial.print(F(", offTime: ")) , Serial.println(offTime);
    }

    doTransaction(chopperControl);

}


void CCStepperDevice_TMC2130::setCoolStepRegister(bool minCoolStepCurrentValue, unsigned int currentDecrementSpeedValue, unsigned int upperCoolStepThreshold, unsigned int currentIncrementStepsValue, unsigned int lowerCoolStepThreshold) {
    
    coolStepControl = 0xA0000;
    
    bitWrite(coolStepControl, 15, minCoolStepCurrentValue);
    coolStepControl |= (currentDecrementSpeedValue & 0x3) << 13;
    coolStepControl |= (upperCoolStepThreshold & 0xF) << 8;
    coolStepControl |= (currentIncrementStepsValue & 0x3) << 5;
    coolStepControl |= lowerCoolStepThreshold & 0xF;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup Cool Step: minCoolStepCurrent: "));
        if (minCoolStepCurrentValue) {
            Serial.print("halfed");
        } else {
            Serial.print("quartered");
        }
        Serial.print(F(", currentDecrementSpeedValue: ")) , Serial.print(currentDecrementSpeedValue);
        Serial.print(F(", upperCoolStepThreshold: ")) , Serial.print(upperCoolStepThreshold);
        Serial.print(F(", currentIncrementStepsValue: ")) , Serial.print(currentIncrementStepsValue);
        Serial.print(F(", lowerCoolStepThreshold: ")) , Serial.println(lowerCoolStepThreshold);
    }

    doTransaction(coolStepControl);

}
void CCStepperDevice_TMC2130::setStallGuard2Register(bool stallGuard2FilterEnable, int stallGuard2Threshold) {

    stallGuard2Control = 0xC0000;
 
    bitWrite(stallGuard2Control, 16, stallGuard2FilterEnable);
    stallGuard2Control |= (stallGuard2Threshold & 0x7F) << 8;
    stallGuard2Control |= (currentScaleOf32 - 1) & 0x1F;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup Stall Guard 2: stallGuard2FilterEnable: ")) , Serial.print(stallGuard2FilterEnable);
        Serial.print(F(", stallGuard2Threshold: ")) , Serial.print(stallGuard2Threshold);
        Serial.print(F(", currentScaleOf32: ")) , Serial.println(currentScaleOf32);
    }

    doTransaction(stallGuard2Control);

}
void CCStepperDevice_TMC2130::setDriverConfigurationRegister(unsigned int slopeControlHighSide, unsigned int slopeControlLowSide, bool shortToGndProtectionDisable, unsigned int shortToGndDetectionTimerValue, bool stepDirInterfaceDisable, unsigned int selectReadOut) {
    driverConfiguration = 0xE0000;
    
    driverConfiguration |= (slopeControlHighSide & 0x3) << 14;
    driverConfiguration |= (slopeControlLowSide & 0x3) << 12;
    bitWrite(driverConfiguration, 10, shortToGndProtectionDisable);
    driverConfiguration |= (shortToGndDetectionTimerValue & 0x3) << 8;
    bitWrite(driverConfiguration, 7, stepDirInterfaceDisable);
    bitWrite(driverConfiguration, 6, senseResistorVoltage165mV);
    driverConfiguration |= (selectReadOut & 0x3) << 4;
    
    if (verbosity & TMC2130_SETUPDEBUG) {
        Serial.print(F("[CCStepperDevice_TMC2130]: "));
        Serial.print(deviceName);
        Serial.print(F(": setup: Driver Configuration: slopeControlHighSide: ")) , Serial.print(slopeControlHighSide);
        Serial.print(F(", slopeControlLowSide: ")) , Serial.print(slopeControlLowSide);
        Serial.print(F(", shortToGndProtectionDisable: ")) , Serial.print(shortToGndProtectionDisable);
        Serial.print(F(", shortToGndDetectionTimerValue: ")) , Serial.print(shortToGndDetectionTimerValue);
        Serial.print(F(", stepDirInterfaceDisable: ")) , Serial.print(stepDirInterfaceDisable);
        Serial.print(F(", senseResistorVoltage165mV: ")) , Serial.print(senseResistorVoltage165mV);
        Serial.print(F(", selectReadOut: ")) , Serial.println(selectReadOut);
    }

    
    doTransaction(driverConfiguration);

}


uint16_t CCStepperDevice_TMC2130::getDeviceDriverStatus(deviceDriverStatusInfo info) {
    switch (info) {
        case STANDSTILL:
            return extendedDriverStatus & TMC_DRV_STATUS_STANDSTILL_MASK;
        case OPENLOAD:
            return extendedDriverStatus & TMC_DRV_STATUS_OPENLOAD_MASK;
        case SHORT_TO_GROUND:
            return extendedDriverStatus & TMC_DRV_STATUS_SHORT2GND_MASK;
        case OVERTEMPERATURE_WARNING:
            return extendedDriverStatus & TMC_DRV_STATUS_OVERTEMP_WARNING_MASK;
        case OVERTEMPERATURE_SHUTDOWN:
            return extendedDriverStatus & TMC_DRV_STATUS_OVERTEMPERATURE_MASK;
        case MOTORSTALL:
            return extendedDriverStatus & TMC_DRV_STATUS_STALL_MASK;
        case MOTOR_CURRENT:
            return (extendedDriverStatus & TMC_DRV_STATUS_MOTORCURRENT_MASK) >> TMC_DRV_STATUS_MOTORCURRENT_OFFSET;
        case STALLGUARD_VALUE:
            return extendedDriverStatus & TMC_DRV_STATUS_STALLGUARD_RESULT_MASK;
        default:
            return 0;
    }
}

void CCStepperDevice_TMC2130::readStatus() {
//    digitalWrite(chipSelect_pin, LOW);
//    
//    // read address
//    driverStatus = SPI.transfer(TMC_REG_DRV_STATUS);
//    
//    // flush 4 bytes
//    for(int i=0;i<4;i++){
//        SPI.transfer(0x00);
//    }
//    
//    digitalWrite(chipSelect_pin, HIGH);
//    // restart transmission
//    digitalWrite(chipSelect_pin, LOW);
//    
//    // read address
//    driverStatus = SPI.transfer(TMC_REG_DRV_STATUS);
//    
//    // retrieve data
//    extendedDriverStatus  = SPI.transfer(0x00)&0xFF;
//    extendedDriverStatus <<=8;
//    extendedDriverStatus |= SPI.transfer(0x00)&0xFF;
//    extendedDriverStatus <<=8;
//    extendedDriverStatus |= SPI.transfer(0x00)&0xFF;
//    extendedDriverStatus <<=8;
//    extendedDriverStatus |= SPI.transfer(0x00)&0xFF;
//    
//    digitalWrite(chipSelect_pin, HIGH);

    read_REG(TMC_REG_DRV_STATUS, &extendedDriverStatus);
    
    Serial.print("*** driverStatus: ");
    Serial.print(driverStatus);
    Serial.print(", readOut: ");
    Serial.print(extendedDriverStatus);
    Serial.print(", STANDSTILL: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_STANDSTILL_MASK);
    Serial.print(", OPENLOAD: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_OPENLOAD_MASK);
    Serial.print(", SHORT_TO_GROUND: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_SHORT2GND_MASK);
    Serial.print(", OVERTEMPERATURE_WARNING: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_OVERTEMP_WARNING_MASK);
    Serial.print(", OVERTEMPERATURE_SHUTDOWN: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_OVERTEMPERATURE_MASK);
    Serial.print(", MOTORSTALL: ");
    Serial.print(extendedDriverStatus & TMC_DRV_STATUS_STALL_MASK);
    Serial.print(", MOTOR_CURRENT: ");
    Serial.print((extendedDriverStatus & TMC_DRV_STATUS_MOTORCURRENT_MASK) >> TMC_DRV_STATUS_MOTORCURRENT_OFFSET);
    Serial.print(", STALLGUARD_VALUE: ");
    Serial.println(extendedDriverStatus & TMC_DRV_STATUS_STALLGUARD_RESULT_MASK);

}



// read status
uint8_t CCStepperDevice_TMC2130::read_STAT()
{
        digitalWrite(chipSelect_pin, LOW);
    
    // read address
    driverStatus = SPI.transfer(0x00);
    
    // flush 4 bytes
    for(int i=0;i<4;i++){
        SPI.transfer(0x00);
    }
    
    digitalWrite(chipSelect_pin, HIGH);
    
    return driverStatus;
}

// read a register
uint8_t CCStepperDevice_TMC2130::read_REG(uint8_t address, uint32_t *data)
{
        digitalWrite(chipSelect_pin, LOW);
    
    // read address
    driverStatus = SPI.transfer(address&~TMC_WRITE);
    
    // flush 4 bytes
    for(int i=0;i<4;i++){
        SPI.transfer(0x00);
    }
    
    digitalWrite(chipSelect_pin, HIGH);
    // restart transmission
    delay(1);
    digitalWrite(chipSelect_pin, LOW);
    
    // read address
    driverStatus = SPI.transfer(address&~TMC_WRITE);
    
    // retrieve data
    *data  = SPI.transfer(0x00)&0xFF;
    *data <<=8;
    *data |= SPI.transfer(0x00)&0xFF;
    *data <<=8;
    *data |= SPI.transfer(0x00)&0xFF;
    *data <<=8;
    *data |= SPI.transfer(0x00)&0xFF;
    
    digitalWrite(chipSelect_pin, HIGH);
    
    return driverStatus;
}

// write to a register
uint8_t CCStepperDevice_TMC2130::write_REG(uint8_t address, uint32_t data)
{
    digitalWrite(chipSelect_pin, LOW);
    
    // write address
    driverStatus = SPI.transfer(address|TMC_WRITE);
    
    // write data
    SPI.transfer((data>>24UL)&0xFF);
    SPI.transfer((data>>16UL)&0xFF);
    SPI.transfer((data>> 8UL)&0xFF);
    SPI.transfer((data>> 0UL)&0xFF);
    
    digitalWrite(chipSelect_pin, HIGH);
    
    return driverStatus;
}

// alter a register using a bitmask
uint8_t CCStepperDevice_TMC2130::alter_REG(uint8_t address, uint32_t data, uint32_t mask)
{
    uint32_t oldData, newData;
    
    read_REG( address, &oldData );
    
    newData = ( oldData&~mask ) | ( data&mask );
    
    write_REG( address, newData );
    
    return driverStatus;
}

// set single bits in the GCONF register
uint8_t CCStepperDevice_TMC2130::set_GCONF(uint8_t position, uint8_t value)
{
    alter_REG(TMC_REG_GCONF, uint32_t(value)<<position, 0x1UL<<position);
    
    return driverStatus;
}

// set single bits or values in the chopconf register (constraining masks are applied if necessary)
uint8_t CCStepperDevice_TMC2130::set_CHOPCONF(uint8_t position, uint8_t value)
{
    alter_REG(TMC_REG_CHOPCONF, uint32_t(value)<<position, TMC_CHOPCONF_MASKS[position]<<position);
    
    return driverStatus;
}

//////////
// GCONF
//////////

uint8_t CCStepperDevice_TMC2130::set_I_scale_analog(uint8_t value)
{
    set_GCONF(TMC_GCONF_I_SCALE_ANALOG, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_internal_Rsense(uint8_t value)
{
    set_GCONF(TMC_GCONF_INTERNAL_RSENSE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_en_pwm_mode(uint8_t value)
{
    set_GCONF(TMC_GCONF_EN_PWM_MODE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_enc_commutation(uint8_t value)
{
    set_GCONF(TMC_GCONF_ENC_COMMUTATION, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_shaft(uint8_t value)
{
    set_GCONF(TMC_GCONF_SHAFT, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag0_error(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG0_ERROR, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag0_otpw(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG0_OTPW, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag0_stall(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG0_STALL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag1_stall(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG1_STALL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag1_index(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG1_INDEX, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag1_onstate(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG1_ONSTATE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag1_steps_skipped(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG1_STEPS_SKIPPED, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag0_int_pushpull(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG0_INT_PUSHPULL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_diag1_int_pushpull(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIAG1_INT_PUSHPULL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_small_hysteresis(uint8_t value)
{
    set_GCONF(TMC_GCONF_SMALL_HYSTERESIS, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_stop_enable(uint8_t value)
{
    set_GCONF(TMC_GCONF_STOP_ENABLE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_direct_mode(uint8_t value)
{
    set_GCONF(TMC_GCONF_DIRECT_MODE, value);
    
    return driverStatus;
}

/*
 uint8_t CCStepperDevice_TMC2130::set_test_mode(uint8_t value)
 {
 set_GCONF(TMC_GCONF_TEST_MODE, value);
 
 return driverStatus;
 }
 */

//////////
// IHOLD_IRUN
//////////

uint8_t CCStepperDevice_TMC2130::set_IHOLD_IRUN(uint8_t ihold, uint8_t irun, uint8_t iholddelay)
{
    uint32_t data;
    // adding ihold
    data = (( uint32_t(ihold)&TMC_IHOLD_MASK )<<TMC_IHOLD );
    // adding irun
    data |= (( uint32_t(irun)&TMC_IRUN_MASK )<<TMC_IRUN );
    // adding iholddelay
    data |= (( uint32_t(iholddelay)&TMC_IHOLDDELAY_MASK )<<TMC_IHOLDDELAY );
    
    // writing data
    write_REG(TMC_REG_IHOLD_IRUN, data);
    
    return driverStatus;
}

//////////
// TPOWERDOWN
//////////

uint8_t CCStepperDevice_TMC2130::set_TPOWERDOWN(uint8_t value)
{
    uint32_t data;
    
    data = value & TMC_TPOWERDOWN_MASK;
    
    write_REG(TMC_REG_TPOWERDOWN, data);
    
    return driverStatus;
}

//////////
// TSTEP
//////////

uint32_t CCStepperDevice_TMC2130::get_TSTEP()
{
    uint32_t data;
    
    read_REG(TMC_REG_TPOWERDOWN, &data);
    
    data &= TMC_TSTEP_MASK;
    
    return data;
}

//////////
// TPWMTHRS
//////////

uint8_t CCStepperDevice_TMC2130::set_TPWMTHRS(uint32_t value)
{
    uint32_t data;
    
    data = value & TMC_TPWMTHRS_MASK;
    
    write_REG(TMC_REG_TPOWERDOWN, data);
    
    return driverStatus;
}

//////////
// TCOOLTHRS
//////////

uint8_t CCStepperDevice_TMC2130::set_TCOOLTHRS(uint32_t value)
{
    uint32_t data;
    
    data = value & TMC_TCOOLTHRS_MASK;
    
    write_REG(TMC_REG_TCOOLTHRS, data);
    
    return driverStatus;
}

//////////
// THIGH
//////////

uint8_t CCStepperDevice_TMC2130::set_THIGH(uint32_t value)
{
    uint32_t data;
    
    data = value & TMC_THIGH_MASK;
    
    write_REG(TMC_REG_THIGH, data);
    
    return driverStatus;
}

//////////
// XDIRECT
//////////

uint8_t CCStepperDevice_TMC2130::set_XDIRECT(int16_t coil_a, int16_t coil_b)
{
    uint32_t data;
    
    data = 0x0;
    data |= ( coil_b & TMC_XDIRECT_COIL_B_MASK );
    data = data << TMC_XDIRECT_COIL_B;
    data |= ( coil_a & TMC_XDIRECT_COIL_A_MASK );
    data &= TMC_XDIRECT_MASK;
    
    write_REG(TMC_REG_XDIRECT, data);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_XDIRECT(uint32_t value)
{
    uint32_t data;
    
    data = value;// & TMC_XDIRECT_MASK;
    
    write_REG(TMC_REG_XDIRECT, data);
    
    return driverStatus;
}

int32_t CCStepperDevice_TMC2130::get_XDIRECT()
{
    uint32_t data;
    
    read_REG(TMC_REG_XDIRECT, &data);
    
    data &= TMC_XDIRECT_MASK;
    
    return data;
}

//////////
// VDCMIN
//////////

uint8_t CCStepperDevice_TMC2130::set_VDCMIN(int32_t value)
{
    int32_t data;
    
    data = value & TMC_VDCMIN_MASK;
    
    write_REG(TMC_REG_VDCMIN, data);
    
    return driverStatus;
}

//////////
// MSLUT
//////////

uint8_t CCStepperDevice_TMC2130::set_MSLUT0(uint32_t value)
{
    write_REG(TMC_REG_MSLUT0, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT1(uint32_t value)
{
    write_REG(TMC_REG_MSLUT1, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT2(uint32_t value)
{
    write_REG(TMC_REG_MSLUT2, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT3(uint32_t value)
{
    write_REG(TMC_REG_MSLUT3, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT4(uint32_t value)
{
    write_REG(TMC_REG_MSLUT4, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT5(uint32_t value)
{
    write_REG(TMC_REG_MSLUT5, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT6(uint32_t value)
{
    write_REG(TMC_REG_MSLUT6, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_MSLUT7(uint32_t value)
{
    write_REG(TMC_REG_MSLUT7, value);
    
    return driverStatus;
}

//////////
// MSLUTSEL
//////////

uint8_t CCStepperDevice_TMC2130::set_MSLUTSEL(uint32_t value)
{
    write_REG(TMC_REG_MSLUTSEL, value);
    
    return driverStatus;
}

//////////
// MSLUTSTART
//////////

uint8_t CCStepperDevice_TMC2130::set_MSLUTSTART(uint8_t start_sin, uint8_t start_sin90)
{
    uint32_t data;
    data = ( uint32_t(start_sin90) & TMC_MSLUTSTART_START_SIN90_MASK );
    data = data<<TMC_MSLUTSTART_START_SIN90;
    data |= ( uint32_t(start_sin) & TMC_MSLUTSTART_START_SIN_MASK );
    data &= TMC_MSLUTSTART_MASK;
    
    write_REG(TMC_REG_MSLUTSTART, data);
    
    return driverStatus;
}

//////////
// MSCNT
//////////

uint16_t CCStepperDevice_TMC2130::get_MSCNT()
{
    uint32_t data;
    
    read_REG(TMC_REG_MSCNT, &data);
    
    data &= TMC_MSCNT_MASK;
    
    data = uint16_t(data);
    
    return data;
}

//////////
// MSCURACT
//////////

int32_t CCStepperDevice_TMC2130::get_MSCURACT()
{
    uint32_t data;
    
    read_REG(TMC_REG_MSCURACT, &data);
    
    data &= TMC_MSCURACT_MASK;
    
    return data;
}

//////////
// CHOPCONF
//////////

uint8_t CCStepperDevice_TMC2130::set_diss2g(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_DISS2G, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_dedge(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_DEDGE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_intpol(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_INTPOL, value);
    
    return driverStatus;
}

// setting the microstep resolution
uint8_t CCStepperDevice_TMC2130::set_mres(uint16_t value)
{
    uint8_t data = 0;
    
    switch(value){
        case 1:
            data = 8;
            break;
        case 2:
            data = 7;
            break;
        case 4:
            data = 6;
            break;
        case 8:
            data = 5;
            break;
        case 16:
            data = 4;
            break;
        case 32:
            data = 3;
            break;
        case 64:
            data = 2;
            break;
        case 128:
            data = 1;
            break;
    }
    
    set_CHOPCONF(TMC_CHOPCONF_MRES, data);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_sync(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_SYNC, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_vhighchm(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_VHIGHCHM, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_vhighfs(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_VHIGHFS, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_vsense(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_VSENSE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_tbl(uint8_t value)
{
    set_CHOPCONF(TMC_CHOPCONF_TBL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_chm(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_CHM, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_rndtf(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_RNDTF, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_disfdcc(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_DISFDCC, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_fd(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_FD, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_hend(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_HEND, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_hstrt(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_HSTRT, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_toff(uint8_t value)
{
    
    set_CHOPCONF(TMC_CHOPCONF_TOFF, value);
    
    return driverStatus;
}

//////////
// COOLCONF
//////////

// alter coolconf
uint8_t CCStepperDevice_TMC2130::alter_COOLCONF(uint32_t data, uint32_t mask)
{
    uint32_t newData;
    
    _coolconf = ( _coolconf & ~mask ) | ( data & mask );
    
    write_REG( TMC_REG_COOLCONF, _coolconf );
    
    return driverStatus;
}

// set coolconf
uint8_t CCStepperDevice_TMC2130::set_COOLCONF(uint8_t position, uint8_t value)
{
    
    alter_COOLCONF( uint32_t(value)<<position, TMC_CHOPCONF_MASKS[position]<<position);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_sfilt(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SFILT, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_sgt(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SGT, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_seimin(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SEIMIN, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_sedn(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SEDN, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_semax(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SEMAX, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_seup(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SEUP, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_semin(uint8_t value)
{
    set_CHOPCONF(TMC_COOLCONF_SEMIN, value);
    
    return driverStatus;
}

//////////
// DCCTRL
//////////

uint8_t CCStepperDevice_TMC2130::set_DCCTRL(uint16_t dc_time, uint16_t dc_sg)
{
    uint32_t data;
    data = ( uint32_t(dc_sg) & TMC_DCCTRL_DC_SG_MASK );
    data = data<<TMC_DCCTRL_DC_SG;
    data |= ( uint32_t(dc_time) & TMC_DCCTRL_DC_TIME_MASK );
    data &= TMC_DCCTRL_MASK;
    
    write_REG(TMC_REG_DCCTRL, data);
    
    return driverStatus;
}

//////////
// PWMCONF
//////////

// alter pwmconf
uint8_t CCStepperDevice_TMC2130::alter_PWMCONF(uint32_t data, uint32_t mask)
{
    uint32_t newData;
    
    _pwmconf = ( _pwmconf & ~mask ) | ( data & mask );
    
    write_REG( TMC_REG_PWMCONF, _pwmconf );
    
    return driverStatus;
}

// set pwmconf
uint8_t CCStepperDevice_TMC2130::set_PWMCONF(uint8_t position, uint8_t value)
{
    
    alter_PWMCONF( uint32_t(value)<<position, TMC_CHOPCONF_MASKS[position]<<position );
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_freewheel(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_FREEWHEEL, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_pwm_symmetric(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_PWM_SYMMETRIC, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_pwm_autoscale(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_PWM_AUTOSCALE, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_pwm_freq(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_PWM_FREQ, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_PWM_GRAD(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_PWM_GRAD, value);
    
    return driverStatus;
}

uint8_t CCStepperDevice_TMC2130::set_PWM_AMPL(uint8_t value)
{
    set_PWMCONF(TMC_PWMCONF_PWM_AMPL, value);
    
    return driverStatus;
}

//////////
// ENCM_CTRL
//////////

uint8_t CCStepperDevice_TMC2130::set_ENCM_CTRL(uint8_t value)
{
    uint8_t data;
    
    data = value & TMC_ENCM_CTRL_MASK;
    
    write_REG(TMC_REG_ENCM_CTRL, data);
    
    return driverStatus;
}

//////////
// STATUS
//////////

// check the reset status
bool CCStepperDevice_TMC2130::isReset()
{
    return driverStatus&TMC_SPISTATUS_RESET_MASK ? true : false;
}

// check the error status
bool CCStepperDevice_TMC2130::isError()
{
    return driverStatus&TMC_SPISTATUS_ERROR_MASK ? true : false;
}

// check the stallguard status
bool CCStepperDevice_TMC2130::isStallguard()
{
    return driverStatus&TMC_SPISTATUS_STALLGUARD_MASK ? true : false;
}

// check the standstill status
bool CCStepperDevice_TMC2130::isStandstill()
{
    return driverStatus&TMC_SPISTATUS_STANDSTILL_MASK ? true : false;
}

// get debug messages
String CCStepperDevice_TMC2130::debug()
{
    return debugHelp;
}










