/*
 * MCP23017.h
 *
 *  Created on: 03.10.2019
 *      Author: Marcin
 */

#ifndef MCP23017_H_
#define MCP23017_H_

#include "I2C.h"
#include "GPIO.h"

#define MCP23017_0_INT_PORT    GPIOD    //temporary value XXX
#define MCP23017_0_INT_PIN     GPIO_PIN_1   //temporary value XXX

// caution! the following addressing reflects the default addressing scheme (IOCON.BANK = 0)
enum MCP23017Register
{
    MCP23017_IPOLA = 0x02,
    MCP23017_INTFA = 0x0E
};

class MCP23017 : public I2cDevice
{
public:
    MCP23017(I2cBus* pBus, DeviceAddress deviceAddress, GPIO_TypeDef* portINT, uint32_t pinINT, uint16_t pinMask);
    void handler(void);
private:
    DeviceAddress deviceAddress;
    GPIO interruptPin;
    bool waitingForData;
};

#endif /* MCP23017_H_ */
