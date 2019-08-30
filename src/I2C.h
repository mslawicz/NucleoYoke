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

};

#endif /* I2C_H_ */
