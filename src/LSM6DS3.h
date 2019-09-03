/*
 * LSM6DS3.h
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#ifndef LSM6DS3_H_
#define LSM6DS3_H_

#include "I2C.h"

class LSM6DS3 : public I2cDevice
{
public:
    LSM6DS3(I2cBus* pBus);
    ~LSM6DS3();
};

#endif /* LSM6DS3_H_ */
