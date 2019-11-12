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
#include "Decoder.h"
#include <string>
#include <unordered_map>

enum YokeMode
{
    YM_force_feedback,
    YM_spring,
    YM_demo,
    YM_end
};

struct ForceFeedbackData
{
    /*
     * bits of boolean flags:
     * 0 -> true if any gear is retractable
     */
    uint8_t booleanFlags;
    uint8_t gearDeflection[3]; // gear deflection state: 0-up, 1-on the way, 2-down; array of 3 gear units
    float flapsDeflection;  // flaps deflection ratio 0..1
    float totalPitch;   // total pitch control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float totalRoll;    // total roll control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float totalYaw;     // total yaw control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float throttle; // throttle position of the handle itself - this controls all the handles at once <0.0f .. 1.0f>
    float airSpeed; // aircraft airspeed in relation to its Vno <0.0f .. 1.0f+> (may exceed 1.0f)
};

/*
 * the structure of data to be displayed in LED indicators
 */
struct IndicatorData
{
    bool isRetractable;
    float flapsDeflection;  // flaps deflection ratio 0..1
    uint8_t gearDeflection[3];  // gear deflection state; array of 3 gear units
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
    void registerButtonDecoders(void);
    void changeMode(int8_t changeValue);
    std::string getYokeModeText(void) const { return modeText.find(yokeMode)->second; }
    YokeMode getYokeMode(void) const { return yokeMode; }
    void sendDataToIndicators(bool force = false);
private:
    void updateButtons(void);
    int16_t toInt16(float value, int16_t maxValue);
    void computeParameters(void);
    void sendJoystickData(void);
    void setJoystickForces(void);
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
    PCA9685 motorDriverBottom;      // DC motor PWM controller for north and south force feedback electromagnets
    PCA9685 motorDriverTop;         // DC motor PWM controller for east, center and west force feedback electromagnets
    Electromagnet electromagnet[5];    // array of force feedback electromagnets
    Timer loopTimer;        // timer for triggering main Yoke handler loop
    const uint32_t loopPeriod = 20000;  // handler loop triggered every 20 ms
    Timer forceFeedbackDataTimer;
    uint32_t buttons;       // 32 yoke buttons
    uint32_t buttonCleanMask;   // mask for deferred button clean out
    bool buttonCleanRequest;
    Timer buttonCleanTimer;
    const uint32_t buttonCleanDelay = 100000;   // time which must elapse between last expander update and button clean out
    EMA rudderFilter;
    EMA thrustFilter;
    EMA mixtureFilter;
    EMA propellerFilter;
    EMA autoRudderGainFilter;
    IndicatorData indicatorData;    // stores data to be displayed in RGB LED indicators
    YokeMode yokeMode;
    const std::unordered_map<YokeMode, std::string> modeText{
            {YokeMode::YM_force_feedback, "FF"},
            {YokeMode::YM_spring, "spring"},
            {YokeMode::YM_demo, "demo"}
    };
};

#endif /* YOKE_H_ */
