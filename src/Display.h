/*
 * Display.h
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "SH1106.h"

class Display
{
public:
    Display();
private:
    SH1106 controller;
};

#endif /* DISPLAY_H_ */
