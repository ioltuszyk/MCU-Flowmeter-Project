//----------------------------------------------------------------------------
// File: states.h
// Author: Ian Oltuszyk
// Last Modified: 9/19/2019
//----------------------------------------------------------------------------

#ifndef STATES_H
#define	STATES_H

uint8_t savedProgramState;

uint8_t bootStep = 0; // 0 = booting, 1 = booted
uint8_t traditionalStep = 0; // 0 = prompt, 1 = flowing, 2 = results
uint8_t volumeStep = 0; // 0 = prompt, 1 = flowing, 2 = results
uint8_t timeStep = 0; // 0 = prompt, 1 = flowing, 2 = results
uint8_t cleanStep = 0; // 0 = prompt, 1 = flowing

typedef enum EReturn { step, next } EReturn;
EReturn boot_state(void);
EReturn traditional_state(void);
EReturn volume_state(void);
EReturn time_state(void);
EReturn clean_state(void);
EReturn end_state(void);

typedef enum EState { 
    boot, 
    traditional,
    volume, 
    time, 
    clean, 
    end 
} EState;
EReturn (*states[])(void) = {
    boot_state,
    traditional_state,
    volume_state,
    time_state,
    clean_state,
    end_state
};

uint8_t stateTransitions[][2] = {
    /* state -> {step, next} */
    [boot] = {boot, traditional},
    [traditional] = {traditional, volume},
    [volume] = {volume, time},
    [time] = {time, clean},
    [clean] = {clean, traditional}
};

#endif	/* STATES_H */

