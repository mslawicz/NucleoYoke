/*
 * WS2812.cpp
 *
 *  Created on: 28.09.2019
 *      Author: Marcin
 */

#include "WS2812.h"

WS2812::WS2812(SpiBus* pBus) :
    SpiDevice(pBus)
{

}

/*
 * sends data to daisy-chained LEDs
 * data for a single LED contain 4 8-bit fields in format 0+B+R+G packed in uint32_t
 */
void WS2812::send(std::vector<uint32_t> chainData)
{
    std::vector<uint8_t> dataToSend;    // vector of data to be sent to SPI
    for(auto ledData : chainData)
    {
        // loop for every led in the chain
        for(uint8_t bit = 0; bit < 24; bit++)
        {
            // loop for every bit in the LED data
            dataToSend.push_back((ledData >> (23-bit)) & 0x01 ? onebitPattern : zeroBitPattern);
        }
    }
    dataToSend.push_back(0x00);     // a dummy byte to leave data in zero state
    sendRequest(dataToSend);
}

/*
* returns GRB value for a LED in color cycling
* step - number of step in the cycle 0..23
* phase - number of steps this LED is shifted in the cycle
*/

uint32_t WS2812::getCycledValue(uint8_t step, uint8_t phase)
{
    auto colorValue = [](uint8_t step, uint8_t phase, uint8_t extraPhase)
    {
           uint8_t delta = abs(step - phase + extraPhase) % 24;
           delta = delta > 12 ? 24 - delta : delta;
           return 8 - (delta > 8 ? 8 : delta);
    };
    auto red = colorValue(step, phase, 0);
    auto green = colorValue(step, phase, 8);
    auto blue = colorValue(step, phase, 16);
    return (green << 16) | (red << 8) | blue;
}
