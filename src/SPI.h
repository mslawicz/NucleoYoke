/*
 * SPI.h
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f4xx_hal.h"
#include "GPIO.h"

class SpiBus
{
public:
    SpiBus(SPI_TypeDef* instance);
private:
    SPI_TypeDef* instance;
};

class SpiDevice
{
protected:
    SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS, bool autoCS = true);
    virtual ~SpiDevice();
    SpiBus* pBus;       // SPI bus for this device
private:
    GPIO chipSelect;
    bool autoCS;
};

#endif /* SPI_H_ */
