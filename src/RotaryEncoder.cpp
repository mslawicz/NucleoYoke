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
    if(clockSignal.hasChangedTo0(false) &&
            clockSignal.isStable() &&
            directionSignal.isStable())
    {
        // rotation detected and signals are stable
        //clear the flag
        clockSignal.hasChangedTo0();
        rotation = directionSignal.getState() == GPIO_PinState::GPIO_PIN_RESET ? 1 : -1;
    }
    return rotation;
}

