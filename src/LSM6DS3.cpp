/*
 * LSM6DS3.cpp
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#include "LSM6DS3.h"

LSM6DS3::LSM6DS3(I2cBus* pBus) :
    I2cDevice(pBus, DeviceAddress::LSM6DS3_ADD)
{
    // TODO Auto-generated constructor stub

}

LSM6DS3::~LSM6DS3()
{
    // TODO Auto-generated destructor stub
}

