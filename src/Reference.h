/*
 * Reference.h
 *
 *  Created on: 12.12.2019
 *      Author: Marcin
 */

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "Timer.h"

class Reference
{
public:
    Reference(uint32_t midValue, uint32_t referenceLimit, float referenceDeviation, uint32_t stabilityTime);
    void calculateReference(uint32_t inputValue);
    float getReference(void) const { return referenceValue; }
private:
    uint32_t midValue;              // middle (expected) value of the input range
    uint32_t referenceLimit;        // allowed limit of measured reference value
    float referenceDeviation;       // allowed deviation from measured reference value
    uint32_t stabilityTime;         // time the input must be within allowed range before the new reference is accepted
    uint32_t noOfAveragedSamples {0};   // number of averaged samples
    float averageValue {0.0f};             // samples average value being calculated
    float referenceValue;           // measured and accepted reference value
    Timer stabilityTimer;
};

#endif /* REFERENCE_H_ */
