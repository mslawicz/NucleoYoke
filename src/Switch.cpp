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
    machineState = SwitchState::stable;
    hasChangedFlag = false;
    hasChangedTo0Flag = false;
    hasChangedTo1Flag = false;
    doubleChangedTo0Flag = false;
    doubleChangedTo1Flag = false;
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
    case SwitchState::stable:
        if(pinState != newPinState)
        {
            // pin state switching occurred
            pinState = newPinState;
            hasChangedFlag = true;
            if(pinState == GPIO_PinState::GPIO_PIN_SET)
            {
                hasChangedTo1Flag = true;
                if(!changeTo1Time.elapsed(DoubleChangeTime))
                {
                    // next change shortly after the previous one
                    doubleChangedTo1Flag = true;
                }
                changeTo1Time.reset();
            }
            else
            {
                hasChangedTo0Flag = true;
                if(!changeTo0Time.elapsed(DoubleChangeTime))
                {
                    // next change shortly after the previous one
                    doubleChangedTo0Flag = true;
                }
                changeTo0Time.reset();
            }
            eventTime.reset();
            machineState = SwitchState::debouncing;
        }
        break;
    case SwitchState::debouncing:
        if(pinState != newPinState)
        {
            // input is not yet stable - reset timer
            eventTime.reset();
        }
        if(eventTime.elapsed(debounceTime))
        {
            // input is stable long enough - end of debouncing
            machineState = SwitchState::stable;
        }
        break;
    default:
        machineState = SwitchState::stable;
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


/*
 * check if switch has changed its state 0->1 two times in short time
 * by default the change flag is cleared
 */
bool Switch::doubleChangedTo1(bool clear)
{
    stateMachine();
    bool flag = doubleChangedTo1Flag;
    if(clear)
    {
        doubleChangedTo1Flag = false;
    }
    return flag;
}

/*
 * check if switch has changed its state 1->0 two times in short time
 * by default the change flag is cleared
 */
bool Switch::doubleChangedTo0(bool clear)
{
    stateMachine();
    bool flag = doubleChangedTo0Flag;
    if(clear)
    {
        doubleChangedTo0Flag = false;
    }
    return flag;
}
