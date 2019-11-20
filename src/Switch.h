/*
 * Switch.h
 *
 *  Created on: 19.11.2019
 *      Author: Marcin
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include "GPIO.h"
#include "Timer.h"

enum SwitchState
{
    SS_stable,
    SS_debouncing
};

class Switch
{
public:
    Switch(GPIO_TypeDef* port, uint32_t pin, GPIO_PinState startState, uint32_t debounceTime = 20000);
    GPIO_PinState getState(void) { stateMachine(); return pinState; }
    bool hasChanged(bool clear = true);
    bool hasChangedTo1(bool clear = true);
    bool hasChangedTo0(bool clear = true);
    bool isStable(void) { stateMachine(); return machineState == SwitchState::SS_stable; }
private:
    void stateMachine(void);
    GPIO inputPin;      // input GPIO object
    GPIO_PinState pinState;     // the current level of the input pin
    SwitchState machineState;   // state of the state machine
    Timer eventTime;            // timer for switching time measurement
    uint32_t debounceTime;      // requested switch stability time
    bool hasChangedFlag;        // true if switch has changed its state
    bool hasChangedTo1Flag;     // true if switch has chenged 0->1
    bool hasChangedTo0Flag;     //  true if switch has changed 1->0
};

#endif /* SWITCH_H_ */