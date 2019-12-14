/*
 * RGB.cpp
 *
 *  Created on: 24.10.2019
 *      Author: Marcin
 */

#include "RGB.h"

RGB::RGB(SpiBus* pBus, uint8_t numberOfLeds) :
    device(pBus)
{
    ledData.resize(numberOfLeds, 0);
}

/*
* returns GRB value for a LED in color cycling
* step - number of step in the cycle 0..23
* phase - number of steps this LED is shifted in the cycle
*/

uint32_t RGB::getCycledValue(uint8_t step, uint8_t phase)
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

/*
 * RGB LEDs handler
 * to be called periodically in a loop
 */
void RGB::handler(void)
{
    if(updateRequest)
    {
        updateRequest = false;
        device.send(ledData);
    }
}

/*
 * sets the BRG value of a single LED in a chain
 */
void RGB::setValue(uint8_t ledIndex, uint32_t value)
{
    if(ledIndex < ledData.size())
    {
        ledData[ledIndex] = value;
    }
}
