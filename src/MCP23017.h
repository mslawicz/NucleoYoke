/*
 * MCP23017.h
 *
 *  Created on: 03.10.2019
 *      Author: Marcin
 */

#ifndef MCP23017_H_
#define MCP23017_H_

#include "I2C.h"

// caution! the following addressing reflects the default addressing scheme (IOCON.BANK = 0)
enum MCP23017Register
{
    MCP23017_IODIRA = 0x00,
    MCP23017_GPIOA = 0x12
};

class MCP23017 : public I2cDevice
{
public:
    MCP23017(I2cBus* pBus, DeviceAddress deviceAddress);
private:
    DeviceAddress deviceAddress;
};

#endif /* MCP23017_H_ */
