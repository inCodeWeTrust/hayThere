//
//  CCStepperDevice_TMC2130.h
//  deviceScheduler
//
//  Created by Little Abakus on 30.05.14.
//  Copyright (c) 2014 Little Abakus. All rights reserved.
//

#ifndef __deviceScheduler__CCStepperDevice_TMC2130__
#define __deviceScheduler__CCStepperDevice_TMC2130__


#include "CCStepperDevice.h"

//#ifdef ARDUINO_SIMULATION
//static SPICommunication SPI;
//#else
//#include <SPI.h>
//#endif
//



#define READOUT_MICROSTEP_POSITION    0
#define READOUT_STALLGUARD_LEVEL      1
#define READOUT_CURRENT_LEVEL         2


// TMC2130 sensing resistor value in mOhm
#define R_SENSE                             150


///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class CCStepperDevice_TMC2130
///
/// @brief Device class for stepper motor devices driven by a TMC2130 stepper driver
///
///////////////////////////////////////////////////////////////////////////////////////////////////


class CCStepperDevice_TMC2130 : public CCStepperDevice {
        
public:

    CCStepperDevice_TMC2130(const String deviceName, const unsigned int step_pin, const unsigned int dir_pin, const unsigned int enable_pin, const unsigned int stepsPerRotation, const unsigned int chipSelect_pin, unsigned int currentMax);
    
    ~CCStepperDevice_TMC2130();
    
    
    /// Sets and configures the TMC 260 driver.
    /// @param stepInterpolation Enable step interpolation. 0: Disable STEP pulse interpolation, 1: Enable STEP pulse multiplication by 16.
    /// @param doubleEdgeStepPulses Enable double edge step pulses. 0: Rising step pulse edge is activee, falling edge is inactivee, 1: Both rising and falling step pulse edges are activee.
    /// @param microSteppingMode Microstep resolution for STEP/DIR mode. Microsteps per 90°: %0000: 256, %0001: 128, %0010: 64, ... %0111: 2 (halfstep), %1000: 1 (fullstep)
    void setDriverControlRegister(bool stepInterpolation, bool doubleEdgeStepPulses, unsigned int microSteppingMode);

    
    /// Sets and configures with spread cycle chopper.
    /// @param blankingTimeValue selects the comparator blank time. This time needs to safely cover the switching event and the duration of the ringing on the sense resistor. For most low current drivers, a setting of 1 or 2 is good. For high current applications with large MOSFETs, a setting of 2 or 3 will be required. 0: (min setting) … 3: (max setting)
    /// @param randomTOffTime Use random off time for noise reduction (0 for off, -1 for on).
    /// @param hysteresisStart Hysteresis start setting. Please remark, that this value is an offset to the hysteresis end value. 1 … 8
    /// @param hysteresisEnd Hysteresis end setting. Sets the hysteresis end value after a number of decrements. Decrement interval time is controlled by hysteresis_decrement. The sum hysteresis_start + hysteresis_end must be <16. At a current setting CS of max. 30 (amplitude reduced to 240), the sum is not limited.
    /// @param hysteresisDecrementPeriodValue Hysteresis decrement setting. This setting determines the slope of the hysteresis during on time and during fast decay time. 0 (fast decrement) … 3 (slow decrement).
    /// @param offTime The off time setting controls the minimum chopper frequency. For most applications an off time within the range of 5μs to 20μs will fit. Setting this parameter to zero completely disables all driver transistors and the motor can free-wheel. 0: chopper off, 1:15: off time setting
    ///
    /// The spreadCycle chopper scheme (pat.fil.) is a precise and simple to use chopper principle, which automatically determines
    /// the optimum fast decay portion for the motor. Anyhow, a number of settings can be made in order to optimally fit the driver
    /// to the motor.
    /// Each chopper cycle is comprised of an on-phase, a slow decay phase, a fast decay phase and a second slow decay phase.
    /// The slow decay phases limit the maximum chopper frequency and are important for low motor and driver power dissipation.
    /// The hysteresis start setting limits the chopper frequency by forcing the driver to introduce a minimum amount of
    /// current ripple into the motor coils. The motor inductivity determines the ability to follow a changing motor current.
    /// The duration of the on- and fast decay phase needs to cover at least the blank time, because the current comparator is
    /// disabled during this time.
    ///
    /// @sa setChopperControlRegister_fastDecay()
    ///
    void setChopperControlRegister_spreadCycle(unsigned int blankingTimeValue, bool randomTOffTime, unsigned int hysteresisDecrementPeriodValue, int hysteresisEnd, unsigned int hysteresisStart, unsigned int offTime);
    
