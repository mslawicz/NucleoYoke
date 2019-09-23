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
    EMA();
    float getFilteredValue(float input);
private:
    float filteredValue;
    float alpha;     // filter strength parameter: 0-max filtering, 1-no filtering
    float averageDelta;
};

#endif /* FILTER_H_ */
