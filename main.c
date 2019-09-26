//----------------------------------------------------------------------------
// File: main.c
// Author: Ian Oltuszyk
// Last Modified: 9/16/2019
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

void main(void) {
    eeprom_init();
    lcd_init();
    
    EReturn returnCode;
    EReturn (*stateFunction)(void);
    EState currentState = boot;

    while (1) {
        stateFunction = states[currentState];
        returnCode = stateFunction();
        if (currentState == end) {
            break;
        }
        currentState = stateTransitions[currentState][returnCode];
    }
}
