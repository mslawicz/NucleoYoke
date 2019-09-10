/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "usbd_customhid.h"
#include "Yoke.h"
#include "System.h"
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
    imu(I2cBus::pI2c2)
{
    theta = phi = dTheta = dPhi = 0.0f;
    alpha = 0.02;
    waitingForImuData = false;
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
    if(imu.isDataReady() && !waitingForImuData)
    {
        // new data ready in IMU sensor which has not been yet requested
        waitingForImuData = true;
        // request data transmission from IMU sensor
        imu.getData();
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
    }

    if(imu.isNewDataReceived())
    {
        // new data from IMU sensor has been just received
        imu.markNewDataReceived(false);
        waitingForImuData = false;
        System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
        // copy IMU data from reception vector to IMU raw data structure
        memcpy(&imuRawData, &imu.getReceivedData()[0], imu.getReceivedData().size());
        // compute yoke parameters after reception of new sensor data
        computeParameters();
        // send yoke data to PC using USB HID joystick report
        sendJoystickData();
        // start new AD conversion set
        System::getInstance().testPin2.write(GPIO_PinState::GPIO_PIN_SET);    //XXX
        adc.startConversions();
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
    // angular rate and acceleration value calculations should reflect the sensor orientation
    // acceleration.Z == 1 when yoke is neutral
    // acceleration.Y > 0 when yoke is pulled (elevator up)
    // acceleration.X > 0 when yoke is turned right (right wing down)
    // angular rate sign must reflect acceleration sign
    angularRate.Y = static_cast<float>(imuRawData.gyroscopeX) / MeasurementRegisterFullScaleValue * AngularRateFullScale;
    angularRate.X = static_cast<float>(imuRawData.gyroscopeY) / MeasurementRegisterFullScaleValue * AngularRateFullScale;
    angularRate.Z = -static_cast<float>(imuRawData.gyroscopeZ) / MeasurementRegisterFullScaleValue * AngularRateFullScale;
    acceleration.Y = -static_cast<float>(imuRawData.accelerometerX) / MeasurementRegisterFullScaleValue * AccelerationFullScale;
    acceleration.X = static_cast<float>(imuRawData.accelerometerY) / MeasurementRegisterFullScaleValue * AccelerationFullScale;
    acceleration.Z = static_cast<float>(imuRawData.accelerometerZ) / MeasurementRegisterFullScaleValue * AccelerationFullScale;
    gGyro = angularRate; //XXX
    gAcc = acceleration; //XXX
    // calculate pitch angle derivative [rad/s]
    dTheta = angularRate.Y * cos(phi) + angularRate.Z * sin(phi);
    // calculate roll angle derivative [rad/s]
    dPhi = angularRate.X;
    // calculate pitch angle from accelerometer data
    float thetaA = atan2(acceleration.X, sqrt(acceleration.Z * acceleration.Z + acceleration.Y * acceleration.Y));
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
    int16_t deflectionX = toInt16(phi * phiGain, JoystickXyzMaxValue);
    int16_t deflectionY = toInt16(theta * thetaGain, JoystickXyzMaxValue);
    // rudder is derived from the first converted value
    // it must be converted from 12-bit unsigned to 13-bit signed
    int16_t deflectionZ = (adc.getConvertedValues()[0] << 1) - 0xFFF;
    uint8_t reportBuffer[] =
    {
            0x01,   // report ID
            LOBYTE(deflectionX),
            HIBYTE(deflectionX),
            LOBYTE(deflectionY),
            HIBYTE(deflectionY),
            LOBYTE(deflectionZ),
            HIBYTE(deflectionZ),
            0,0,0,0,0,0,0,0
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
