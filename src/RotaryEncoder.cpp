/*
 * RotaryEncoder.cpp
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(GPIO_TypeDef* clkPort, uint32_t clkPin, GPIO_TypeDef* dirPort, uint32_t dirPin, uint32_t debounceTime) :
    clockSignal(clkPort, clkPin, GPIO_PinState::GPIO_PIN_SET, debounceTime),
    directionSignal(dirPort, dirPin, GPIO_PinState::GPIO_PIN_SET, debounceTime)
{
    // update signal state and clear flags
    clockSignal.hasChanged();
    directionSignal.hasChanged();
}

/*
 * get current state of rotary encoder (one shot - rotated value reported only once)
 * return value:
 * -1 rotated left
 * 0 no change
 * 1 rotated right
 */
int RotaryEncoder::getState(void)
{
    int rotation = 0;
    if(clockSignal.hasChanged())
    {
        // rotation detected
        if(directionSignal.getState() == clockSignal.getState())
        {
            rotation = 1;
        }
        else
        {
            rotation = -1;
        }
    }
    return rotation;
}

