/*
 * PCA9685.h
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#ifndef PCA9685_H_
#define PCA9685_H_

#include "I2C.h"

#define LOBYTE(x)  ((uint8_t)(x & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00U) >> 8U))

enum PCA9685Register
{
    MODE1 = 0x00,
    MODE2 = 0x01,
    LED0_ON_L = 0x06,
    PRE_SCALE = 0xFE
};

enum ChannelState
{
    ChannelOff,
    ChannelOn
};

class PCA9685 : public I2cDevice
{
public:
    PCA9685(I2cBus* pBus, DeviceAddress deviceAddress);
    void setPwmFrequency(uint16_t frequency);
    void setChannels(uint8_t firstChannel, std::vector<uint8_t> data);
    const std::vector<uint8_t> ChannelHigh{0x00, 0x10, 0x00, 0x00};
    const std::vector<uint8_t> ChannelLow{0x00, 0x00, 0x00, 0x10};
private:
    DeviceAddress deviceAddress;
};

#endif /* PCA9685_H_ */