    /// Sets and configure the classical Constant Off Timer Chopper
    /// @param blankingTimeValue Selects the comparator blank time. This time needs to safely cover the switching event and the duration of the ringing on the sense resistor. For most low current drivers, a setting of 1 or 2 is good. For high current applications with large MOSFETs, a setting of 2 or 3 will be required. 0 (min setting) … (3) amx setting
    /// @param randomTOffTime Use random off time for noise reduction (0 for off, -1 for on).
    /// @param onlyTimerTerminatesDecayPhase Selects usage of the current comparator for termination of the fast decay cycle. If current comparator is enabled, it terminates the fast decay cycle in case the current reaches a higher negative value than the actual positive value.
    /// @param sinwaveOffset Controls the sine wave offset. A positive offset corrects for zero crossing error. -3…-1: negative offset, 0: no offset,1…12: positive offset
    /// @param fastDecayTime Fast decay time setting. Controls the portion of fast decay for each chopper cycle. 0: slow decay only, 1…15: duration of fast decay phase
    /// @param offTime The off time setting controls the minimum chopper frequency. For most applications an off time within the range of 5μs to 20μs will fit. Setting this parameter to zero completely disables all driver transistors and the motor can free-wheel. 0: chopper off, 1:15: off time setting
    ///
    /// The classic constant off time chopper uses a fixed portion of fast decay following each on phase.
    /// While the duration of the on time is determined by the chopper comparator, the fast decay time needs
    /// to be set by the user in a way, that the current decay is enough for the driver to be able to follow
    /// the falling slope of the sine wave, and on the other hand it should not be too long, in order to minimize
    /// motor current ripple and power dissipation.
    /// @sa setChopperControlRegister_spreadCycle()
    void setChopperControlRegister_fastDecay(unsigned int blankingTimeValue, bool randomTOffTime, bool onlyTimerTerminatesDecayPhase, int sinwaveOffset, unsigned int fastDecayTime, unsigned int offTime);
    
    /// This method configures the CoolStep smart energy operation. You must have a proper StallGuard configuration for the motor situation (current, voltage, speed) in rder to use this feature.
    /// @param minCoolStepCurrentValue Sets the lower motor current limit for coolStepTM operation by scaling the CS value. It can be halfed or quartered.
    /// @param currentDecrementSpeedValue Sets the current decrement steps. If the StallGuard value is above the threshold the current gets decremented by this step size. 0...32
    /// @param upperCoolStepThreshold Sets the distance between the lower and the upper threshold for stallGuard2TM reading. Above the upper threshold (which is lower_SG_threshold+SG_hysteresis+1) the motor current becomes decreased. Allowed values are 0...480
    /// @param currentIncrementStepsValue Sets the current increment step. The current becomes incremented for each measured stallGuard2TM value below the lower threshold. 0...8
    /// @param lowerCoolStepThreshold Sets the lower threshold for stallGuard2TM reading. Below this value, the motor current becomes increased. Allowed values are 0...480
    /// The CoolStep smart energy operation automatically adjust the current sent into the motor according to the current load,
    /// read out by the StallGuard in order to provide the optimum torque with the minimal current consumption.
    /// You configure the CoolStep current regulator by defining upper and lower bounds of StallGuard readouts. If the readout is above the
    /// limit the current gets increased, below the limit the current gets decreased.
    /// You can specify the upper an lower threshold of the StallGuard readout in order to adjust the current. You can also set the number of
    /// StallGuard readings neccessary above or below the limit to get a more stable current adjustement.
    /// The current adjustement itself is configured by the number of steps the current gests in- or decreased and the absolut minimum current
    /// (1/2 or 1/4th otf the configured current).
    void setCoolStepRegister(bool minCoolStepCurrentValue, unsigned int currentDecrementSpeedValue, unsigned int upperCoolStepThreshold, unsigned int currentIncrementStepsValue, unsigned int lowerCoolStepThreshold);
    
