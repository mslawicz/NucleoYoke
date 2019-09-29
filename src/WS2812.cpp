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
    dataToSend.push_back(0x00);
    sendRequest(dataToSend);
}
