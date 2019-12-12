/*
 * Reference.cpp
 *
 *  Created on: 12.12.2019
 *      Author: Marcin
 */

#include "Reference.h"

Reference::Reference(uint32_t midValue, uint32_t referenceLimit, float referenceDeviation, uint32_t stabilityTime) :
    midValue(midValue),
    referenceLimit(referenceLimit),
    referenceDeviation(referenceDeviation),
    stabilityTime(stabilityTime)
{
    noOfAveragedSamples = 0;
    averageValue = 0.0f;
    referenceValue = static_cast<float>(midValue);
}

// calculates reference according to input values
void Reference::calculateReference(uint32_t inputValue)
{
    if ((inputValue > midValue - referenceLimit) &&
        (inputValue < midValue + referenceLimit))
    {
        // input value in the allowed range of reference value
        if (++noOfAveragedSamples == 1)
        {
            // the first valid sample
            averageValue = static_cast<float>(inputValue);
            stabilityTimer.reset();
        }

        if (fabs(inputValue - averageValue) < referenceDeviation)
        {
            // input value in allowed deviation reange
            averageValue = (averageValue * (noOfAveragedSamples - 1) + inputValue) / noOfAveragedSamples;
        }
        else
        {
            // input value out of allowed deviation range
            noOfAveragedSamples = 0;
        }

        if (stabilityTimer.elapsed(stabilityTime))   // stable state for enough time
        {
            referenceValue = averageValue;
        }
    }
    else
    {
        // input value out of allowed range
        noOfAveragedSamples = 0;
    }
}
