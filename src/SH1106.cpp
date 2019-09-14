/*
 * SH1106.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "SH1106.h"
#include "Timer.h" //XXX
#include "System.h" //XXX

SH1106::SH1106(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS) :
    SpiDevice(pBus, portCS, pinCS),
    resetPin(SH1106_RESET_PORT, SH1106_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_LOW)
{
    state = DisplayControllerState::DCS_start;
}

void SH1106::test(void)
{
    static Timer tm;//XXX
    if(tm.elapsed(10000)) //XXX
    {
        tm.reset();
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET);    //XXX
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_RESET);    //XXX
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET);    //XXX

        sendRequest(std::vector<uint8_t>{10,11,12}, true);
        sendRequest(std::vector<uint8_t>{1,2,3,4,5,6,7,8});
    }
}

/*
 * SC1106 controller handler
 * state machine for initializing and display refreshing
 */
void SH1106::handler(void)
{
    switch(state)
    {
    case DCS_start:
        resetPin.write(GPIO_PinState::GPIO_PIN_RESET);
        //send a dummy byte to get SPI ready
        sendRequest(std::vector<uint8_t>{0x00}, true);
        state = DCS_reset_off;
        break;
    case DCS_reset_off:
        //wait for end of transmission
        if(!pBus->isBusy())
        {
            resetPin.write(GPIO_PinState::GPIO_PIN_SET);
            controllerTimer.reset();
            state = DCS_wait_before_init;
        }
        break;
    case DCS_wait_before_init:
        if(controllerTimer.elapsed(WaitBeforeInitTime))
        {
            state = DCS_initialize;
        }
        break;
    case DCS_initialize:
        //send the initialization data
        sendRequest(SH1106InitData, true);
        controllerTimer.reset();
        state = DCS_clear_screen;
        break;
    case DCS_clear_screen:
        {
            std::vector<uint8_t> pageOfZeroes(128, 0);
            for(uint8_t page = 0; page < NoOfPages; page++)
            {
                sendRequest(std::vector<uint8_t>{0x02, 0x10, static_cast<uint8_t>(0xB0 | page)}, true);
                sendRequest(pageOfZeroes);
            }
        }
        state = DCS_display_on;
        break;
    case DCS_display_on:
        sendRequest(std::vector<uint8_t>{0xAF}, true);   //display on
        state = DCS_wait_after_init;
        break;
    case DCS_wait_after_init:
        if(controllerTimer.elapsed(WaitAfterInitTime))
        {
            state = DCS_send_loop;
        }
        break;
    case DCS_send_loop:

        break;
    default:
        break;
    }
}
