/*
 * LSM6DS3.cpp
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#include "LSM6DS3.h"

LSM6DS3::LSM6DS3(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),
    address(deviceAddress)
{
    // Gyroscope Data Ready to INT1 pin for test only
    writeRequest(address, LSM6DS3Register::LSM6DS3_INT1_CTRL, std::vector<uint8_t>{0x02});
    // Accelerometer ODR=52 Hz, full scale +-2g
    // Gyroscope ODR=52 Hz, full scale 250 dps
    writeRequest(address, LSM6DS3Register::LSM6DS3_CTRL1_XL, std::vector<uint8_t>{0x30, 0x30});
    // Gyroscope digital high-pass filter enable, 0.0324 Hz
    writeRequest(address, LSM6DS3Register::LSM6DS3_CTRL7_G, std::vector<uint8_t>{0x50});
}

/*
 * returns float vector of measured angular rate in rad/s
 * positive slope fot moving joystick forward, right and twisting right
 */
FloatVector LSM6DS3::getAngularRate(void)
{
    return FloatVector
    {
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[2])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[0])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue,
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[4])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue
    };
}

/*
 * returns float vector of measured acceleration in g
 * positive values for Z in neutral and joystick forward and to right
 */
FloatVector LSM6DS3::getAcceleration(void)
{
    return FloatVector
    {
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[8])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[6])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[10])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue
    };
}

