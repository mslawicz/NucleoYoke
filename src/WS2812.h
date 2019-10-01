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

enum WS2812Color
{
    Color_blue = 0x000001,
    Color_red = 0x000100,
    Color_green = 0x010000,
    Color_magenta = 0x000101,
    Color_cyan = 0x010001,
    Color_yellow = 0x010100,
    Color_white = 0x010101
};

class WS2812 : public SpiDevice
{
public:
    WS2812(SpiBus* pBus);
    void send(std::vector<uint32_t> dataGRB);
    uint32_t getCycledValue(uint8_t step, uint8_t phase);
private:
    const uint8_t zeroBitPattern = 0xC0;
    const uint8_t onebitPattern = 0xFC;
};

#endif /* WS2812_H_ */
