//----------------------------------------------------------------------------
// File: eeprom.h
// Author: Ian Oltuszyk
// Last Modified: 9/18/2019
//----------------------------------------------------------------------------

#ifndef EEPROM_H
#define	EEPROM_H

void eeprom_init(void);
void eeprom_access(uint8_t address, uint8_t *data, bool write);

#endif	/* EEPROM_H */

