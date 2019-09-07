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
#include <string>

enum YokeState
{
    YS_start,
    YS_wait_for_imu_data_ready,
    YS_wait_for_data_reception,
    YS_compute_imu_data,
    YS_send_yoke_data
};

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
private:
    int16_t toInt16(float value, int16_t maxValue);
    USB::Device interface;      // USB interface of this yoke
    ForceFeedbackData forceFeedbackData;    // force feedback data read from PC
    LSM6DS3 imu;    // IMU sensor
    YokeState state;    // state of the handler state machine
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
    const float thetaGain = 5000.0f;    // multiplier for achieving elevator deflection full scale
    const float phiGain = 6000.0f;      // multiplier for achieving aileron deflection full scale
};

#endif /* YOKE_H_ */