    /// set the StallGuard threshold in order to get sensible StallGuard readings.
    /// @param stallGuard2Threshold -64 … 63 the StallGuard threshold
    /// @param stallGuard2FilterEnable 0 if the filter is disabled, -1 if it is enabled
    ///
    /// The StallGuard threshold is used to optimize the StallGuard reading to sensible values. It should be at 0 at
    /// the maximum allowable load on the otor (but not before). = is a good starting point (and the default)
    /// If you get Stall Gaurd readings of 0 without any load or with too little laod increase the value.
    /// If you get readings of 1023 even with load decrease the setting.
    ///
    /// If you switch on the filter the StallGuard reading is only updated each 4th full step to reduce the noise in the
    /// reading.
	void setStallGuard2Register(bool stallGuard2FilterEnable, int stallGuard2Threshold);

    void setDriverConfigurationRegister(unsigned int slopeControlHighSide, unsigned int slopeControlLowSide, bool shortToGndProtectionDisable, unsigned int shortToGndDetectionTimerValue, bool stepDirInterfaceDisable, unsigned int selectReadOut);

    
    void getReadOut(unsigned int theReadOut);
    void setCurrentScale(unsigned int currentScale);
    
    /// set the maximum motor current in mA
    /// Keep in mind this is the maximum peak Current. The RMS current will be 1/sqrt(2) smaller. The actual current can also be smaller
    /// by employing CoolStep.
    /// @param current the maximum motor current in mA
    /// @sa getCurrent(), getCurrentCurrent()
	void setCurrent(unsigned int current);
    
    
    //  TMC2130 readback:
    uint16_t getDriverState(driverStatusInfo info);

    
    uint8_t read_STAT();
    uint8_t read_REG( uint8_t address , uint32_t *data );
    uint8_t write_REG( uint8_t address, uint32_t data );
    uint8_t alter_REG(uint8_t address, uint32_t data, uint32_t mask);
    
    uint8_t set_GCONF(uint8_t bit, uint8_t value);
    uint8_t set_I_scale_analog(uint8_t value);
    uint8_t set_internal_Rsense(uint8_t value);
    uint8_t set_en_pwm_mode(uint8_t value);
    uint8_t set_enc_commutation(uint8_t value);
    uint8_t set_shaft(uint8_t value);
    uint8_t set_diag0_error(uint8_t value);
    uint8_t set_diag0_otpw(uint8_t value);
    uint8_t set_diag0_stall(uint8_t value);
    uint8_t set_diag1_stall(uint8_t value);
    uint8_t set_diag1_index(uint8_t value);
    uint8_t set_diag1_onstate(uint8_t value);
    uint8_t set_diag1_steps_skipped(uint8_t value);
    uint8_t set_diag0_int_pushpull(uint8_t value);
    uint8_t set_diag1_int_pushpull(uint8_t value);
    uint8_t set_small_hysteresis(uint8_t value);
    uint8_t set_stop_enable(uint8_t value);
    uint8_t set_direct_mode(uint8_t value);
    //uint8_t test_mode(uint8_t value);
    
    uint8_t set_IHOLD_IRUN(uint8_t ihold, uint8_t irun, uint8_t iholddelay);
    uint8_t set_TPOWERDOWN(uint8_t value);
    uint32_t get_TSTEP();
    uint8_t set_TPWMTHRS(uint32_t value);
    uint8_t set_TCOOLTHRS(uint32_t value);
    uint8_t set_THIGH(uint32_t value);
    uint8_t set_XDIRECT(uint32_t value);
    uint8_t set_XDIRECT(int16_t coil_a, int16_t coil_b);
    int32_t get_XDIRECT();
    uint8_t set_VDCMIN(int32_t value);
    
