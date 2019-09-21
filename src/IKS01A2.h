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
    LSM6DSL_INT1_CTRL = 0x0D,
    LSM6DSL_CTRL1_XL = 0x10,
    LSM6DSL_CTRL7_G = 0x16,
    LSM6DSL_OUTX_L_G = 0x22
};

enum LSM303AGRRegister
{
    LSM303AGR_CFG_REG_A_M = 0x60,
    LSM303AGR_CFG_REG_C_M = 0x62,
    LSM303AGR_OUTX_L_REG_M = 0x68
};

/*
 * LSM6DSL gyroscope and accelerometer class
 */
class LSM6DSL : public I2cDevice
{
public:
    LSM6DSL(I2cBus* pBus, DeviceAddress deviceAddress);
    void getData(void) { readRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_OUTX_L_G, 12); }
    const float gyroscopeFullScaleValue = 4.363323f;  // full scale value [rad/s]
    const float accelerometerFullScaleValue = 2.0f;  // full scale value [g]
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
    void getData(void) { readRequest(deviceAddress, LSM303AGRRegister::LSM303AGR_OUTX_L_REG_M, 6); }
    const float magnetometerFullScaleValue = 50.0f;  // full scale value [gauss]
private:
    DeviceAddress deviceAddress;
};

#endif /* IKS01A2_H_ */
