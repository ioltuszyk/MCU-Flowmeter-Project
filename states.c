//----------------------------------------------------------------------------
// File: states.c
// Author: Ian Oltuszyk
// Last Modified: 9/20/2019
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementation-Specific Library Headers
#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// User-Defined Headers
#include "settings.h"
#include "states.h"
#include "utility.h"
#include "interrupts.h"
#include "eeprom.h"
#include "polling.h"
#include "lcd.h"
//----------------------------------------------------------------------------

EReturn boot_state(void) {
    switch (bootStep) {
        case 0:
            OSCCONbits.IRCF = 7; /* Internal Oscillator Frequency -> 8MHz */
            
            ADCON1bits.PCFG0 = 1; /* ANS0 -> RA0 */
            ADCON1bits.PCFG1 = 1; /* ANS1 -> RA1 */
            ADCON1bits.PCFG2 = 1; /* ANS2 -> RA2 */
            ADCON1bits.PCFG3 = 1; /* ANS3 -> RA3 */
            ADCON1bits.PCFG4 = 1; /* ANS4 -> RB0 */
            ADCON1bits.PCFG5 = 1; /* ANS5 -> RB1 */
            ADCON1bits.PCFG6 = 1; /* ANS6 -> RB4 */
            
            TRISAbits.RA0 = 0; /* LCD D4; RA0 -> Output */
            TRISAbits.RA1 = 0; /* LCD D5; RA1 -> Output */
            TRISAbits.RA2 = 0; /* LCD D6; RA2 -> Output */
            TRISAbits.RA3 = 0; /* LCD D7; RA3 -> Output */
            TRISAbits.RA4 = 1; /* Flow Sensor; RA4 -> Input */
            // MCLRE = OFF; Unused; RA5 -> Input
            TRISAbits.RA6 = 1; /* Encoder B; RA6 -> Input */
            TRISAbits.RA7 = 1; /* Encoder A; RA7 -> Input */
            
            TRISBbits.RB0 = 0; /* LCD RS; RB0 -> Output */
            TRISBbits.RB1 = 1; /* Execute Button; RB1 -> Input */
            TRISBbits.RB2 = 1; /* Program Button; RB2 -> Input */
            TRISBbits.RB3 = 0; /* LCD R/W; RB3 -> Output */
            TRISBbits.RB4 = 0; /* LCD E; RB4 -> Output */
            TRISBbits.RB5 = 0; /* Solenoid; RB5 -> Output */
            TRISBbits.RB6 = 0; /* Unused; RB6 -> Output */
            TRISBbits.RB7 = 0; /* Unused; RB7 -> Output */
            
            INTCONbits.GIE=1;
            INTCONbits.TMR0IF=1;
            INTCONbits.TMR0IE=1;
            INTCON2bits.RBPU=0;
            INTCON2bits.INTEDG2=1;
            INTCON2bits.INTEDG2=1;
            INTCON3bits.INT1IF=0;
            INTCON3bits.INT2IF=0;
            INTCON3bits.INT1IE=1;
            INTCON3bits.INT2IE=1;
            
            T0CONbits.TMR0ON = 1;
            T0CONbits.T08BIT = 1;
            T0CONbits.T0CS = 0;
            T0CONbits.PSA = 1;
            
            eeprom_access(254, &savedProgramState, false);

            lcd_putc("\fCHUG-METER V1.0\nUse Responsibly");
            
            bootStep++;
            return next;
    }
    return step;
}

