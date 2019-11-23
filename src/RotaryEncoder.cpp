/*
 * RotaryEncoder.cpp
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(GPIO_TypeDef* clkPort, uint32_t clkPin, GPIO_TypeDef* dirPort, uint32_t dirPin, RotaryEncoderType type, uint32_t debounceTime) :
    clockSignal(clkPort, clkPin, GPIO_PinState::GPIO_PIN_SET, debounceTime),
    directionSignal(dirPort, dirPin, GPIO_PinState::GPIO_PIN_SET, debounceTime),
    type(type)
{
    // update signal state and clear flags
    clockSignal.hasChanged();
    clockSignal.hasChangedTo0();
    directionSignal.hasChanged();
    state = 0;
}

/*
 * get current state of rotary encoder (one shot - rotated value reported only once)
 * return value:
 * -1 rotated left
 * 0 no change
 * 1 rotated right
 */
int RotaryEncoder::getState(bool clear)
{
    int currentState = state;
    if(clear)
    {
        state = 0;
    }
    return currentState;
}

/*
 * encoder to be called frequently in a loop
 */
void RotaryEncoder::handler(void)
{
    int direction = directionSignal.getState();
    switch(type)
    {
    case RET_single_slope:
        if(clockSignal.hasChanged())
        {
            // clock signal state change detected
            state = (direction == clockSignal.getState() ? -1 : 1);
        }
        break;
    case RET_dual_slope:
        if(clockSignal.hasChangedTo0())
        {
            // clock signal falling slope detected
            state = (direction == GPIO_PinState::GPIO_PIN_RESET ? -1 : 1);
        }
        break;
    default:
        break;
    }
}
