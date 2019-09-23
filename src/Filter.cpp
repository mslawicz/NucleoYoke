/*
 * Filter.cpp
 *
 *  Created on: 23.09.2019
 *      Author: Marcin
 */

#include "Filter.h"

EMA::EMA()
{
    filteredValue = 0.0f;
    strength = 0.98f;
}

float EMA::getFilteredValue(float input)
{
    filteredValue = strength * filteredValue + (1-strength) * input;
    return filteredValue;
}
