/*
 * Decoder.cpp
 *
 *  Created on: 15.10.2019
 *      Author: Marcin
 */

#include "Decoder.h"

RotaryEncoder::RotaryEncoder(uint8_t clkPosition, uint8_t directionPosition) :
    clkPosition(clkPosition),
    directionPosition(directionPosition)
{
    previousData = 0;
    output = 0;
}

/*
 * decodes input from clk and direction pins of a rotary encoder
 * returns -1 for left turn, 1 for right turn and 0 for no change
 */
int RotaryEncoder::decode(uint16_t expanderData)
{
    output = 0;

    if(((expanderData & (1 << clkPosition)) != 0) &&
       ((previousData & (1 << clkPosition)) == 0))
    {
        // clk transition 0->1
        output = (expanderData & (1 << directionPosition)) != 0 ? 1 : -1;
    }

    previousData = expanderData;
    return output;
}

ToggleSwitch::ToggleSwitch(uint8_t inputPosition) :
        inputPosition(inputPosition)
{
    previousData = 0;
    output = 0;
}

/*
 * decodes input from toggle switch pin
 * returns -1 for switching to off, 1 for switching to on and 0 for no change
 */
int ToggleSwitch::decode(uint16_t expanderData)
{
    if(((expanderData & (1 << inputPosition)) != 0) &&
       ((previousData & (1 << inputPosition)) == 0))
    {
        // input transition 0->1
        output = 1;
    }
    else if(((expanderData & (1 << inputPosition)) == 0) &&
       ((previousData & (1 << inputPosition)) != 0))
    {
        // input transition 1->0
        output = -1;
    }
    else
    {
        output = 0;
    }

    previousData = expanderData;
    return output;
}
