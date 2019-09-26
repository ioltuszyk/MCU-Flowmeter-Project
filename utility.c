//----------------------------------------------------------------------------
// File: utility.c
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

void variable_delay_us(uint16_t microseconds)
{
   while(microseconds-- > 0)
   {
       __delay_us(1);
   }
}

void variable_delay_ms(uint16_t milliseconds)
{
   while(milliseconds-- > 0)
   {
       __delay_ms(1);
   }
}