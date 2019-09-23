/*
 * Filter.cpp
 *
 *  Created on: 23.09.2019
 *      Author: Marcin
 */

#include "Filter.h"
#include <cmath>

EMA::EMA()
{
    filteredValue = 0.0f;
    alpha = 0.02f;
    averageDelta = 0.0f;
}

float EMA::getFilteredValue(float input)
{
    float delta = input - filteredValue;
    averageDelta = 0.98 * averageDelta + 0.02 * fabs(delta);
    if(averageDelta != 0)
    {
        alpha = 0.2f * fabs(delta) / averageDelta;
    }
    else
    {
        alpha = 1.0f;
    }
    if(alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    filteredValue += alpha * delta;
    return filteredValue;
}
