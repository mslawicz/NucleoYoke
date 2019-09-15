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
    return X;
}
