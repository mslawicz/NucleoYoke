/*
 * I2C.h
 *
 *  Created on: 30.08.2019
 *      Author: Marcin
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx_hal.h"

class I2cBus
{
public:
    I2cBus(I2C_TypeDef* I2cInstance);
private:
    I2C_TypeDef* instance;
};

class I2cDevice
{

};

#endif /* I2C_H_ */
