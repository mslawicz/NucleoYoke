/*
 * Filter.h
 *
 *  Created on: 23.09.2019
 *      Author: Marcin
 */

#ifndef FILTER_H_
#define FILTER_H_

class EMA
{
public:
    EMA(float quickness = 0.2f);
    float getFilteredValue(float input);
private:
    float filteredValue;
    float alpha;     // filter smoothing parameter: 0-max filtering, 1-no filtering
    const float Beta = 0.02f;   // average value smoothing factor
    float averageDelta;     // average value of delta
    float quickness;     //
};

#endif /* FILTER_H_ */
