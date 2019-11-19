/*
 * Switch.cpp
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#include "Switch.h"

Switch::Switch(GPIO_TypeDef* port, uint32_t pin, GPIO_PinState startState, uint32_t debounceTime) :
        inputPin(port, pin, GPIO_MODE_INPUT, startState == GPIO_PinState::GPIO_PIN_SET ? GPIO_PULLUP : GPIO_PULLDOWN),
        pinState(startState),
        debounceTime(debounceTime)
{
    machineState = SwitchState::SS_stable;
    hasChangedFlag = false;
    hasChangedTo0Flag = false;
    hasChangedTo1Flag = false;
}

/*
 * switch state machine handler
 * it must be called periodically
 */
void Switch::stateMachine(void)
{
    GPIO_PinState newPinState = inputPin.read();
    switch(machineState)
    {
    case SS_stable:
        if(pinState != newPinState)
        {
            // pin state switching occurred
            pinState = newPinState;
            hasChangedFlag = true;
            if(pinState == GPIO_PinState::GPIO_PIN_SET)
            {
                hasChangedTo1Flag = true;
            }
            else
            {
                hasChangedTo0Flag = true;
            }
            eventTime.reset();
            machineState = SS_debouncing;
        }
        break;
    case SS_debouncing:
        if(pinState != newPinState)
        {
            // input is not yet stable - reset timer
            eventTime.reset();
        }
        if(eventTime.elapsed(debounceTime))
        {
            // input is stable long enough - end of debouncing
            machineState = SS_stable;
        }
        break;
    default:
        machineState = SS_stable;
        break;
    }
}

/*
 * check if switch has changed its state
 * by default the change flag is cleared
 */
bool Switch::hasChanged(bool clear)
{
    stateMachine();
    bool flag = hasChangedFlag;
    if(clear)
    {
        hasChangedFlag = false;
    }
    return flag;
}

/*
 * check if switch has changed its state 0->1
 * by default the change flag is cleared
 */
bool Switch::hasChangedTo1(bool clear)
{
    stateMachine();
    bool flag = hasChangedTo1Flag;
    if(clear)
    {
        hasChangedTo1Flag = false;
    }
    return flag;
}

/*
 * check if switch has changed its state 1->0
 * by default the change flag is cleared
 */
bool Switch::hasChangedTo0(bool clear)
{
    stateMachine();
    bool flag = hasChangedTo0Flag;
    if(clear)
    {
        hasChangedTo0Flag = false;
    }
    return flag;
}

