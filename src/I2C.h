/*
 * I2C.h
 *
 *  Created on: 30.08.2019
 *      Author: Marcin
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx_hal.h"
#include <string>

// I2C device address
enum DeviceAddress
{
    LSM9DS1_AG_ADD = 0xD6
};

class I2cBus
{
public:
    I2cBus(I2C_TypeDef* instance);
    I2C_HandleTypeDef* getHandle(void) const { return const_cast<__I2C_HandleTypeDef*>(&hI2c); }
    DMA_HandleTypeDef* getDmaTxHandle(void) const { return const_cast<DMA_HandleTypeDef*>(&hDmaI2cTx); }
    DMA_HandleTypeDef* getDmaRxHandle(void) const { return const_cast<DMA_HandleTypeDef*>(&hDmaI2cRx); }
    static I2cBus* pI2c1;
private:
    I2C_HandleTypeDef hI2c;
    std::string name;
    DMA_HandleTypeDef hDmaI2cTx;
    DMA_HandleTypeDef hDmaI2cRx;
};

class I2cDevice
{
public:
    void test(void);
protected:
public://XXX
    I2cDevice(I2cBus* pBus, DeviceAddress deviceAddress);
private:
    I2cBus* pBus;       // I2C bus for this device
    DeviceAddress deviceAddress;        // I2C device address (7-bit left aligned)
};

#endif /* I2C_H_ */
