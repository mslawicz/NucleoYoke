/*
 * SH1106.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "SH1106.h"

SH1106::SH1106() :
    SpiDevice(SpiBus::pSpi3, SH1106_CS_PORT, SH1106_CS_PIN)
{
    // TODO Auto-generated constructor stub

}

