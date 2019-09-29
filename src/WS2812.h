/*
 * WS2812.h
 *
 *  Created on: 28.09.2019
 *      Author: Marcin
 */

#ifndef WS2812_H_
#define WS2812_H_

#include "SPI.h"
#include <vector>

class WS2812 : public SpiDevice
{
public:
    WS2812(SpiBus* pBus);
    void send(std::vector<uint32_t> dataGRB);
private:
    const uint8_t zeroBitPattern = 0x80;
    const uint8_t onebitPattern = 0xF0;
};

#endif /* WS2812_H_ */
