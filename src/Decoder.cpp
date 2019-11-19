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
RotaryEncoderEx::RotaryEncoderEx(uint8_t clkBit, uint8_t directionBit, uint8_t turnLeftBit, uint8_t turnRightBit, uint32_t& cleanMask) :
        Decoder(clkBit, directionBit, turnLeftBit, turnRightBit)
{
    cleanMask |= (1 << turnLeftBit);
    cleanMask |= (1 << turnRightBit);
}

/*
 * generates turn-left and turn-right signals for button register from clk and direction bits in expander data
 */
bool RotaryEncoderEx::decode(uint16_t expanderData, uint32_t& buttons)
{
    bool cleanRequest = false;

    // check if there is clk transition
    if((expanderData & (1 << i1Bit)) != (previousExpanderData & (1 << i1Bit)))
    {
        if(((expanderData >> i1Bit) ^ (expanderData >> i2Bit)) & 0x0001)
        {
            // encoder rotated left
            buttons |= (1 << o1Bit);
            buttons &= ~(1 << o2Bit);
        }
        else
        {
            // encoder rotated right
            buttons &= ~(1 << o1Bit);
            buttons |= (1 << o2Bit);
        }

        cleanRequest = true;
    }

    previousExpanderData = expanderData;
    return cleanRequest;
}

/*
 * constructor of toggle switch object
 */
ToggleSwitch::ToggleSwitch(uint8_t inputBit, uint8_t onBit, uint8_t offBit, uint32_t& cleanMask) :
        Decoder(inputBit, -1, onBit, offBit)
{
    cleanMask |= (1 << onBit);
    cleanMask |= (1 << offBit);
}

/*
 * generates toggle switch on & off signals for button register from input bit in expander data
 */
bool ToggleSwitch::decode(uint16_t expanderData, uint32_t& buttons)
{
    bool cleanRequest = false;

    if(((expanderData & (1 << i1Bit)) != 0) &&
       ((previousExpanderData & (1 << i1Bit)) == 0))
    {
        // input transition 0->1
        buttons |= (1 << o1Bit);
        buttons &= ~(1 << o2Bit);
        cleanRequest = true;
    }
    else if(((expanderData & (1 << i1Bit)) == 0) &&
       ((previousExpanderData & (1 << i1Bit)) != 0))
    {
        // input transition 1->0
        buttons |= (1 << o2Bit);
        buttons &= ~(1 << o1Bit);
        cleanRequest = true;
    }

    previousExpanderData = expanderData;
    return cleanRequest;
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
