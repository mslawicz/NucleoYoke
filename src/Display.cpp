/*
 * Display.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "Display.h"

Display::Display() :
    controller(SpiBus::pSpi4, SH1106_CS_PORT, SH1106_CS_PIN)
{

}

/*
 * displays character on the screen
 * ch - ascii code
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverted - clears pixels if true
 * refresh - request for screen update
 * upToX - if >0, stops at X==upToX
 */
uint8_t Display::putChar(uint8_t X, uint8_t Y,uint8_t ch, const uint8_t* font, bool inverted, bool refresh, uint8_t upToX)
{
    bool isSpace = false;

    if((ch < font[4]) || (ch >= font[4]+font[5]))
    {
        // ascii code out of this font range
        return X;
    }

    // width of this char
    uint8_t charWidth = font[6 + ch - font[4]];
    if(charWidth == 0)
    {
        isSpace = true;
        charWidth = font[2];
    }

    // height of this char
    uint8_t charHeight = font[3];

    // calculate index of this char definition in array
    uint16_t charDefinitionIndex = 6 + font[5];
    for(uint8_t i = 0; i < ch - font[4]; i++)
    {
        charDefinitionIndex += font[6 + i] * (1 + (charHeight - 1) / 8);
    }

    // for every column
    uint8_t ix;
    for(ix = 0; ix < charWidth; ix++)
    {
        // if upToX!=0 then print up to this X limit
        if((upToX != 0) && (X+ix > upToX))
        {
            break;
        }
        // for every horizontal row
        for(uint8_t iy = 0; iy < charHeight; iy++)
        {
            uint8_t bitPattern = isSpace ? 0 : font[charDefinitionIndex + ix + (iy / 8) * charWidth];
            bool lastByte = iy / 8 == charHeight / 8;
            uint8_t extraShift = lastByte ? 8 - charHeight % 8 : 0;
            controller.setPoint(X + ix, Y + iy, ((bitPattern >> (extraShift + iy % 8)) & 0x01) != inverted);
        }
    }

    if(refresh)
    {
        controller.requestUpdate();
    }

    return X + ix;
}

/*
 * draws rectangle
 */
void Display::drawRectangle(uint8_t X, uint8_t Y, uint8_t width, uint8_t height, bool inverted)
{
    for(uint8_t ix = 0; ix < width; ix++)
    {
        for(uint8_t iy = 0; iy < height; iy++)
        {
            controller.setPoint(X + ix, Y + iy, !inverted);
        }
    }
}

/*
 * displays character on the screen
 * ch - ascii code
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverted - clears pixels if true
 * refresh - request for screen update
 * upToX - if >0, stops at X==upToX
 */
uint8_t Display::putChar2CharSpace(uint8_t X, uint8_t Y, const uint8_t* font, bool inverted, bool refresh, uint8_t upToX)
{
    // height of this space
    uint8_t charHeight = font[3];

    // width of this space
    uint8_t charWidth = 1 + (charHeight - 2) / 8;

    // for every column
    uint8_t ix;
    for(ix = 0; ix < charWidth; ix++)
    {
        // if upToX!=0 then print up to this X limit
        if((upToX != 0) && (X+ix > upToX))
        {
            break;
        }
        // for every horizontal row
        for(uint8_t iy = 0; iy < charHeight; iy++)
        {
            controller.setPoint(X + ix, Y + iy, inverted);
        }
    }

    if(refresh)
    {
        controller.requestUpdate();
    }

    return X + ix;
}

/*
 * displays string on the screen
 * text - string to be displayed
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverted - clears pixels if true
 * refresh - request for screen update
 * upToX - if >0, prints the text up to X==upToX
 */
uint8_t Display::print(uint8_t X, uint8_t Y, std::string text, const uint8_t* font, bool inverted, bool refresh, uint8_t upToX)
{
    for(size_t index = 0; index < text.size(); index++)
    {
        X = putChar(X, Y, text[index], font, inverted, refresh, upToX);
        if(index < text.size() - 1)
        {
            X = putChar2CharSpace(X, Y, font, inverted, refresh, upToX);
        }
    }
    return X;
}

/*
 * calculates the length of the text in pixels according to applied font
 */
uint8_t Display::calculateLength(std::string text, const uint8_t* font)
{
    uint8_t length = 0;
    for(size_t index = 0; index < text.size(); index++)
    {
        auto ch = text[index];
        if((ch < font[4]) || (ch >= font[4]+font[5]))
        {
            // ascii code out of this font range
            continue;
        }

        // width of this char
        uint8_t charWidth = font[6 + ch - font[4]];
        if(charWidth == 0)
        {
            charWidth = font[2];
        }

        length += charWidth;

        // add char-to-char space
        if(index < text.size() - 1)
        {
            length += 1 + (font[3] - 2) / 8;
        }
    }
    return length;
}
