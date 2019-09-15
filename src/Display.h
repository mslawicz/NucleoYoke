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

class Display
{
public:
    Display();
    void handler(void) { controller.handler(); }
    SH1106& getController(void) { return controller; }
    uint8_t putChar(uint8_t ch, uint8_t X, uint8_t Y, const uint8_t* font, bool inverted = false, bool refresh = false);
private:
    SH1106 controller;
};

#endif /* DISPLAY_H_ */
