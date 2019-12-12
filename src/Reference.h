/*
 * Reference.h
 *
 *  Created on: 12.12.2019
 *      Author: Marcin
 */

#ifndef REFERENCE_H_
#define REFERENCE_H_

class Reference
{
public:
    Reference(uint32_t midValue, uint32_t referenceLimit, float referenceDeviation);
    void calculateReference(uint32_t inputValue);
    float getReference(void) const { return referenceValue; }
private:
    uint32_t midValue;              // middle (expected) value of the input range
    uint32_t referenceLimit;        // allowed limit of measured reference value
    float referenceDeviation;       // allowed deviation from measured reference value
    uint32_t noOfAveragedSamples;   // number of averaged samples
    float averageValue;             // samples average value being calculated
    float referenceValue;           // measured and accepted reference value
};

#endif /* REFERENCE_H_ */
