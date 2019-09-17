/*
 * StepperMotor.h
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

#include "PCA9685.h"

class StepperMotor
{
public:
    StepperMotor(PCA9685* pDriver, uint8_t motorIndex);
    void setForce(float force);
private:
    PCA9685* pDriver;
    uint8_t motorIndex;
    // PCA9685 driver channel assignment
    // indexes: [motorIndex=0,1][output=0(A),1(B)][channelIndex=0(PWM),1(IN1),2(IN2)]
    // value in array = number of PCA9685 PWM channel (0..15)
    const uint8_t DriverChannel[2][2][3] =
    {
//        // motorIndex 0
//        {
//            // output A
//            {8, 10, 9},
//            // output B
//            {13, 11, 12}
//        },
//        // motorIndex 1
//        {
//            // output A
//            {2, 4, 3},
//            // output B
//            {7, 5, 6}
//        }
// XXX data for testing:
            // motorIndex 0
            {
                // output A
                {2, 0, 1},
                // output B
                {13, 14, 15}
            },
            // motorIndex 1
            {
                // output A
                {2, 4, 3},
                // output B
                {7, 5, 6}
            }
    };
};

#endif /* STEPPERMOTOR_H_ */
