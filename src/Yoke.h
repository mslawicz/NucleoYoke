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
#include "Filter.h"
#include <string>

struct ForceFeedbackData
{
    uint8_t counter;    // frame counter
    float pitchForce;   // pitch force to yoke expressed in lbs
    float rollForce;    // roll force to yoke expressed in lbs
    float yawForce;     // yaw force to yoke expressed in lbs
    float flapsDeflection;  // flaps deflection ratio 0..1
    int32_t isRetractable;  // !=0 if any gear is retractable
    float gear1Deflection;  // gear 1 deflection ratio 0..1
    float gear2Deflection;  // gear 2 deflection ratio 0..1
    float gear3Deflection;  // gear 3 deflection ratio 0..1
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
    void displayForceFeedbackData(void);
private:
    int16_t toInt16(float value, int16_t maxValue);
    void computeParameters(void);
    void sendJoystickData(void);
    USB::Device interface;      // USB interface of this yoke
    ForceFeedbackData forceFeedbackData;    // force feedback data read from PC
    LSM6DS3 sensorAG;     // gyroscope and accelerometer sensor
    FloatVector angularRate;    // angular rate measured [rad/s]
    FloatVector acceleration;   // acceleration measured [g]
    float dTheta;   // pitch angle derivative [rad/s]
    float dPhi;     // roll angle derivative [rad/s]
    float theta;    // joystick pitch angle [rad]
    float phi;      // joystick roll angle [rad]
    float alpha;    // complementary filter strength factor
    Timer calculationTimer;
    const int16_t JoystickXyzMaxValue = 0x0FFF;
    ADConverter adc;                // ADC converter object
    PCA9685 motorDriver;        // DC motor PWM controller for pitch and roll force feedback electromagnets
    Electromagnet pitchMagnet;    // force feedback electromagnet of pitch control
    Timer loopTimer;        // timer for triggering main Yoke handler loop
    const uint32_t loopPeriod = 20000;  // handler loop triggered every 20 ms
};

#endif /* YOKE_H_ */
