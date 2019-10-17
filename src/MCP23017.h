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
#include "Timer.h"
#include "Decoder.h"
#include <vector>

#define MCP23017_0_INT_PORT    GPIOD    //temporary value XXX
#define MCP23017_0_INT_PIN     GPIO_PIN_1   //temporary value XXX

// caution! the following addressing reflects the default addressing scheme (IOCON.BANK = 0)
enum MCP23017Register
{
    MCP23017_IPOLA = 0x02,
    MCP23017_INTFA = 0x0E,
    MCP23017_GPIOA = 0x12
};

enum ExpanderState
{
    ES_start,
    ES_wait_for_int,
    ES_debouncing,
    ES_stable
};

class MCP23017 : public I2cDevice
{
public:
    MCP23017(I2cBus* pBus, DeviceAddress deviceAddress, GPIO_TypeDef* portINT, uint32_t pinINT, uint16_t pinMask);
    bool handler(void);
    uint16_t getInputRegister(void) const { return inputRegister; }
    std::vector<Decoder*>& getDecoders(void) { return decoders; }
private:
    DeviceAddress deviceAddress;
    GPIO interruptPin;
    uint8_t state;
    Timer debounceTimer;        // timer for input debouncing purpose
    const uint32_t RepeadPeriod = 1000;     // period of expander readout repetitions for debouncing
    const uint32_t StabilityTime = 10000;   // required time of stable input states
    uint16_t inputRegister;  // current input data
    std::vector<Decoder*> decoders;   // vector of decoders for this GPIO expander
};

#endif /* MCP23017_H_ */
