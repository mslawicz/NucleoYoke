/*
 * LSM6DS3.h
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#ifndef LSM6DS3_H_
#define LSM6DS3_H_

#include "I2C.h"
#include "GPIO.h"

#define LSM6DS3_INT1_PORT   GPIOC
#define LSM6DS3_INT1_PIN    GPIO_PIN_3

enum LSM6DS3Register
{
    ORIENT_CFG_G=0x0B,
    INT1_CTRL=0x0D,
    CTRL1_XL=0x10,
    OUTX_L_G=0x22
};

class LSM6DS3 : public I2cDevice
{
public:
    LSM6DS3(I2cBus* pBus);
    ~LSM6DS3();
private:
    GPIO int1Pin;
};

#endif /* LSM6DS3_H_ */
