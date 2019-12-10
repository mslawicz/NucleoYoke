/*
 * HX711.cpp
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#include "HX711.h"

HX711::HX711() :
    sckSignal(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_MEDIUM)
{
    boards.push_back(HX711board{GPIO(GPIOF, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_MEDIUM), 0});  // yoke pitch tension converter board
    sckSignal.write(GPIO_PinState::GPIO_PIN_RESET);
    state = HX711State::HXS_wait_for_data_ready;
}

void HX711::handler(void)
{
    bool ready = true;
    uint8_t bitNumber;
    switch(state)
    {
    case HXS_wait_for_data_ready:
        for(auto board : boards)
        {
            if(board.dOut.read() == GPIO_PinState::GPIO_PIN_SET)
            {
                ready = false;
                break;
            }
        }
        if(ready)
        {
            // all boards are ready
            bitNumber = 0;
            state = HXS_read_bit;
        }
        break;
    case HXS_read_bit:
        break;
    default:
        break;
    }
}
