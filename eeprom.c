//----------------------------------------------------------------------------
// File: eeprom.c
// Author: Ian Oltuszyk
// Last Modified: 9/18/2019
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

uint8_t isInitialized; 
/*char lcdLine[16];
uint8_t lcdLineMap[3] = {
    0,
};*/

void eeprom_init() {
    /*uint16_t address = 0;*/
    
    eeprom_access(255, &isInitialized, false);
    if (isInitialized==0xFF) {
        /*uint8_t it_lcdLine = 0;
        while (it_lcdLine<(sizeof(lcdLineMap)-1)) {
            switch (it_lcdLine++) {
                case 0: strcpy(lcdLine, ""); break; 
            }
            for (int i=0; i<((lcdLineMap[it_lcdLine]-lcdLineMap[it_lcdLine-1])); i++) {
                eeprom_access(address++, (uint8_t *)&lcdLine[i], true);
            }
        }*/
        
        savedProgramState = traditional;
        eeprom_access(254, &savedProgramState, true);
        
        isInitialized = 0;
        eeprom_access(255, &isInitialized, true);
    }
}

// RE: Non-reentrant advisory
// Opt #1) Save state during main loop
// Opt #2) Single invoke by interrupt on boot
void eeprom_access(uint8_t address, uint8_t *data, bool write) {
    switch (write) {
        case true:
            do{											
                while (EECON1bits.WR) { 				
                    continue; 							
                } 										
                EEADR = address;							
                EEDATA = *data;													
                STATUSbits.CARRY = 0;					
                if (INTCONbits.GIE) {					
                    STATUSbits.CARRY = 1;				
                }										
                INTCONbits.GIE = 0;						
                EECON1bits.WREN = 1;					
                EECON2 = 0x55;							
                EECON2 = 0xAA;							
                EECON1bits.WR = 1;						
                EECON1bits.WREN = 0;					
                if (STATUSbits.CARRY) {					
                    INTCONbits.GIE = 1;					
                }										
            } while (0);
            break;
        case false:
            EEADR = address;
            EECON1bits.EEPGD = 0;
            EECON1bits.RD = 1;
            *data = EEDATA;
            break;
    }
}