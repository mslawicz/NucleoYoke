/*
 * SH1106.cpp
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#include "SH1106.h"
#include "Timer.h" //XXX
#include "System.h" //XXX

SH1106::SH1106(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS) :
    SpiDevice(pBus, portCS, pinCS)
{
    // TODO Auto-generated constructor stub

}

void SH1106::test(void)
{
    static Timer tm;//XXX
    if(tm.elapsed(10000)) //XXX
    {
        tm.reset();
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET);    //XXX
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_RESET);    //XXX
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET);    //XXX

        sendRequest(std::vector<uint8_t>{10,11,12}, true);
        sendRequest(std::vector<uint8_t>{1,2,3,4,5,6,7,8});
    }
}
