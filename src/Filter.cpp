/*
 * Filter.cpp
 *
 *  Created on: 23.09.2019
 *      Author: Marcin
 */

#include "Filter.h"
#include <cmath>

/*
 * quickness defines how fast the filter follows the input
 * faster means it's dynamically more accurate, but there's less smoothing in long term
 */
EMA::EMA(float quickness) :
    quickness (quickness)
{
}

float EMA::getFilteredValue(float input)
{
    float delta = input - filteredValue;
    averageDelta += Beta * (fabs(delta) - averageDelta);
    if(averageDelta != 0)
    {
        alpha = quickness * fabs(delta) / averageDelta;
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
