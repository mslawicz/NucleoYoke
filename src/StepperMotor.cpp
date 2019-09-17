/*
 * StepperMotor.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "StepperMotor.h"
#include "System.h"
#include "Conversion.h"
#include <map>
#include <vector>
#include <cmath>

/*
 * stepper motor constructor
 * pDriver - pointer to PCA9685 controller object
 * motorIndex - index of one of two motors controlled by a PCA9685 controller (0 or 1)
 */
StepperMotor::StepperMotor(PCA9685* pDriver, uint8_t motorIndex) :
    pDriver(pDriver),
    motorIndex(motorIndex)
{


}

/*
 * set motor force by controlling PWM waves
 * 0.0 - motor off
 * 0.5 - light force (wave drive)
 * 0.75 - moderate force (half step drive)
 * 1.0 - maximum force (full step drive)
 * use negative values for reverse direction
 */
void StepperMotor::setForce(float force)
{
    std::map<uint8_t, std::vector<uint8_t>> channelData;
    if(force == 0.0f)
    {
        // stop motor -> set all PWM channels to constant low
        for(uint8_t output = 0; output < 2; output++)   // motor outputs (coils) 0 and 1
        {
            for(uint8_t channelIndex = 0; channelIndex < 3; channelIndex++) // PWM, IN1 and IN2
            {
                channelData.emplace(DriverChannel[motorIndex][output][channelIndex], pDriver->ChannelLow);
            }
        }
    }
    else
    {
        // set PWM input of both channels to constant high
        channelData.emplace(DriverChannel[motorIndex][0][0], pDriver->ChannelHigh);
        channelData.emplace(DriverChannel[motorIndex][1][0], pDriver->ChannelHigh);
        auto delta = scaleValue<float>(0.0f, 1.0f, 1, 1023, fabs(force));
        uint16_t turnOn, turnOff;
        // data for channel 0 input IN1
        turnOn = 4096 - delta;
        turnOff = delta;
        channelData.emplace(DriverChannel[motorIndex][0][1], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
        // data for channel 0 input IN2
        turnOn = 2048 - delta;
        turnOff = 2048 + delta;
        channelData.emplace(DriverChannel[motorIndex][0][2], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});

        //XXX ad channels 3-12
        for(uint8_t i=3; i<13; i++)
        {
            channelData.emplace(i, pDriver->ChannelLow);
        }

        // data for channel 1 input IN1
        turnOn = (force > 0 ? 1024 : 3072) - delta;
        turnOff = (force > 0 ? 1024 : 3072) + delta;
        channelData.emplace(DriverChannel[motorIndex][1][1], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
        // data for channel 1 input IN2
        turnOn = (force > 0 ? 3072 : 1024) - delta;
        turnOff = (force > 0 ? 3072 : 1024) + delta;
        channelData.emplace(DriverChannel[motorIndex][1][2], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
    }

    uint8_t firstChannel = channelData.begin()->first;
    std::vector<uint8_t> dataToSend;
    for(auto element : channelData)
    {
        dataToSend.insert(dataToSend.end(), element.second.begin(), element.second.end());
    }
    pDriver->setChannels(firstChannel, dataToSend);
}
