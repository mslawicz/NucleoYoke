/*
 * SH1106.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "SH1106.h"

SH1106::SH1106(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS) :
    SpiDevice(pBus, portCS, pinCS)
{
    // TODO Auto-generated constructor stub

}

