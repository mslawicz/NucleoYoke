/*
 * Decoder.h
 *
 *  Created on: 15.10.2019
 *      Author: Marcin
 */

#ifndef DECODER_H_
#define DECODER_H_

#include "stm32f4xx_hal.h"

class Decoder
{
public:
    Decoder(uint8_t i1Bit, uint8_t i2Bit, uint8_t o1Bit, uint8_t o2Bit);
    virtual ~Decoder() {};
    virtual bool decode(uint16_t expanderData, uint32_t& buttons) = 0;
protected:
    uint8_t i1Bit;  // input 1 bit in expander data
    uint8_t i2Bit;  // input 2 bit in expander data
    uint8_t o1Bit;  // output 1 bit in yoke button register
    uint8_t o2Bit;  // output 2 bit in yoke button register
    uint16_t previousExpanderData;
};

class RotaryEncoder : public Decoder
{
public:
    RotaryEncoder(uint8_t clkBit, uint8_t directionBit, uint8_t turnLeftBit, uint8_t turnRightBit, uint32_t& cleanMask);
    bool decode(uint16_t expanderData, uint32_t& buttons);
};

class ToggleSwitch : public Decoder
{
public:
    ToggleSwitch(uint8_t inputBit, uint8_t onBit, uint8_t offBit, uint32_t& cleanMask);
    bool decode(uint16_t expanderData, uint32_t& buttons);
};

class DirectButton : public Decoder
{
public:
    DirectButton(uint8_t inputBit, uint8_t outputBit, bool inverted = false);
    bool decode(uint16_t expanderData, uint32_t& buttons);
private:
    bool inverted;
};

#endif /* DECODER_H_ */
