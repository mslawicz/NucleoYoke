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
    Color_blue = 0x000008,
    Color_red = 0x000800,
    Color_green = 0x080000,
    Color_orange = 0x020800,
    Color_magenta = 0x000808,
    Color_cyan = 0x080008,
    Color_yellow = 0x080800,
    Color_white = 0x080808
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
