/*
 * Electromagnet.h
 *
 *  Created on: 20.09.2019
 *      Author: Marcin
 */

#ifndef ELECTROMAGNET_H_
#define ELECTROMAGNET_H_

#include "PCA9685.h"

class Electromagnet
{
public:
    Electromagnet(PCA9685* pDriver, uint8_t magnetIndex);
    void setForce(float force);
private:
    PCA9685* pDriver;
    uint8_t magnetIndex;
    // PCA9685 driver channel assignment
    // indexes: [magnetIndex=0..3][channelIndex=0(PWM),1(IN1),2(IN2)]
    // value in array = number of PCA9685 PWM channel (0..15)
    const uint8_t DriverChannel[4][3] =
    {
            {8, 10, 9},     // magnet 0 labeled as motor 1
            {13, 11, 12},   // magnet 1 labeled as motor 2
            {2, 4, 3},      // magnet 2 labeled as motor 3
            {7, 5, 6}       // magnet 3 labeled as motor 4
    };
};

#endif /* ELECTROMAGNET_H_ */
