//----------------------------------------------------------------------------
// File: lcds.h
// Author: Ian Oltuszyk
// Last Modified: 9/20/2019
//----------------------------------------------------------------------------

#ifndef LCD_H
#define	LCD_H

#define bit_test(D,i) (D & (0x01 << i))

#define LCD_D4    PORTAbits.RA0
#define LCD_D5    PORTAbits.RA1
#define LCD_D6    PORTAbits.RA2
#define LCD_D7    PORTAbits.RA3
#define LCD_RS    PORTBbits.RB0
#define LCD_RW    PORTBbits.RB3
#define LCD_E     PORTBbits.RB4

#define LCD_TYPE              2           // 0=5x7, 1=5x10, 2=2 lines
#define LCD_LINE_TWO          0x40        // LCD RAM address for the second line

#define POINTER 0
#define ADJUST_POINTER 1
#define DOWN_ARROW 2
#define SOLID_CURSOR 3
#define UNDERLINE_CURSOR 4

char const LCD_INIT_STRING[4] = {0x20 | (LCD_TYPE << 2), 0x0c, 1, 6};
                             // These bytes need to be sent to the LCD
                             // to start it up
uint8_t lcd_read_byte(void);
void lcd_send_nibble(uint8_t n);
void lcd_send_byte(uint8_t address, uint8_t n);
void lcd_init(void);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_putc(char c);
char lcd_getc(uint8_t x, uint8_t y);
void setup_cgram(void);

#endif	/* LCD_H */

