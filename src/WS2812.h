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

enum class WS2812Color : uint32_t
{
    off = 0,
    blue = 0x000004,
    red = 0x000400,
    green = 0x040000,
    magenta = 0x000202,
    cyan = 0x020002,
    yellow = 0x020200,
    orange = 0x010300,
    white = 0x020202,
    gray = 0x010101
};

class WS2812 : public SpiDevice
{
public:
    WS2812(SpiBus* pBus);
    void send(std::vector<uint32_t> dataGRB);
private:
    const uint8_t zeroBitPattern = 0b11000000;
    const uint8_t onebitPattern = 0b11111100;
};

#endif /* WS2812_H_ */
