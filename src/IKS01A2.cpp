/*
 * IKS01A2.cpp
 *
 *  Created on: 21.09.2019
 *      Author: Marcin
 */

#include "IKS01A2.h"

LSM6DSL::LSM6DSL(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, DeviceAddress::LSM6DSL_ADD),// deviceAddress),
    deviceAddress(deviceAddress)
{
    // Gyroscope Data Ready on INT1 pad for checking
    writeRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_INT1_CTRL, std::vector<uint8_t>{0x02});
    // Accelerometer ODR=52 Hz, +-2g
    // Gyroscope ODR=52 Hz, full scale=250 dps
    writeRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_CTRL1_XL, std::vector<uint8_t>{0x30, 0x30});
    // Gyroscope HPF=65 mHz
    writeRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_CTRL7_G, std::vector<uint8_t>{0x50});
}

LSM303AGR::LSM303AGR(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, DeviceAddress::LSM303AGR_M_ADDR),// deviceAddress),
    deviceAddress(deviceAddress)
{
    // Magnetometer ODR=50 Hz, continues mode
    writeRequest(deviceAddress, LSM303AGRRegister::LSM303AGR_CFG_REG_A_M, std::vector<uint8_t>{0x08});
    // asynchronous data always valid
    writeRequest(deviceAddress, LSM303AGRRegister::LSM303AGR_CFG_REG_C_M, std::vector<uint8_t>{0x10});
}

/*
 * returns float vector of measured angular rate in rad/s
 * returned values must reflect right hand rule of XYZ->north-east-down oriented axis
 */
FloatVector LSM6DSL::getAngularRate(void)
{
    return FloatVector
    {
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[0])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue,
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[2])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[4])) / MeasurementRegisterFullScaleValue * gyroscopeFullScaleValue
    };
}

/*
 * returns float vector of measured acceleration in g
 * returned values must reflect XYZ->north-east-down oriented axis
 */
FloatVector LSM6DSL::getAcceleration(void)
{
    return FloatVector
    {
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[6])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[8])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue,
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[10])) / MeasurementRegisterFullScaleValue * accelerometerFullScaleValue
    };
}

/*
 * returns float vector of measured magnetic field in gauss
 * returned values must reflect XYZ->north-east-down oriented axis
 */
FloatVector LSM303AGR::getMagneticField(void)
{
    return FloatVector
    {
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[0])) / MeasurementRegisterFullScaleValue * magnetometerFullScaleValue,
        -static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[2])) / MeasurementRegisterFullScaleValue * magnetometerFullScaleValue,
        static_cast<float>(*reinterpret_cast<int16_t*>(&receiveBuffer[4])) / MeasurementRegisterFullScaleValue * magnetometerFullScaleValue
    };
}
