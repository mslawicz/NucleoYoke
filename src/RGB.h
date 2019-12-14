/*
 * RGB.h
 *
 *  Created on: 24.10.2019
 *      Author: Marcin
 */

#ifndef RGB_H_
#define RGB_H_

#include "WS2812.h"
#include "SPI.h"
#include <vector>

class RGB
{
public:
    RGB(SpiBus* pBus, uint8_t numberOfLeds);
    uint32_t getCycledValue(uint8_t step, uint8_t phase);
    void handler(void);
    void setValue(uint8_t ledIndex, uint32_t value);
    void setValue(uint8_t ledIndex, WS2812Color value) { setValue(ledIndex, static_cast<uint32_t>(value)); }
    void requestUpdate(void) { updateRequest = true; }
private:
    WS2812 device;
    std::vector<uint32_t> ledData;
    bool updateRequest {true};
};

#endif /* RGB_H_ */
