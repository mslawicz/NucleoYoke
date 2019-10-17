/*
 * Decoder.cpp
 *
 *  Created on: 15.10.2019
 *      Author: Marcin
 */

#include "Decoder.h"

/*
 * base decoder object constructor
 */
Decoder::Decoder(uint8_t i1Bit, uint8_t i2Bit, uint8_t o1Bit, uint8_t o2Bit) :
    i1Bit(i1Bit),
    i2Bit(i2Bit),
    o1Bit(o1Bit),
    o2Bit(o2Bit)
{
    previousExpanderData = 0;
}

/*
 * constructor of rotary encoder object
 */
RotaryEncoder::RotaryEncoder(uint8_t clkBit, uint8_t directionBit, uint8_t turnLeftBit, uint8_t turnRightBit, uint32_t& clearMask) :
        Decoder(clkBit, directionBit, turnLeftBit, turnRightBit)
{
    clearMask |= (1 << turnLeftBit);
    clearMask |= (1 << turnRightBit);
}

/*
 * generates turn-left and turn-right signals for button register from clk and direction bits in expander data
 */
bool RotaryEncoder::decode(uint16_t expanderData, uint32_t& buttons)
{
    bool changedToOne = false;

    // check if there is clk transition 0->1
    if(((expanderData & (1 << i1Bit)) != 0) &&
       ((previousExpanderData & (1 << i1Bit)) == 0))
    {
        if((expanderData & (1 << i2Bit)) != 0)
        {
            // encoder rotated right
            buttons &= ~(1 << o1Bit);
            buttons |= (1 << o2Bit);
        }
        else
        {
            // encoder rotated left
            buttons |= (1 << o1Bit);
            buttons &= ~(1 << o2Bit);
        }

        changedToOne = true;
    }
    // check if there is clk transition 1->0
    else if(((expanderData & (1 << i1Bit)) == 0) &&
       ((previousExpanderData & (1 << i1Bit)) != 0))
    {
        // clear both output signals
        buttons &= ~(1 << o1Bit);
        buttons &= ~(1 << o2Bit);
    }

    previousExpanderData = expanderData;
    return changedToOne;
}

/*
 * constructor of toggle switch object
 */
ToggleSwitch::ToggleSwitch(uint8_t inputBit, uint8_t outputBit, uint32_t& clearMask) :
        Decoder(inputBit, -1, outputBit, -1)
{
    clearMask |= (1 << outputBit);
}

/*
 * generates toggle switch signal for button register from input bit in expander data
 */
bool ToggleSwitch::decode(uint16_t expanderData, uint32_t& buttons)
{
    bool changedToOne = false;

    if(((expanderData & (1 << i1Bit)) != 0) &&
       ((previousExpanderData & (1 << i1Bit)) == 0))
    {
        // input transition 0->1
        buttons |= (1 << o1Bit);
        changedToOne = true;
    }
    else if(((expanderData & (1 << i1Bit)) == 0) &&
       ((previousExpanderData & (1 << i1Bit)) != 0))
    {
        // input transition 1->0
        buttons &= ~(1 << o1Bit);
    }

    previousExpanderData = expanderData;
    return changedToOne;
}

/*
 * constructor of direct button object
 * input level is transfered directly or inverted to output register
 */
DirectButton::DirectButton(uint8_t inputBit, uint8_t outputBit, bool inverted) :
        Decoder(inputBit, -1, outputBit, -1),
        inverted(inverted)
{
}

/*
 * generates plain button signal for button register from input bit in expander data
 * the signal can be inverted
 */
bool DirectButton::decode(uint16_t expanderData, uint32_t& buttons)
{
    if(((expanderData & (1 << i1Bit)) != 0) == inverted)
    {
        buttons &= ~(1 << o1Bit);
    }
    else
    {
        buttons |= (1 << o1Bit);
    }

    return false;
}
