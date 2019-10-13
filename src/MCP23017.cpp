/*
 * MCP23017.cpp
 *
 *  Created on: 03.10.2019
 *      Author: Marcin
 */

#include "MCP23017.h"
#include "System.h" //XXX
#include "Conversion.h"

MCP23017::MCP23017(I2cBus* pBus, DeviceAddress deviceAddress, GPIO_TypeDef* portINT, uint32_t pinINT, uint16_t pinMask) :
    I2cDevice(pBus, deviceAddress),
    deviceAddress(deviceAddress),
    interruptPin(portINT, pinINT, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_MEDIUM)
{
    std::vector<uint8_t> configurationData =
    {
            // every register consists of 2 bytes (for ports A and B)
            0xFF, 0xFF, // inverted polarity
            LOBYTE(pinMask), HIBYTE(pinMask), // enable interrupt-on-change for particular pins
            0x00, 0x00, // default compare register
            0x00, 0x00, // interrupt control register
            0x42, 0x42, // configuration register: INT mirrored, INT active high
            LOBYTE(pinMask), HIBYTE(pinMask)  // pull-up register
    };
    writeRequest(deviceAddress, MCP23017Register::MCP23017_IPOLA, configurationData);
    state = ExpanderState::ES_start;
}

/*
 * MCP23017 GPIO expander handler
 * to be called periodically in a loop
 */
void MCP23017::handler(void)
{
    switch(state)
    {
    case ES_start:
        markNewDataReceived(false);
        state = ES_wait_for_int;
        break;
    case ES_wait_for_int:
        if(interruptPin.read() == GPIO_PinState::GPIO_PIN_SET)
        {
            System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
            System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET); //XXX
            // interrupt signal on - read expander data
            readRequest(deviceAddress, MCP23017Register::MCP23017_GPIOA, 2);
            eventTimer.reset();
            state = ES_debouncing;
        }
        break;
    case ES_debouncing:
        if((interruptPin.read() == GPIO_PinState::GPIO_PIN_SET) &&
                (eventTimer.elapsed(RepeadPeriod)))
        {
            System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
            // after RepeatPeriod time there is another INT signal - read data again
            state = ES_wait_for_int;
        }
        if(eventTimer.elapsed(StabilityTime))
        {
            // no next INT signal after StabilityTime
            state = ES_stable;
        }
        break;
    case ES_stable:
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
        if(isNewDataReceived())
        {
            System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
            markNewDataReceived(false);

            for(auto byte : receiveBuffer)  //XXX
            {
                System::getInstance().getConsole()->getInterface().send(Console::toHex(byte, 2, false)+" ");
            }
            System::getInstance().getConsole()->getInterface().send("\r\n"); //XXX
        }
        state = ES_wait_for_int;
        break;
    default:
        break;
    }
}
