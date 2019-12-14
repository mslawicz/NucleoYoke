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
    resetPin(SH1106_RESET_PORT, SH1106_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW),
    displayBuffer{},
    refreshRange{}
{
    state = DisplayControllerState::start;
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
    case DisplayControllerState::start:
        resetPin.write(GPIO_PinState::GPIO_PIN_RESET);
        //send a dummy byte to get SPI ready
        sendRequest(std::vector<uint8_t>{0x00}, true);
        state = DisplayControllerState::reset_off;
        break;
    case DisplayControllerState::reset_off:
        //wait for end of transmission
        if(!pBus->isBusy())
        {
            resetPin.write(GPIO_PinState::GPIO_PIN_SET);
            controllerTimer.reset();
            state = DisplayControllerState::wait_before_init;
        }
        break;
    case DisplayControllerState::wait_before_init:
        if(controllerTimer.elapsed(WaitBeforeInitTime))
        {
            state = DisplayControllerState::initialize;
        }
        break;
    case DisplayControllerState::initialize:
        //send the initialization data
        sendRequest(SH1106InitData, true);
        controllerTimer.reset();
        state = DisplayControllerState::clear_screen;
        break;
    case DisplayControllerState::clear_screen:
        for(uint8_t page = 0; page < NoOfPages; page++)
        {
            // declare the whole row to be refreshed
            refreshRange[page][0] = 2;
            refreshRange[page][1] = 129;
        }
        refreshDisplay();
        state = DisplayControllerState::display_on;
        break;
    case DisplayControllerState::display_on:
        sendRequest(std::vector<uint8_t>{0xAF}, true);   //display on
        state = DisplayControllerState::wait_after_init;
        break;
    case DisplayControllerState::wait_after_init:
        if(controllerTimer.elapsed(WaitAfterInitTime))
        {
            state = DisplayControllerState::refresh_loop;
        }
        break;
    case DisplayControllerState::refresh_loop:
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
            // refresh range a..b causes sending data from displaBuffer indexes a-2...b-2
            std::vector<uint8_t> data(&displayBuffer[page][refreshRange[page][0]-2], &displayBuffer[page][refreshRange[page][1]-1]);
            sendRequest(data);
            // clear range to mark the update has been done
            refreshRange[page][0] = refreshRange[page][1] = 0;
        }
    }
}

/*
 * sets (set==true) or clears (set==false) point in X,Y coordinates
 * if refresh==true the display will be refreshed soon
 */
void SH1106::setPoint(uint8_t X, uint8_t Y, bool set, bool refresh)
{
    if((X>127) || (Y>63))
    {
        // out of range
        return;
    }
    uint8_t page = Y / 8;
    uint8_t mask = 1 << (Y % 8);

    // set or clear point in display buffer
    if(set)
    {
        displayBuffer[page][X] |= mask;
    }
    else
    {
        displayBuffer[page][X] &= ~mask;
    }

    // set lower limit of refreshing range
    if((refreshRange[page][0] == 0) || (refreshRange[page][0] > X+2))
    {
        refreshRange[page][0] = X+2;
    }
    // set upper limit of refreshing range
    if(refreshRange[page][1] < X+2)
    {
        refreshRange[page][1] = X+2;
    }

    //refresh display if required
    refreshRequest = refresh;
}
