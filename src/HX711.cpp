/*
 * HX711.cpp
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#include "HX711.h"

HX711::HX711(GPIO_TypeDef* clockPort, uint32_t clockPin, GPIO_TypeDef* dataPort, uint32_t dataPin, uint8_t totalPulses) :
    clockSignal(clockPort, clockPin, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_MEDIUM),
    dataSignal(dataPort, dataPin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_MEDIUM),
    totalPulses(totalPulses)
{
    clockSignal.write(GPIO_PinState::GPIO_PIN_RESET);
    state = HX711State::wait_for_data_ready;
    pulseNumber = 0;
    dataBuffer = 0;
    dataRegister = 0;
}

void HX711::handler(void)
{
    switch(state)
    {
    case HX711State::wait_for_data_ready:
        if(dataSignal.read() == GPIO_PinState::GPIO_PIN_RESET)
        {
            // data is ready
            pulseNumber = 0;
            dataBuffer = 0;
            state = HX711State::clock_pulse;
        }
        break;
    case HX711State::clock_pulse:
        clockSignal.write(GPIO_PinState::GPIO_PIN_SET);
        if(pulseNumber < 24)
        {
            dataBuffer = (dataBuffer << 1) | dataSignal.read();
        }
        clockSignal.write(GPIO_PinState::GPIO_PIN_RESET);
        state = HX711State::after_pulse;
        break;
    case HX711State::after_pulse:
        if(++pulseNumber == totalPulses)
        {
            dataRegister = dataBuffer;
            state = HX711State::wait_for_data_ready;
        }
        else
        {
            state = HX711State::clock_pulse;
        }
        break;
    default:
        break;
    }
}
