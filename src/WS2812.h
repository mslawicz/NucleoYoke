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
    Color_off = 0,
    Color_blue = 0x000004,
    Color_red = 0x000400,
    Color_green = 0x040000,
    Color_magenta = 0x000202,
    Color_cyan = 0x020002,
    Color_yellow = 0x020200,
    Color_orange = 0x010300,
    Color_white = 0x020202,
    Color_gray = 0x010101
};

class WS2812 : public SpiDevice
{
public:
    WS2812(SpiBus* pBus);
    void send(std::vector<uint32_t> dataGRB);
private:
    const uint8_t zeroBitPattern = 0xC0;
    const uint8_t onebitPattern = 0xFC;
};

#endif /* WS2812_H_ */
