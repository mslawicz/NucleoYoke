/*
 * LSM9DS1.cpp
 *
 *  Created on: 21.09.2019
 *      Author: Marcin
 */

#include "LSM9DS1.h"

LSM9DS1AG::LSM9DS1AG(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),
    address(deviceAddress)
{
    // Gyroscope ODR=59.5 Hz, full scale 245 dps
    // int/out selection default
    // low power disable, HPF enable, HPF=0.05 Hz
    writeRequest(address, LSM9DS1Register::LSM9DS1_CTRL_REG1_G, std::vector<uint8_t>{0x40, 0x00, 0x46});
    // Accelerometer ODR=50 Hz, full scale +=2g
    writeRequest(address, LSM9DS1Register::LSM9DS1_CTRL_REG6_XL, std::vector<uint8_t>{0x40});
    // INT1<-DRDY_G for check only
    writeRequest(address, LSM9DS1Register::LSM9DS1_INT1_CTRL, std::vector<uint8_t>{0x02});
}

LSM9DS1M::LSM9DS1M(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),// deviceAddress),
    address(deviceAddress)
{
    // Magnetometer X&Y high-performance mode, ODR=40 Hz
    // full scale +-4 gauss
    // continues conversion mode
    // Z-axis high-performance mode
    writeRequest(address, LSM9DS1Register::LSM9DS1_CTRL_REG1_M, std::vector<uint8_t>{0x58, 0x00, 0x00, 0x80});
}


