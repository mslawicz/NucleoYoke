/*
 * Conversion.h
 *
 *  Created on: 17.09.2019
 *      Author: Marcin
 */

#ifndef CONVERSION_H_
#define CONVERSION_H_

#include <string>

#define LOBYTE(x)  ((uint8_t)(x & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00U) >> 8U))

struct FloatVector
{
    float X;
    float Y;
    float Z;
};

std::string toHex(uint32_t value, uint8_t positions = 0, bool prefix = true);

// scale input value in the given type T input range to output value in the given output range
template<typename Ti, typename To> To scaleValue(Ti inputMin, Ti inputMax, To outputMin, To outputMax, Ti inputValue)
{
    auto outputValue = static_cast<To>(1.0f * (inputValue - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
    if(outputValue > outputMax)
    {
        outputValue = outputMax;
    }
    else if(outputValue < outputMin)
    {
        outputValue = outputMin;
    }
    return outputValue;
}

#endif /* CONVERSION_H_ */
