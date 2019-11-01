/*
 * Display.h
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "SH1106.h"
#include "fonts.h"
#include <string>

class Display
{
public:
    Display();
    void handler(void) { controller.handler(); }
    SH1106& getController(void) { return controller; }
    uint8_t putChar(uint8_t X, uint8_t Y, uint8_t ch, const uint8_t* font, bool inverted = false, bool refresh = false, uint8_t upToX = 0);
    uint8_t print(uint8_t X, uint8_t Y, std::string text, const uint8_t* font, bool inverted = false, bool refresh = false, uint8_t upToX = 0);
    uint8_t calculateLength(std::string text, const uint8_t* font);
private:
    uint8_t putChar2CharSpace(uint8_t X, uint8_t Y, const uint8_t* font, bool inverted = false, bool refresh = false, uint8_t upToX = 0);
    SH1106 controller;
};

#endif /* DISPLAY_H_ */
