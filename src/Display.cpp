/*
 * Display.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "Display.h"

Display::Display() :
    controller(SpiBus::pSpi3, SH1106_CS_PORT, SH1106_CS_PIN)
{

}

/*
 * displays character on the screen
 * ch - ascii code
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverted - clears pixels if true
 * refresh - request for screen update
 */
uint8_t Display::putChar(uint8_t ch, uint8_t X, uint8_t Y, const uint8_t* font, bool inverted, bool refresh)
{
    if((ch < font[4]) || (ch >= font[4]+font[5]))
    {
        // ascii code out of this font range
        return X;
    }

    // width of this char
    uint8_t charWidth = font[6 + ch - font[4]];

    // height of this char
    uint8_t charHeight = font[3];

    // calculate index of this char definition in array
    uint8_t charDefinitionIndex = 6 + font[5];
    for(uint8_t i = 0; i < ch - font[4]; i++)
    {
        charDefinitionIndex += font[6 + i];
    }

    // for every column
    for(uint8_t ix = 0; ix < charWidth; ix++)
    {
        // for every horizontal row
        for(uint8_t iy = 0; iy < charHeight; iy++)
        {
            uint8_t bitPattern = font[charDefinitionIndex + ix + (iy / 8) * charWidth];
            controller.setPoint(X + ix, Y + iy, ((bitPattern >> (iy % 8)) & 0x01) != inverted);
        }
    }

    if(refresh)
    {
        controller.requestUpdate();
    }

    return X + charWidth;
}
