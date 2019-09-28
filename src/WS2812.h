/*
 * WS2812.h
 *
 *  Created on: 28.09.2019
 *      Author: Marcin
 */

#ifndef WS2812_H_
#define WS2812_H_

#include "SPI.h"

class WS2812 : public SpiDevice
{
public:
    WS2812();
};

#endif /* WS2812_H_ */
