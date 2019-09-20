/*
 * yoke.h
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */


#ifndef YOKE_H_
#define YOKE_H_

#include "USB.h"
#include "LSM6DS3.h"
#include "Timer.h"
#include "ADC.h"
#include "PCA9685.h"
#include "Electromagnet.h"
#include <string>

struct ForceFeedbackData
{
    float pitchForce;
    float rollForce;
    float yawForce;
};

struct ImuRawData
{
    int16_t gyroscopeX;
    int16_t gyroscopeY;
    int16_t gyroscopeZ;
    int16_t accelerometerX;
    int16_t accelerometerY;
    int16_t accelerometerZ;
};

struct FloatVector
{
    float X;
    float Y;
    float Z;
};

class Yoke
{
public:
    Yoke();
    ~Yoke();
    void handler(void);
    USB::Device& getInterface(void) { return interface; }
    void forceFeedbackHandler(uint8_t* buffer);
    void resetParameters(void);
private:
    int16_t toInt16(float value, int16_t maxValue);
    void computeParameters(void);
    void sendJoystickData(void);
    USB::Device interface;      // USB interface of this yoke
    ForceFeedbackData forceFeedbackData;    // force feedback data read from PC
    LSM6DS3 imu;    // IMU sensor
    ImuRawData imuRawData;      // raw data read from IMU sensor
    FloatVector angularRate;    // angular rate measured [rad/s]
    FloatVector acceleration;   // acceleration measured [g]
    const int16_t MeasurementRegisterFullScaleValue = 0x7FFF;     // IMU measurement full scale value
    const float AngularRateFullScale = 4.3633f;   // gyroscope angular rate full scale value [rad/s]
    const float AccelerationFullScale = 2.0f;   // accelerometer full scale value [g]
    float dTheta;   // pitch angle derivative [rad/s]
    float dPhi;     // roll angle derivative [rad/s]
    float theta;    // yoke pitch angle [rad]
    float phi;      // yoke roll angle [rad]
    float alpha;    // complementary filter strength factor
    Timer calculationTimer;
    const int16_t JoystickXyzMaxValue = 0x0FFF;
    ADConverter adc;                // ADC converter object
    bool waitingForImuData;         // true when yoke is waiting for new data from INU sensor
    PCA9685 motorDriver;        // DC motor PWM controller for pitch and roll force feedback electromagnets
    Electromagnet pitchMagnet;    // force feedback electromagnet of pitch control
};

#endif /* YOKE_H_ */
