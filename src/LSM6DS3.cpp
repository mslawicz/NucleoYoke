/*
 * LSM6DS3.cpp
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#include "LSM6DS3.h"

LSM6DS3::LSM6DS3(I2cBus* pBus) :
    I2cDevice(pBus, DeviceAddress::LSM6DS3_ADD),
    int1Pin(LSM6DS3_INT1_PORT, LSM6DS3_INT1_PIN, GPIO_MODE_INPUT, GPIO_PULLDOWN)
{
    // Gyroscope Data Ready to INT1 pin
    writeRequest(DeviceAddress::LSM6DS3_ADD, LSM6DS3Register::LSM6DS3_INT1_CTRL, std::vector<uint8_t>{0x02});
    // Accelerometer ODR=52 Hz, full scale +-2g
    // Gyroscope ODR=52 Hz, full scale 250 dps
    // Interrupt signal high, push-pull, auto increment, little endian
    writeRequest(DeviceAddress::LSM6DS3_ADD, LSM6DS3Register::LSM6DS3_CTRL1_XL, std::vector<uint8_t>{0x30, 0x30, 0x04});
    // Gyroscope digital high-pass filter enable, 0.0324 Hz
    writeRequest(DeviceAddress::LSM6DS3_ADD, LSM6DS3Register::LSM6DS3_CTRL7_G, std::vector<uint8_t>{0x50});
}

LSM6DS3::~LSM6DS3()
{
    // TODO Auto-generated destructor stub
}

