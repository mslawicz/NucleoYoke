/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "usbd_customhid.h"
#include "Yoke.h"
#include "System.h"
#include "Conversion.h"
#include <cmath>

FloatVector gGyro; //XXX
FloatVector gAcc; //XXX
float gThetaA; //XXX
float gPhiA; //XXX
float gTheta; //XXX
float gPhi; //XXX

extern USBD_HandleTypeDef hUsbDeviceFS;

Yoke::Yoke() :
    interface(),
    sensorAG(I2cBus::pI2c2, DeviceAddress::LSM6DS3_ADD),
    motorDriver(I2cBus::pI2c1, DeviceAddress::PCA9685_ADD),
    pitchMagnet(&motorDriver, 0)
{
    theta = phi = dTheta = dPhi = 0.0f;
    alpha = 0.02;
    pitchMagnet.setForce(0.0f); //XXX
}

Yoke::~Yoke()
{
    // TODO Auto-generated destructor stub
}

/*
 * yoke handler
 * to be called in a main loop
 */
void Yoke::handler(void)
{
    if(loopTimer.elapsed(loopPeriod))
    {
        loopTimer.reset();
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
        // compute yoke parameters after reception of new sensor data
        computeParameters();
        // send yoke data to PC using USB HID joystick report
        if(interface.isActive())
        {
            sendJoystickData();
        }
        // start new AD conversion set
        adc.startConversions();
        // request data transmission from IMU sensors
        sensorAG.readNewData();

        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
    }
}

/*
 * called in USB data reception callback function
 */
void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    if(buffer[0] == 0x03)
    {
        memcpy(&forceFeedbackData, buffer+1, sizeof(ForceFeedbackData));
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Received unexpected HID report id=" + std::to_string(static_cast<int>(buffer[0])));
    }
}

int16_t Yoke::toInt16(float value, int16_t maxValue)
{
    int16_t outValue = static_cast<int16_t>(value);
    if(outValue > maxValue)
    {
        outValue = maxValue;
    }
    else if(outValue < -maxValue)
    {
        outValue = -maxValue;
    }
    return outValue;
}

/*
 * computes yoke parameters after receiving new data from sensors
 */
void Yoke::computeParameters(void)
{
    // IMU sensor must return data in north-east-down orientation and right hand rule
    angularRate = sensorAG.getAngularRate();
    acceleration = sensorAG.getAcceleration();
    gGyro = angularRate; //XXX
    gAcc = acceleration; //XXX

    // calculate pitch angle derivative [rad/s]
    dTheta = angularRate.Y;
    // calculate roll angle derivative [rad/s]
    dPhi = angularRate.X;

    // calculate pitch angle from accelerometer data
    float thetaA = -atan2(acceleration.X, acceleration.Z);
    // calculate roll angle from accelerometer data
    float phiA = atan2(acceleration.Y, acceleration.Z);

    gThetaA = thetaA; //XXX
    gPhiA = phiA; //XXX

    // time elapsed since the last calculation [s]
    float dt = 1e-6f * calculationTimer.elapsed();
    calculationTimer.reset();

    // calculate pitch angle using complementary filter [rad]
    theta = (1-alpha) * (theta + dTheta * dt) + alpha * thetaA;
    // calculate roll angle using complementary filter [rad]
    phi = (1-alpha) * (phi + dPhi * dt) + alpha * phiA;

    gTheta = theta; //XXX
    gPhi = phi; //XXX
}


/*
 * sends yoke data to PC using USB HID joystick report
 */
void Yoke::sendJoystickData(void)
{
    int16_t deflectionX = scaleValue<float>(-1.0f, 1.0f, -JoystickXyzMaxValue, JoystickXyzMaxValue, phi);
    int16_t deflectionY = scaleValue<float>(-0.5f, 0.5f, -JoystickXyzMaxValue, JoystickXyzMaxValue, theta);
    int16_t deflectionZ = scaleValue<uint16_t>(0, 0xFFF, -JoystickXyzMaxValue, JoystickXyzMaxValue, adc.getConvertedValues()[0]);
    uint8_t reportBuffer[] =
    {
            0x01,   // report ID
            LOBYTE(deflectionX),
            HIBYTE(deflectionX),
            LOBYTE(deflectionY),
            HIBYTE(deflectionY),
            LOBYTE(deflectionZ),
            HIBYTE(deflectionZ),
            0,0,0,
            LOBYTE(scaleValue<int16_t>(0, 0xFFF, 0, 255, adc.getConvertedValues()[1])),
            0,0,0,0,0,0,0
    };
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, reportBuffer, sizeof(reportBuffer));
}

/*
 * resets essential yoke parameters
 */
void Yoke::resetParameters(void)
{
    theta = phi = dTheta = dPhi = 0.0f;
    calculationTimer.reset();
}
