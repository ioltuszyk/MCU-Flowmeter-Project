//----------------------------------------------------------------------------
// File: polling.h
// Author: Ian Oltuszyk
// Last Modified: 9/20/2019
//----------------------------------------------------------------------------
#ifndef POLLING_H
#define	POLLING_H

int8_t encoderPulses = 0;

uint8_t prevFlowSensor = false;
uint16_t flowSensorPulses = 0;

uint8_t prevEncoder;
uint8_t encoderMap[] = {
    [0] = 0, // AB off
    [1] = 1, // A on
    [2] = 3, // B on
    [3] = 2  // AB on
};

void poll_encoder(void);
void poll_flow_sensor(void);

#endif	/* POLLING_H */

