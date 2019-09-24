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
#include "Conversion.h"

enum LSM6DS3Register
{
    LSM6DS3_INT1_CTRL=0x0D,
    LSM6DS3_CTRL1_XL=0x10,
    LSM6DS3_CTRL7_G = 0x16,
    LSM6DS3_OUTX_L_G=0x22
};

class LSM6DS3 : public I2cDevice
{
public:
    LSM6DS3(I2cBus* pBus, DeviceAddress deviceAddress);
    void readNewData(void) { readRequest(deviceAddress, LSM6DS3Register::LSM6DS3_OUTX_L_G, 12); }
    FloatVector getAngularRate(void);
    FloatVector getAcceleration(void);
private:
    DeviceAddress deviceAddress;
    const int16_t MeasurementRegisterFullScaleValue = 0x7FFF;     // sensor measurement full scale value
    const float gyroscopeFullScaleValue = 4.363323f;  // gyroscope read full scale value [rad/s]
    const float accelerometerFullScaleValue = 2.0f;  // accelerometer read full scale value [g]
};

#endif /* LSM6DS3_H_ */
