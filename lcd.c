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

uint8_t lcd_read_byte(void) {
   uint8_t low, high;
   
   TRISA = 0xff;
   LCD_RW = 1;
   __delay_us(5);
   LCD_E = 1;
   __delay_us(5);
   high = PORTA & 0x0f;
   LCD_E = 0;
   __delay_us(5);
   LCD_E = 1;
   __delay_us(5);
   low = PORTA & 0x0f;
   LCD_E = 0;
   TRISA = 0xf;
   return( (high << 4) | low);
}


void lcd_send_nibble(uint8_t n) {
   LATA = n;
   __delay_us(5);
   LCD_E = 1;
   __delay_us(5);
   LCD_E = 0;
}


void lcd_send_byte(uint8_t address, uint8_t n) {
   LCD_RS = 0;
   while (bit_test(lcd_read_byte(), 7));
   if (address != 0) {
      LCD_RS = 1;
   }
   else {
      LCD_RS = 0;
   }
   __delay_us(5);
   LCD_RW = 0;
   __delay_us(5);
   LCD_E = 0;
   lcd_send_nibble(n >> 4);
   lcd_send_nibble(n & 0x0f);
}


void lcd_init(void) {
   char i;
   
   TRISA = 0xf0;
   LCD_RS = 0;
   LCD_RW = 0;
   LCD_E = 0;
   __delay_ms(15);
   for(i = 1; i <= 3; ++i) {
      lcd_send_nibble(3);
      __delay_ms(5);
   }
   lcd_send_nibble(2);
   for(i = 0; i <= 3; ++i) {
      lcd_send_byte(0, LCD_INIT_STRING[i]);
   }
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
   char address;
   if ((y % 2) == 0) { // y = 2 or 4
      address = LCD_LINE_TWO;
   }
   else {
      address = 0;
   }

   if ((y == 3) || (y == 4)) {
      x = x + 20;
   }
   address += x - 1;
   lcd_send_byte(0, 0x80 | address);
}

void lcd_putc(char c) {
   switch (c) {
      case '\f'   : lcd_send_byte(0, 1);
                    __delay_ms(2);
                                             break;
      case '\2'   : lcd_gotoxy(1, 2);        break;
      case '\n'   : lcd_gotoxy(1, 2);        break;
      case '\3'   : lcd_gotoxy(1, 3);        break;
      case '\4'   : lcd_gotoxy(1, 4);        break;
      case '\b'   : lcd_send_byte(0, 0x10);  break;
      default     : lcd_send_byte(1, c);     break;
   }
}

#define POINTER 0
#define ADJUST_POINTER 1
#define DOWN_ARROW 2
#define SOLID_CURSOR 3
#define UNDERLINE_CURSOR 4
