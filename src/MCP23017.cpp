/*
 * MCP23017.cpp
 *
 *  Created on: 03.10.2019
 *      Author: Marcin
 */

#include "MCP23017.h"

MCP23017::MCP23017(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),
    deviceAddress(deviceAddress)
{


}

