/*
 * StepperMotor.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "StepperMotor.h"

StepperMotor::StepperMotor(PCA9685* pDriver, uint8_t motorIndex) :
    pDriver(pDriver),
    motorIndex(motorIndex)
{


}
