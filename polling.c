//----------------------------------------------------------------------------
// File: polling.c
// Author: Ian Oltuszyk
// Last Modified: 9/20/2019
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementation-Specific Library Headers
#include <xc.h>
#include <stdint.h>
#include <string.h>

// User-Defined Headers
#include "settings.h"
#include "states.h"
#include "utility.h"
#include "interrupts.h"
#include "eeprom.h"
#include "polling.h"
#include "lcd.h"
//----------------------------------------------------------------------------

// RA4 = flow sensor
// RA6 = encoder b
// RA7 = encoder a

void poll_encoder() {
    uint8_t currentEncoder = PORTAbits.RA7 + (PORTAbits.RA6<<1);
    if (currentEncoder!=prevEncoder){
        uint8_t changeInPosition = (encoderMap[currentEncoder]-encoderMap[prevEncoder])&3;
        if (changeInPosition == 1) { // CW
            encoderPulses--;
            if (millisecondsElapsed<100) {
                encoderPulses--;
            }
            millisecondsElapsed = 0;
        }
        else if (changeInPosition == 3) { // CCW
            encoderPulses++;
            if (millisecondsElapsed<100) {
                encoderPulses++;
            }
            millisecondsElapsed = 0;
        }
        prevEncoder = currentEncoder;
    }
}

void poll_flow_sensor() {
    uint8_t currentFlowSensor = PORTAbits.RA1;
    if (prevFlowSensor == 0 && currentFlowSensor) {
        prevFlowSensor = 1; // waiting for pulse
    }
    else if (prevFlowSensor == 1 && !currentFlowSensor) {
        flowSensorPulses++;
        prevFlowSensor = 0;
    }
}