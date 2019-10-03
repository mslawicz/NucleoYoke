/*
 * PCA9685.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "PCA9685.h"
#include "Conversion.h"
#include <vector>

PCA9685::PCA9685(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),
    deviceAddress(deviceAddress)
{
    // set initial PWM frequency, and also these parameters:
    // address autoincrement enabled, all-call enabled
    setPwmFrequency(1500);
    // output logic not inverted; change on ACK, totem pole
    writeRequest(deviceAddress, PCA9685Register::MODE2, std::vector<uint8_t>{0x0C});
}

/*
 * set PWM frequency for all channels
 * input range 24..1526 Hz
 */
void PCA9685::setPwmFrequency(uint16_t frequency)
{
    uint16_t prescaleValue = 25000000u / (4096 * frequency) - 1;
    if(prescaleValue < 3)
    {
        prescaleValue = 3;
    }
    else if (prescaleValue > 0xFF)
    {
        prescaleValue = 0xFF;
    }
    // set sleep mode
    writeRequest(deviceAddress, PCA9685Register::MODE1, std::vector<uint8_t>{0x31});
    // write PRE_SCALE register
    writeRequest(deviceAddress, PCA9685Register::PRE_SCALE, std::vector<uint8_t>{LOBYTE(prescaleValue)});
    // set normal mode
    writeRequest(deviceAddress, PCA9685Register::MODE1, std::vector<uint8_t>{0x21});
}

/*
 * set channels
 */
void PCA9685::setChannels(uint8_t firstChannel, std::vector<uint8_t> data)
{
    writeRequest(deviceAddress, PCA9685Register::LED0_ON_L + firstChannel * 4, data);
}
