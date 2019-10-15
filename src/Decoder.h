/*
 * Decoder.h
 *
 *  Created on: 15.10.2019
 *      Author: Marcin
 */

#ifndef DECODER_H_
#define DECODER_H_

#include "stm32f4xx_hal.h"

class RotaryEncoder
{
public:
    RotaryEncoder(uint8_t clkPosition, uint8_t directionPosition);
    int decode(uint16_t expanderData);
private:
    uint8_t clkPosition;
    uint8_t directionPosition;
    uint16_t previousData;
    int output;
};

class ToggleSwitch
{
public:
    ToggleSwitch(uint8_t inputPosition);
    int decode(uint16_t expanderData);
private:
    uint8_t inputPosition;
    uint16_t previousData;
    int output;
};

#endif /* DECODER_H_ */
