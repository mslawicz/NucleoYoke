/*
 * RotaryEncoder.h
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include "Switch.h"

enum RotaryEncoderType
{
    RET_single_slope,   // one clock slope per encoder detent
    RET_dual_slope      // two clock slopes per encoder detent (falling slope detection only)
};

class RotaryEncoder
{
public:
    RotaryEncoder(GPIO_TypeDef* clkPort, uint32_t clkPin, GPIO_TypeDef* dirPort, uint32_t dirPin, RotaryEncoderType type, uint32_t debounceTime = 10000);
    int getState(bool clear = true);
    void handler(void);
private:
    Switch clockSignal;
    Switch directionSignal;
    RotaryEncoderType type;
    int state;
};

#endif /* ROTARYENCODER_H_ */
