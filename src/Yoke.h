/*
 * yoke.h
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */


#ifndef YOKE_H_
#define YOKE_H_

#include "USB.h"
#include "Timer.h"
#include "ADC.h"
#include "Filter.h"
#include "Servo.h"
#include "Switch.h"
#include "RotaryEncoder.h"
#include "Conversion.h"
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
     * 0 -> 1 if any gear is retractable
     * 1 -> 1 if stick shaker is on
     * 2 -> 1 if reverser is deployed
     */
    uint16_t booleanFlags;
    uint8_t gearDeflection[3]; // gear deflection state: 0-up, 1-on the way, 2-down; array of 3 gear units
    float flapsDeflection;  // flaps deflection ratio 0..1
    float totalPitch;   // total pitch control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float totalRoll;    // total roll control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float totalYaw;     // total yaw control input (sum of user yoke plus autopilot servo plus artificial stability) <-1.0f .. 1.0f>
    float throttle; // throttle position of the handle itself - this controls all the handles at once <0.0f .. 1.0f>
    float airSpeed; // aircraft airspeed in relation to its Vno <0.0f .. 1.0f+> (may exceed 1.0f)
    float propellerSpeed;   // propeller speed in [rpm]; the higher value of first 2 engines is used
};

/*
 * the structure of data to be displayed in LED indicators
 */
struct IndicatorData
{
    bool isRetractable;
    float flapsDeflection;  // flaps deflection ratio 0..1
    uint8_t gearDeflection[3];  // gear deflection state; array of 3 gear units
    bool reverserOn;    // true if reverser in switched on
};

class Yoke
{
public:
    Yoke();
    ~Yoke();
    void handler(void);
    USB::Device& getInterface(void) { return interface; }
    void forceFeedbackHandler(uint8_t* buffer);
    void displayForceFeedbackData(void);
    void changeMode(int8_t changeValue);
    std::string getYokeModeText(void) const { return modeText.find(yokeMode)->second; }
    YokeMode getYokeMode(void) const { return yokeMode; }
    void sendDataToIndicators(bool force = false);
private:
    void sendYokeData(void);
    void updateEncoders(void);
    void setServos(void);
    USB::Device interface;      // USB interface of this yoke
    ForceFeedbackData forceFeedbackData;    // force feedback data read from PC
    ADConverter adc;                // ADC converter object
    Timer loopTimer;        // timer for triggering main Yoke handler loop
    const uint32_t loopPeriod = 10000;  // handler loop triggered every 10 ms
    Timer forceFeedbackDataTimer;
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
    bool pcDataReceived;
    bool ffchannelActive;   // true if FF data are being received
    Switch flapsUp;
    Switch flapsDown;
    Switch gearUp;
    Switch gearDown;
    RotaryEncoder elevatorTrim;
    Switch hatUp;
    Switch hatDown;
    Switch hatLeft;
    Switch hatRight;
    Switch hatMiddle;
    Servo yokePitchServo;
};

#endif /* YOKE_H_ */
