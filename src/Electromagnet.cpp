/*
 * Electromagnet.cpp
 *
 *  Created on: 20.09.2019
 *      Author: Marcin
 */

#include "Electromagnet.h"
#include "Conversion.h"
#include <vector>
#include <map>
#include <cmath>


/*
 * electromagnet constructor
 * pDriver - pointer to PCA9685 controller object
 * magnetIndex - index of one of four electromagnets controlled by a PCA9685 controller
 */
Electromagnet::Electromagnet(PCA9685* pDriver, uint8_t magnetIndex) :
    pDriver(pDriver),
    magnetIndex(magnetIndex)
{


}


/*
 * set electromagnet force by controlling PWM waves
 * input value -1..+1
 * 0.0 - electromagnet off
 */
void Electromagnet::setForce(float force)
{
    std::map<uint8_t, std::vector<uint8_t>> channelData;
    if(force == 0.0f)
    {
        // electromagnet off -> set all PWM channels to constant low
        for(uint8_t channelIndex = 0; channelIndex < 3; channelIndex++) // PWM, IN1 and IN2
        {
            channelData.emplace(DriverChannel[magnetIndex][channelIndex], pDriver->ChannelLow);
        }
    }
    else
    {
        uint16_t turnOff;
        // set PWM input to desired duty cycle
        turnOff = scaleValue<float>(0.0f, 1.0f, 1, 4095, fabs(force));
        channelData.emplace(DriverChannel[magnetIndex][0], std::vector<uint8_t>{0, 0, LOBYTE(turnOff), HIBYTE(turnOff)});
        // set input IN1
        channelData.emplace(DriverChannel[magnetIndex][1], force > 0 ? pDriver->ChannelHigh : pDriver->ChannelLow);
        // dset input IN2
        channelData.emplace(DriverChannel[magnetIndex][2], force > 0 ? pDriver->ChannelLow : pDriver->ChannelHigh);
    }

    uint8_t firstChannel = channelData.begin()->first;
    std::vector<uint8_t> dataToSend;
    for(auto element : channelData)
    {
        dataToSend.insert(dataToSend.end(), element.second.begin(), element.second.end());
    }
    pDriver->setChannels(firstChannel, dataToSend);
}
