/*
 * IKS01A2.h
 *
 *  Created on: 21.09.2019
 *      Author: Marcin
 */

#ifndef IKS01A2_H_
#define IKS01A2_H_

#include "I2C.h"
#include "GPIO.h"

enum LSM6DSLRegister
{
    LSM6DSL_INT1_CTRL = 0x00
};

enum LSM303AGRRegister
{
    LSM303AGR_REG = 0x00
};

/*
 * LSM6DSL gyroscope and accelerometer class
 */
class LSM6DSL : public I2cDevice
{
public:
    LSM6DSL(I2cBus* pBus, DeviceAddress deviceAddress);
    void getData(void) { readRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_INT1_CTRL, 12); }
private:
    DeviceAddress deviceAddress;
};

/*
 * LSM303AGR magnetometer class
 */
class LSM303AGR : public I2cDevice
{
public:
    LSM303AGR(I2cBus* pBus, DeviceAddress deviceAddress);
    void getData(void) { readRequest(deviceAddress, LSM303AGRRegister::LSM303AGR_REG, 6); }
private:
    DeviceAddress deviceAddress;
};

#endif /* IKS01A2_H_ */
