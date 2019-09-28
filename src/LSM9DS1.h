/*
 * LSM9DS1.h
 *
 *  Created on: 21.09.2019
 *      Author: Marcin
 */

#ifndef LSM9DS1_H_
#define LSM9DS1_H_

#include "I2C.h"
#include "GPIO.h"

enum LSM9DS1Register
{
    LSM9DS1_INT1_CTRL = 0x0C,
    LSM9DS1_CTRL_REG1_G = 0x10,
    LSM9DS1_OUT_X_L_G = 0x18,
    LSM9DS1_CTRL_REG6_XL = 0x20,
    LSM9DS1_CTRL_REG1_M = 0x20,
    LSM9DS1_OUT_X_L_M = 0x28
};

/*
 * LSM9DS1 gyroscope and accelerometer class
 */
class LSM9DS1AG : public I2cDevice
{
public:
    LSM9DS1AG(I2cBus* pBus, DeviceAddress deviceAddress);
    void getData(void) { readRequest(address, LSM9DS1Register::LSM9DS1_OUT_X_L_G, 12); }
private:
    DeviceAddress address;
};

/*
 * LSM9DS1 magnetometer class
 */
class LSM9DS1M : public I2cDevice
{
public:
    LSM9DS1M(I2cBus* pBus, DeviceAddress deviceAddress);
    void getData(void) { readRequest(address, LSM9DS1Register::LSM9DS1_OUT_X_L_M, 6); }
private:
    DeviceAddress address;
};

#endif /* LSM9DS1_H_ */
