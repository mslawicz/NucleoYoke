/*
 * SPI.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "SPI.h"
#include "system.h"

SpiBus::SpiBus(SPI_TypeDef* instance) :
    instance(instance)
{
}

SpiDevice::SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS, bool autoCS) :
        pBus(pBus),
        chipSelect(portCS, pinCS, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH),
        autoCS(autoCS)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_SPI, "SPI device created, CS=" + Console::toHex(reinterpret_cast<uint32_t>(portCS)) + "/" + Console::toHex(pinCS));
    chipSelect.write(GPIO_PinState::GPIO_PIN_SET);
}

SpiDevice::~SpiDevice() {}
