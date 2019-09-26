//----------------------------------------------------------------------------
// File: eeprom.c
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

// RB1 = execute
// RB2 = program

void __interrupt() interrupt_service_routine(void) {
    if (INTCON3bits.INT2IF) {
        program_interrupt();
        INTCON3bits.INT2IF=0;
    }
    if (INTCON3bits.INT1IF) {
        execute_interrupt();
        INTCON3bits.INT1IF = 0;
    }
    if (INTCONbits.TMR0IF) {
        timer_interrupt();
        INTCONbits.TMR0IF = 0;
    }
}

void timer_interrupt() {
    if (bootStep==1) { // fully booted
        if (traditionalStep==1 || // is flowing
                volumeStep==1 ||
                timeStep==1 || 
                cleanStep==1){
            cyclesElapsed++;
            if (cyclesElapsed==7812) {
                cyclesElapsed = 0;
                secondsElapsed++;
            }
        }
        else if (traditionalStep==0 || // is prompting
                volumeStep==0 ||
                timeStep==0 ||
                cleanStep==1 ){ // or is cleaning
            cyclesElapsed++;
            if (cyclesElapsed%8==4) {
                millisecondsElapsed++; //used for encoder and cleaning...
            }
        }
    }
}

void program_interrupt() {
    if (bootStep==1) { // fully booted
        if (!programButtonPressed) // debounce interrupt
        {
            if (traditionalStep!=1 && // not flowing
                    volumeStep!=1 &&
                    timeStep!=1 && 
                    cleanStep!=1){
                programButtonPressed = true;
                switch (savedProgramState) {
                    case boot:
                        programButtonPressed = false; // defunct
                        break;
                    case traditional:
                        savedProgramState = volume;
                        break;
                    case volume:
                        savedProgramState = time;
                        break;
                    case time:
                        savedProgramState = clean;
                        break;
                    case clean:
                        savedProgramState = traditional;
                        break;
                }
                eeprom_access(254, &savedProgramState, true);
            }
        } 
    }
}
void execute_interrupt() {
    if (bootStep==1) { // fully booted
        if (!executeButtonPressed) // debounce interrupt
        {
            executeButtonPressed = true;
            switch (savedProgramState) {
                case boot:
                    executeButtonPressed = false; // defunct
                    break;
                case traditional:
                    traditionalStep = (traditionalStep+1)%3;
                    break;
                case volume:
                    volumeStep = (volumeStep+1)%3;
                    break;
                case time:
                    timeStep = (timeStep+1)%3;
                    break;
                case clean:
                    cleanStep = (cleanStep+1)%2;
                    break;
            }
        }
    }
}
