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
          uint16_t turnOn, turnOff;
//        // set PWM input of both channels to constant high
//        channelData.emplace(DriverChannel[motorIndex][0][0], pDriver->ChannelHigh);
//        channelData.emplace(DriverChannel[motorIndex][1][0], pDriver->ChannelHigh);
//        auto delta = scaleValue<float>(0.0f, 1.0f, 1, 1023, fabs(force));
//        // data for channel 0 input IN1
//        turnOn = 0;
//        turnOff = delta * 2;
//        channelData.emplace(DriverChannel[motorIndex][0][1], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
//        // data for channel 0 input IN2
//        turnOn = 2048;
//        turnOff = 2048 + delta * 2;
//        channelData.emplace(DriverChannel[motorIndex][0][2], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
//        // data for channel 1 input IN1
//        turnOn = (force > 0 ? 1024 : 3072);
//        turnOff = ((force > 0 ? 1024 : 3072) + delta * 2) & 0xFFF;
//        channelData.emplace(DriverChannel[motorIndex][1][1], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
//        // data for channel 1 input IN2
//        turnOn = (force > 0 ? 3072 : 1024);
//        turnOff = ((force > 0 ? 3072 : 1024) + delta * 2) & 0xFFF;
//        channelData.emplace(DriverChannel[motorIndex][1][2], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});

        // XXX test of driving a coil with PWM - to be removed when checked
        // driving motorIndex=0, output=0 (named Motor1 on the board)
        // set PWM input to desired duty cycle
        turnOn = 0;
        turnOff = scaleValue<float>(0.0f, 1.0f, 1, 4095, force);
        channelData.emplace(DriverChannel[motorIndex][0][0], std::vector<uint8_t>{LOBYTE(turnOn), HIBYTE(turnOn), LOBYTE(turnOff), HIBYTE(turnOff)});
        // data for channel 0 input IN1 - set to high
        channelData.emplace(DriverChannel[motorIndex][0][1], pDriver->ChannelHigh);
        // data for channel 0 input IN2 - set to low
        channelData.emplace(DriverChannel[motorIndex][0][2], pDriver->ChannelLow);
    }

    uint8_t firstChannel = channelData.begin()->first;
    std::vector<uint8_t> dataToSend;
    for(auto element : channelData)
    {
        dataToSend.insert(dataToSend.end(), element.second.begin(), element.second.end());
    }
    pDriver->setChannels(firstChannel, dataToSend);
}
