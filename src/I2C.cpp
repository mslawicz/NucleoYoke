/*
 * I2C.cpp
 *
 *  Created on: 30.08.2019
 *      Author: Marcin
 */

#include "I2C.h"

I2cBus::I2cBus(I2C_TypeDef* I2cInstance) :
    instance(I2cInstance)
{

}
