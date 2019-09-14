/*
 * SH1106.h
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#ifndef SH1106_H_
#define SH1106_H_

#include "SPI.h"
#include "GPIO.h"
#include "Timer.h"

#define SH1106_CS_PORT    GPIOC
#define SH1106_CS_PIN     GPIO_PIN_8
#define SH1106_RESET_PORT    GPIOD
#define SH1106_RESET_PIN     GPIO_PIN_2

enum DisplayControllerState
{
    DCS_start,
    DCS_reset_off,
    DCS_wait_before_init,
    DCS_initialize,
    DCS_wait_after_init,
    DCS_send_loop
};

class SH1106 : public SpiDevice
{
public:
    SH1106(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS);
    void test(void); //XXX
    void handler(void);
private:
    DisplayControllerState state;
    GPIO resetPin;
    Timer controllerTimer;
    const uint32_t WaitBeforeInitTime = 100;
    const uint32_t WaitAfterInitTime = 100000;
    const std::vector<uint8_t> SH1106InitData
    {
        0x32,   //DC voltage output value
        0x40,   //display line for COM0 = 0
        0x81,   //contrast
        0x80,
        0xA0,   //segment right rotation
        0xA4,   //display in normal mode
        0xA6,   //display normal indication
        0xAD,   //DC pump on
        0x8B,
        0xC0,   //scan from 0 to N-1
        0xAF,   //display on
    };
};

#endif /* SH1106_H_ */