EReturn traditional_state(void) { 
    if (savedProgramState==traditional) {
        switch (traditionalStep) {
            case 0:
                lcd_putc("\fSTRAIGHT CHUG\nNo rules...");
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                }
                break;
            case 1:
                prevEncoder = PORTAbits.RA7 + (PORTAbits.RA6<<1);
                cyclesElapsed = 0;
                secondsElapsed = 0;
                flowSensorPulses = 0;
                
                uint16_t avgRate0 = 0; // tenths
                uint16_t avgRate1 = 0; // hundredths
                
                LATBbits.LATB5 = 1;
                while (1) {
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    poll_flow_sensor();
                    
                    avgRate0 = (flowSensorPulses/secondsElapsed)*225/10000;
                    avgRate1 = (flowSensorPulses/secondsElapsed)*225%10000/1000;
                    
                    lcd_putc("\f");
                    lcd_putc(flowSensorPulses / 444 + '0');
                    lcd_putc(".");
                    lcd_putc(flowSensorPulses % 444 / 44 + '0');
                    lcd_putc(flowSensorPulses % 444 % 44 / 4 + '0');
                    lcd_putc("L in ");
                    lcd_putc(secondsElapsed / 100 + '0');
                    lcd_putc(secondsElapsed % 100 / 10 + '0');
                    lcd_putc(secondsElapsed % 100 % 10 + '0');
                    lcd_putc("S\nAvg rate: .");
                    lcd_putc((uint8_t)avgRate0 + '0');
                    lcd_putc((uint8_t)avgRate1 + '0');
                    lcd_putc("L/S");
                }
                LATBbits.LATB5 = 0;
                break;
            case 2:
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                }
                break;
        }
    }
    else {
        traditionalStep = 0;
        return next;
    }
    
    return step;
}
EReturn volume_state(void) {
    if (savedProgramState==volume) {
        switch (volumeStep) {
            case 0:
                cyclesElapsed = 0;
                millisecondsElapsed = 0;
                encoderPulses = 0;
                
                lcd_putc("\fHOW MUCH?\nVolume: 0.35L");
                
                uint16_t volume = 35;
                
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    poll_encoder();
                    if ((volume>0 && encoderPulses<0) || (volume < 999 && encoderPulses>0)) {
                        volume+=encoderPulses;
                        lcd_putc("\fHOW MUCH?\nVolume: ");
                        lcd_putc(volume/100 + '0');
                        lcd_putc(".");
                        lcd_putc(volume%100/10 + '0');
                        lcd_putc(volume%100%10 + '0');
                        lcd_putc("L");
                    }
                };
                break;
            case 1:
                cyclesElapsed = 0;
                secondsElapsed = 0;
                flowSensorPulses = 0;
                
                lcd_putc("\fChugging!...");
                LATBbits.LATB5 = 1;
                while (1) {
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    if (flowSensorPulses*100/444 >= volume) { volumeStep=(volumeStep+1)%3; break; }
                    poll_flow_sensor();
                }
                LATBbits.LATB5 = 0;
                break;
            case 2:
                lcd_putc("\fOnly took ");
                lcd_putc(secondsElapsed / 100 + '0');
                lcd_putc(secondsElapsed % 100 / 10 + '0');
                lcd_putc(secondsElapsed % 100 % 10 + '0');
                lcd_putc("S\nAre you proud?");
                
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                }
                break;
        }
    }
    else {
        volumeStep = 0;
        return next;
    }
    return step;
}

EReturn time_state(void) {
    if (savedProgramState==time) {
        switch (timeStep) {
            case 0:
                cyclesElapsed = 0;
                millisecondsElapsed = 0;
                encoderPulses = 0;
                
                lcd_putc("\fHOW LONG?\nTime: 005S");
                
                uint16_t time = 5;
                
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    poll_encoder();
                    if ((time>0 && encoderPulses<0) || (time < 999 && encoderPulses>0)) {
                        time+=encoderPulses;
                        lcd_putc("\fHOW LONG?\nTime: ");
                        lcd_putc(time/100 + '0');
                        lcd_putc(time%100/10 + '0');
                        lcd_putc(time%100%10 + '0');
                        lcd_putc("S");
                    }
                };
                break;
            case 1:
                cyclesElapsed = 0;
                secondsElapsed = 0;
                flowSensorPulses = 0;
                
                lcd_putc("\fChugging!...");
                LATBbits.LATB5 = 1;
                while (1) {
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    if (secondsElapsed>=time) { timeStep=(timeStep+1)%3; break; }
                    poll_flow_sensor();
                }
                LATBbits.LATB5 = 0;
                break;
            case 2:
                lcd_putc("\fYou downed ");
                lcd_putc(flowSensorPulses / 444 + '0');
                lcd_putc(".");
                lcd_putc(flowSensorPulses % 444 / 44 + '0');
                lcd_putc(flowSensorPulses % 444 % 44 / 4 + '0');
                lcd_putc("S\nAre you proud?");
                
                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                }
                break;
        }
    }
    else {
        timeStep = 0;
        return next;
    }
    return step;
}

EReturn clean_state(void) {
    if (savedProgramState==clean) {
        switch (cleanStep) {
            case 0: 
                lcd_putc("\fCLEANING\nOr annoyance...");

                while (1) {
                    if (programButtonPressed) { programButtonPressed = false; break; }
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                };
                break;
            case 1:
                lcd_putc("\fPlease flush\nwith hot water");
                cyclesElapsed = 0;
                millisecondsElapsed = 0;
                while (1) {
                    if (executeButtonPressed) { executeButtonPressed = false; break; }
                    if (millisecondsElapsed<250) { 
                        //open
                        LATBbits.LATB5 = 1;
                    }
                    else if (millisecondsElapsed<375) {
                        //closed
                        LATBbits.LATB5 = 0;
                    }
                    else {
                        //repeat
                        millisecondsElapsed = 0;
                    }
                }
                LATBbits.LATB5 = 0;
                break;
        }
    }
    else {
        cleanStep = 0;
        return next;
    }
    return step;
}

EReturn end_state(void) {
    return 0; // Not yet implemented
}

