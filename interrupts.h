//----------------------------------------------------------------------------
// File: interrupts.h
// Author: Ian Oltuszyk
// Last Modified: 9/20/2019
//----------------------------------------------------------------------------

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

bool programButtonPressed = false;
bool executeButtonPressed = false;
uint16_t cyclesElapsed = 0;
uint8_t secondsElapsed = 0;
uint16_t millisecondsElapsed = 0;

void timer_interrupt(void);
void program_interrupt(void);
void execute_interrupt(void);

#endif	/* INTERRUPTS_H */

