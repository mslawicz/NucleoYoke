/*
 * PCA9685.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "PCA9685.h"
#include <vector>

PCA9685::PCA9685(I2cBus* pBus, DeviceAddress deviceAddress) :
    I2cDevice(pBus, deviceAddress),
    deviceAddress(deviceAddress)

{
    // set initial PWM frequency, and also these parameters:
    // address autoincrement enabled, all-call enabled
    setPwmFrequency(100);
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
 * set PWM signal in the channel
 */
void PCA9685::setChannel(uint8_t channel, uint16_t turnOn, uint16_t turnOff)
{
    writeRequest(deviceAddress, PCA9685Register::LED0_ON_L + channel * 4, std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
}

/*
 * set channel to constant on or off state
 */
void PCA9685::setChannel(uint8_t channel, ChannelState state)
{
    if(state == ChannelState::ChannelOn)
    {
        writeRequest(deviceAddress, PCA9685Register::LED0_ON_L + channel * 4, std::vector<uint8_t>{0x00, 0x08, 0x00, 0x00});
    }
    else
    {
        writeRequest(deviceAddress, PCA9685Register::LED0_ON_L + channel * 4, std::vector<uint8_t>{0x00, 0x00, 0x00, 0x08});
    }
}
