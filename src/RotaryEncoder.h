/*
 * RotaryEncoder.h
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include "Switch.h"

class RotaryEncoder
{
public:
    RotaryEncoder(GPIO_TypeDef* clkPort, uint32_t clkPin, GPIO_TypeDef* dirPort, uint32_t dirPin, uint32_t debounceTime = 20000);
    int getState(void);
private:
    Switch clockSignal;
    Switch directionSignal;
};

#endif /* ROTARYENCODER_H_ */
