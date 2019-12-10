/*
 * HX711.h
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#ifndef HX711_H_
#define HX711_H_

#include "GPIO.h"
#include <vector>


class HX711
{
public:
    HX711();
private:
    GPIO sckPin;
};

#endif /* HX711_H_ */
