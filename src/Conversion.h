/*
 * Conversion.h
 *
 *  Created on: 17.09.2019
 *      Author: Marcin
 */

#ifndef CONVERSION_H_
#define CONVERSION_H_

#define LOBYTE(x)  ((uint8_t)(x & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00U) >> 8U))

struct FloatVector
{
    float X;
    float Y;
    float Z;
};

// scale input value in the given type T input range to output value in the given output range
template<typename T> int16_t scaleValue(T inputMin, T inputMax, int16_t outputMin, int16_t outputMax, T inputValue)
{
    auto outputValue = static_cast<int16_t>(1.0f * (inputValue - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
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