    uint8_t set_MSLUT0(uint32_t value);
    uint8_t set_MSLUT1(uint32_t value);
    uint8_t set_MSLUT2(uint32_t value);
    uint8_t set_MSLUT3(uint32_t value);
    uint8_t set_MSLUT4(uint32_t value);
    uint8_t set_MSLUT5(uint32_t value);
    uint8_t set_MSLUT6(uint32_t value);
    uint8_t set_MSLUT7(uint32_t value);
    uint8_t set_MSLUTSEL(uint32_t value);
    uint8_t set_MSLUTSTART(uint8_t start_sin, uint8_t start_sin90);
    uint16_t get_MSCNT();
    int32_t get_MSCURACT();
    
    uint8_t set_CHOPCONF(uint8_t position, uint8_t value);
    uint8_t set_dedge(uint8_t value);
    uint8_t set_diss2g(uint8_t value);
    uint8_t set_intpol(uint8_t value);
    uint8_t set_mres(uint16_t value);
    uint8_t set_sync(uint8_t value);
    uint8_t set_vhighchm(uint8_t value);
    uint8_t set_vhighfs(uint8_t value);
    uint8_t set_vsense(uint8_t value);
    uint8_t set_tbl(uint8_t value);
    uint8_t set_chm(uint8_t value);
    uint8_t set_rndtf(uint8_t value);
    uint8_t set_disfdcc(uint8_t value);
    uint8_t set_fd(uint8_t value);
    uint8_t set_hend(uint8_t value);
    uint8_t set_hstrt(uint8_t value);
    uint8_t set_toff(uint8_t value);
    
    uint8_t alter_COOLCONF(uint32_t data, uint32_t mask);
    uint8_t set_COOLCONF(uint8_t position, uint8_t value);
    uint8_t set_sfilt(uint8_t value);
    uint8_t set_sgt(uint8_t value);
    uint8_t set_seimin(uint8_t value);
    uint8_t set_sedn(uint8_t value);
    uint8_t set_semax(uint8_t value);
    uint8_t set_seup(uint8_t value);
    uint8_t set_semin(uint8_t value);
    
    uint8_t set_DCCTRL(uint16_t dc_time, uint16_t dc_sg);
    
    uint8_t alter_PWMCONF(uint32_t data, uint32_t mask);
    uint8_t set_PWMCONF(uint8_t position, uint8_t value);
    uint8_t set_freewheel(uint8_t value);
    uint8_t set_pwm_symmetric(uint8_t value);
    uint8_t set_pwm_autoscale(uint8_t value);
    uint8_t set_pwm_freq(uint8_t value);
    uint8_t set_PWM_GRAD(uint8_t value);
    uint8_t set_PWM_AMPL(uint8_t value);
    
    uint8_t set_ENCM_CTRL(uint8_t value);
    
    bool isReset();
    bool isError();
    bool isStallguard();
    bool isStandstill();
    
    bool isOverTemperatureWarning();
    String debug();

    
private:
    
    void setupMicroSteppingMode();
    void setupMicroSteppingMode(unsigned int data);
    
    const unsigned int chipSelect_pin;
    

    unsigned int currentMax;        //  current in mA
    
    unsigned long driverControl;
    unsigned long chopperControl;
    unsigned long coolStepControl;
    unsigned long stallGuard2Control;
    unsigned long driverConfiguration;
    
    unsigned int currentScaleOf32;
    bool senseResistorVoltage165mV;
    unsigned long resultDatagram;
    
    void calculateCurrentSetup(unsigned int current);
	
    void doTransaction(unsigned long datagram);
    
    
    
    uint32_t _coolconf;
    uint32_t _pwmconf;
    uint8_t driverStatus;
    uint32_t extendedDriverStatus;
    String debugHelp;

};























#endif // defined(__deviceScheduler__CCStepperDevice_TMC2130__)
