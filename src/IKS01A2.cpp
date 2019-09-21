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
    // Gyroscope ODR=59.5 Hz, full scale 245 dps
    // int/out selection default
    // low power disable, HPF enable, HPF=0.05 Hz
    writeRequest(deviceAddress, LSM6DSLRegister::LSM6DSL_INT1_CTRL, std::vector<uint8_t>{0x40, 0x00, 0x46});
}

LSM303AGR::LSM303AGR(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, DeviceAddress::LSM303AGR_M_ADDR),// deviceAddress),
    deviceAddress(deviceAddress)
{
    // Magnetometer X&Y high-performance mode, ODR=40 Hz
    // full scale +-4 gauss
    // continues conversion mode
    // Z-axis high-performance mode
    writeRequest(deviceAddress, LSM303AGRRegister::LSM303AGR_REG, std::vector<uint8_t>{0x58, 0x00, 0x00, 0x80});
}
