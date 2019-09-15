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
    resetPin(SH1106_RESET_PORT, SH1106_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_LOW),
    displayBuffer{},
    refreshRange{}
{
    state = DisplayControllerState::DCS_start;
    refreshRequest = false;
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
        for(uint8_t page = 0; page < NoOfPages; page++)
        {
            // declare the whole row to be refreshed
            refreshRange[page][0] = 2;
            refreshRange[page][1] = 129;
        }
        refreshDisplay();
        state = DCS_display_on;
        break;
    case DCS_display_on:
        sendRequest(std::vector<uint8_t>{0xAF}, true);   //display on
        state = DCS_wait_after_init;
        break;
    case DCS_wait_after_init:
        if(controllerTimer.elapsed(WaitAfterInitTime))
        {
            state = DCS_refresh_loop;
        }
        break;
    case DCS_refresh_loop:
        if(refreshRequest)
        {
            refreshRequest = false;
            refreshDisplay();
        }
        break;
    default:
        break;
    }
}

/*
 * updates display according to refreshRange array content
 */
void SH1106::refreshDisplay(void)
{
    for(uint8_t page = 0; page < NoOfPages; page++)
    {
        if(refreshRange[page][0] && refreshRange[page][1] && (refreshRange[page][1] >= refreshRange[page][0]))
        {
            std::vector<uint8_t> coordinateData =
            {
                    static_cast<uint8_t>(refreshRange[page][0] & 0x0F),     // lower part of column value
                    static_cast<uint8_t>(0x10 | ((refreshRange[page][0] >> 4) & 0x0F)),     // higher part of column value
                    static_cast<uint8_t>(0xB0 | page)       // page value
            };
            sendRequest(coordinateData, true);
            std::vector<uint8_t> data(&displayBuffer[page][refreshRange[page][0]], &displayBuffer[page][refreshRange[page][1]+1]);
            sendRequest(data);
            // clear range to mark the update has been done
            refreshRange[page][0] = refreshRange[page][1] = 0;
        }
    }
}
