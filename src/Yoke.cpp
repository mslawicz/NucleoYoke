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
    motorDriverBottom(I2cBus::pI2c1, DeviceAddress::PCA9685_1_ADD),
    motorDriverTop(I2cBus::pI2c1, DeviceAddress::PCA9685_0_ADD),
    electromagnet
    {
        Electromagnet(&motorDriverBottom, 2),   // north
        Electromagnet(&motorDriverTop, 2),      // east
        Electromagnet(&motorDriverBottom, 3),   // south
        Electromagnet(&motorDriverTop, 3),      // west
        Electromagnet(&motorDriverTop, 0)       // center
    },
    mixtureFilter(0.1f),
    propellerFilter(0.1f),
    autoRudderGainFilter(0.1f)
{
    theta = phi = rudder = dTheta = dPhi = 0.0f;
    alpha = 0.02;
    forceFeedbackDataTimer.reset();
    forceFeedbackData = {0, {0, 0, 0}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    buttons = 0;
    buttonCleanMask = 0x00000000;
    buttonCleanRequest = false;
    yokeMode = YokeMode::YM_force_feedback;
    for(auto& item : electromagnet)
    {
        item.setForce(0);
    }
    pcDataReceived = false;
    ffchannelActive = false;
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
    // update joystick button data
    updateButtons();

    if(loopTimer.elapsed(loopPeriod))
    {
        loopTimer.reset();
        // compute yoke parameters after reception of new sensor data
        computeParameters();

        // send yoke data to PC
        if(interface.isActive())
        {
            if(pcDataReceived)
            {
                // send buffered data to PC
                sendYokeData();
                pcDataReceived = false;
            }
            else
            {
                // TODO joystick data is to be replaced entirely by the buffered data (report ID 3) in the future
                sendJoystickData();
            }
        }

        // apply new forces to joystick
        setJoystickForces();
        // start new AD conversion set
        adc.startConversions();
        // request data transmission from IMU sensors
        sensorAG.readNewData();
        // switch data LED off if no force feedback data are being received during 0.2 sec
        if(forceFeedbackDataTimer.elapsed(200000))
        {
            System::getInstance().dataLED.write(GPIO_PinState::GPIO_PIN_RESET);
            ffchannelActive = false;
        }
    }
}

/*
 * called in USB data reception callback function
 */
void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    if(buffer[0] == 0x03)
    {
        forceFeedbackData.booleanFlags = *(buffer+2);
        forceFeedbackData.gearDeflection[0] = ((*(buffer+3)) >> 0) & 0x03;
        forceFeedbackData.gearDeflection[1] = ((*(buffer+3)) >> 2) & 0x03;
        forceFeedbackData.gearDeflection[2] = ((*(buffer+3)) >> 4) & 0x03;
        forceFeedbackData.flapsDeflection = *reinterpret_cast<float*>(buffer+4);
        forceFeedbackData.totalPitch = *reinterpret_cast<float*>(buffer+8);
        forceFeedbackData.totalRoll = *reinterpret_cast<float*>(buffer+12);
        forceFeedbackData.totalYaw = *reinterpret_cast<float*>(buffer+16);
        forceFeedbackData.throttle = *reinterpret_cast<float*>(buffer+20);
        forceFeedbackData.airSpeed = *reinterpret_cast<float*>(buffer+24);
        forceFeedbackData.propellerSpeed = *reinterpret_cast<float*>(buffer+28);

        // yoke force data is received
        System::getInstance().dataLED.write(GPIO_PinState::GPIO_PIN_SET);
        forceFeedbackDataTimer.reset();

        // check new data and possibly send new values to LED indicators
        sendDataToIndicators();

        // mark that data from PC has been received
        pcDataReceived = true;
        ffchannelActive = true;
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Received unexpected HID report id=" + std::to_string(static_cast<int>(buffer[0])));
    }
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
    float thetaA = atan2(acceleration.X, acceleration.Z);
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

    // rudder deflection read from analog input #0, range -1..+1
    rudder = 2.0f * rudderFilter.getFilteredValue(adc.getConvertedValues()[0]) / 4095.0f - 1.0f;
    rudder *= 0.0f; // XXX rudder signal is not available yet
    // autorudder deflection calculated from phi (roll input) and gain
    float autoRudderGain = scaleValue<int16_t, float>(40, 0xFFF, 0.0f, 1.0f, autoRudderGainFilter.getFilteredValue(adc.getConvertedValues()[6]));
    // gain is squared to achieve semi-exponential curve
    float autoRudder = phi * autoRudderGain * autoRudderGain;
    // autorudder is summed with rudder input
    rudder += autoRudder;
}

/*
 * register yoke button decoders
 */
void Yoke::registerButtonDecoders(void)
{
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(0, 0));   // gear up
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(1, 1));   // gear down
    //System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(8, 2));   // flaps up
    //System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(9, 3));   // flaps down
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(10, 4, 5, buttonCleanMask));     // toggle left of 3
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(11, 6, 7, buttonCleanMask));     // toggle centre of 3
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(12, 8, 9, buttonCleanMask));     // toggle right of 3
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(13, 10, true));   // reverser button (normal closed)
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new RotaryEncoderEx(5, 6, 11, 12, buttonCleanMask)); // elevator trim

    System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new RotaryEncoderEx(5, 6, 13, 14, buttonCleanMask)); // rudder trim
    //System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new RotaryEncoderEx(8, 9, 15, 16, buttonCleanMask)); // aileron trim
    //System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new DirectButton(10, 17));   // aileron trim pushbutton
    System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new ToggleSwitch(11, 24, 25, buttonCleanMask));     // toggle left of 2
    System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new ToggleSwitch(12, 26, 27, buttonCleanMask));     // toggle right of 2

    System::getInstance().getGpioExpanders()[2]->getDecoders().push_back(new DirectButton(0, 18));   // blue button
    System::getInstance().getGpioExpanders()[2]->getDecoders().push_back(new DirectButton(1, 19));   // yellow button
}

/*
 * sends yoke data to PC using USB HID joystick report
 */
void Yoke::sendJoystickData(void)
{
    //XXX check hat switch
    uint8_t hat = System::getInstance().systemPushbutton.read() == GPIO_PinState::GPIO_PIN_SET ? 3 : 0; //XXX

    int16_t deflectionX = scaleValue<float, int16_t>(-0.5f,0.5f, -JoystickXyzMaxValue, JoystickXyzMaxValue, phi);
    int16_t deflectionY = -scaleValue<float, int16_t>(-0.5f, 0.5f, -JoystickXyzMaxValue, JoystickXyzMaxValue, theta);
    int16_t deflectionZ = scaleValue<float, int16_t>(-1.0f, 1.0f, -JoystickXyzMaxValue, JoystickXyzMaxValue, rudder);
    uint8_t reportBuffer[] =
    {
            0x01,   // report ID
            LOBYTE(deflectionX),    //joystick axis X
            HIBYTE(deflectionX),
            LOBYTE(deflectionY),    //joystick axis Y
            HIBYTE(deflectionY),
            LOBYTE(deflectionZ),    //joystick axis Z
            HIBYTE(deflectionZ),
            0,    //joystick axis Rx
            0,    //joystick axis Ry
            0,    //joystick axis Rz
            LOBYTE((scaleValue<int16_t, int16_t>(0, 0xFFF, 0, 255, adc.getConvertedValues()[1]))),    //joystick slider - throttle
            LOBYTE((scaleValue<int16_t, int16_t>(0, 0xFFF, 0, 255, adc.getConvertedValues()[2]))),    //joystick dial - mixture
            LOBYTE((scaleValue<int16_t, int16_t>(0, 0xFFF, 0, 255, adc.getConvertedValues()[3]))),    //joystick wheel - propeller
            hat, //XXX 0,    // HAT switch 1-8, 0=neutral
            static_cast<uint8_t>(buttons & 0xFF),         // buttons 0-7
            static_cast<uint8_t>((buttons >> 8) & 0xFF),  // buttons 8-15
            static_cast<uint8_t>((buttons >> 16) & 0xFF), // buttons 16-23
            static_cast<uint8_t>((buttons >> 24) & 0xFF)  // buttons 24-31
    };
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, reportBuffer, sizeof(reportBuffer));
}

/*
 * sends yoke data to PC using USB HID undefined buffer data
 */
void Yoke::sendYokeData(void)
{
    float fParameter;
    uint8_t sendBuffer[64] = {0x03, 0x00};
    // bytes 8-11 for yoke pith
    fParameter = scaleValue<float, float>(-0.5f,0.5f, -1.0f, 1.0f, -theta);
    memcpy(sendBuffer+8, &fParameter, sizeof(fParameter));
    // bytes 12-15 for yoke roll
    fParameter = scaleValue<float, float>(-0.5f,0.5f, -1.0f, 1.0f, phi);
    memcpy(sendBuffer+12, &fParameter, sizeof(fParameter));
    // bytes 16-19 for rudder control
    fParameter = scaleValue<float, float>(-1.0f, 1.0f, -1.0f, 1.0f, rudder);
    memcpy(sendBuffer+16, &fParameter, sizeof(fParameter));
    // bytes 20-23 for throttle control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, thrustFilter.getFilteredValue(adc.getConvertedValues()[1]));
    memcpy(sendBuffer+20, &fParameter, sizeof(fParameter));
    // bytes 24-27 for mixture control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, mixtureFilter.getFilteredValue(adc.getConvertedValues()[2]));
    memcpy(sendBuffer+24, &fParameter, sizeof(fParameter));
    // bytes 28-31 for propeller control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, propellerFilter.getFilteredValue(adc.getConvertedValues()[3]));
    memcpy(sendBuffer+28, &fParameter, sizeof(fParameter));

    //XXX
    static uint32_t cnt = 0;
    uint32_t trsp = cnt++ % 100;
    memcpy(sendBuffer+4, &trsp, sizeof(trsp));

    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, sendBuffer, sizeof(sendBuffer));
}

/*
 * resets essential yoke parameters
 */
void Yoke::resetParameters(void)
{
    theta = phi = dTheta = dPhi = 0.0f;
    calculationTimer.reset();
}

/*
 * displays force feedback data in the console window
 */
void Yoke::displayForceFeedbackData(void)
{
    auto boolToYN = [](bool val)->std::string{ return (val ? "yes" : "no"); };
    System::getInstance().getConsole()->getInterface().send("total pitch = " + std::to_string(forceFeedbackData.totalPitch) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("total roll = " + std::to_string(forceFeedbackData.totalRoll) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("total yaw = " + std::to_string(forceFeedbackData.totalYaw) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("flaps deflection = " + std::to_string(forceFeedbackData.flapsDeflection) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("is retractable? = " + boolToYN(forceFeedbackData.booleanFlags & 0x01) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("gear deflection = " + std::to_string(forceFeedbackData.gearDeflection[0]) + ", "
         + std::to_string(forceFeedbackData.gearDeflection[1]) + ", "
         + std::to_string(forceFeedbackData.gearDeflection[2]) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("relative airspeed = " + std::to_string(forceFeedbackData.airSpeed) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("throttle = " + std::to_string(forceFeedbackData.throttle) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("stick shaker on? = " + boolToYN(forceFeedbackData.booleanFlags & (1 << 1)) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("reverser on? = " + boolToYN(forceFeedbackData.booleanFlags & (1 << 2)) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("propeller speed = " + std::to_string(forceFeedbackData.propellerSpeed) + " [rpm]\r\n");
}

/*
 * update yoke buttons according to GPIO expander data
 * to be called in yoke handler
 */
void Yoke::updateButtons(void)
{
    for(auto& pExpander : System::getInstance().getGpioExpanders())
    {
        bool updateRequested = pExpander->handler();
        if(updateRequested)
        {
            for(auto& pDecoder : pExpander->getDecoders())
            {
                if(pDecoder->decode(pExpander->getInputRegister(), buttons))
                {
                    buttonCleanRequest = true;
                    buttonCleanTimer.reset();
                }
            }

            System::getInstance().getConsole()->sendMessage(Severity::Debug,LogChannel::LC_EXP,
                    "Expander addr=" + toHex(pExpander->getDeviceAddress(), 2, true) +
                    " value=" + toHex(pExpander->getInputRegister(), 4, true) +
                    " buttons=" + toHex(buttons, 8, true));
        }
    }

    if(buttonCleanRequest && buttonCleanTimer.elapsed(buttonCleanDelay))
    {
        buttonCleanRequest = false;
        // clean all decoder signals
        buttons &= ~buttonCleanMask;
        System::getInstance().getConsole()->sendMessage(Severity::Debug,LogChannel::LC_EXP,
                            "Clean mask=" + toHex(buttonCleanMask, 8, true) +
                            " clean out buttons=" + toHex(buttons, 8, true));
    }
}

/*
 * check if particular data has changed
 * if so, send new values to LED indicators
 */
void Yoke::sendDataToIndicators(bool force)
{
    // check if particular data has changed
    bool isRetractable = (forceFeedbackData.booleanFlags & 0x01) != 0;
    bool reverserOn = (forceFeedbackData.booleanFlags & (1 << 2)) != 0;
    if(((indicatorData.isRetractable != isRetractable) ||
            (indicatorData.flapsDeflection != forceFeedbackData.flapsDeflection) ||
            (indicatorData.gearDeflection[0] != forceFeedbackData.gearDeflection[0]) ||
            (indicatorData.gearDeflection[1] != forceFeedbackData.gearDeflection[1]) ||
            (indicatorData.gearDeflection[2] != forceFeedbackData.gearDeflection[2]) ||
            (indicatorData.reverserOn != reverserOn) ||
            force) &&
            (yokeMode != YokeMode::YM_demo))
    {
        // data has changed
        // set flaps indicators
        System::getInstance().getRGBLeds()->setValue(0, forceFeedbackData.flapsDeflection > 0.0f ? WS2812Color::Color_green : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(1, forceFeedbackData.flapsDeflection > 0.125f ? WS2812Color::Color_yellow : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(2, forceFeedbackData.flapsDeflection > 0.25f ? WS2812Color::Color_orange : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(3, forceFeedbackData.flapsDeflection > 0.375f ? WS2812Color::Color_red : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(4, forceFeedbackData.flapsDeflection > 0.5f ? WS2812Color::Color_magenta : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(5, forceFeedbackData.flapsDeflection > 0.625f ? WS2812Color::Color_blue : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(6, forceFeedbackData.flapsDeflection > 0.75f ? WS2812Color::Color_cyan : WS2812Color::Color_off);
        System::getInstance().getRGBLeds()->setValue(7, forceFeedbackData.flapsDeflection == 1.0f ? WS2812Color::Color_white : WS2812Color::Color_off);

        // set gear indicators
        if(reverserOn)
        {
            System::getInstance().getRGBLeds()->setValue(9, WS2812Color::Color_blue);     // nose gear indication
            System::getInstance().getRGBLeds()->setValue(10, WS2812Color::Color_blue);    // left gear indication
            System::getInstance().getRGBLeds()->setValue(8, WS2812Color::Color_blue);     // right gear indication
        }
        else if(isRetractable)
        {
            // this aircraft has retractable gear
            auto getGearColor = [](uint8_t deflection)
            {
                WS2812Color color;
                if(deflection == 0)
                {
                    color = WS2812Color::Color_off;
                }
                else if(deflection == 2)
                {
                    color = WS2812Color::Color_green;
                }
                else
                {
                    color = WS2812Color::Color_red;
                }
                return color;
            };

            System::getInstance().getRGBLeds()->setValue(9, getGearColor(forceFeedbackData.gearDeflection[0]));     // nose gear indication
            System::getInstance().getRGBLeds()->setValue(10, getGearColor(forceFeedbackData.gearDeflection[1]));    // left gear indication
            System::getInstance().getRGBLeds()->setValue(8, getGearColor(forceFeedbackData.gearDeflection[2]));     // right gear indication
        }
        else
        {
            // this aircraft has fixed gear
            System::getInstance().getRGBLeds()->setValue(9, WS2812Color::Color_gray);     // nose gear indication
            System::getInstance().getRGBLeds()->setValue(10, WS2812Color::Color_gray);    // left gear indication
            System::getInstance().getRGBLeds()->setValue(8, WS2812Color::Color_gray);     // right gear indication
        }

        // request the LED update
        System::getInstance().getRGBLeds()->requestUpdate();

        // store current values
        indicatorData.isRetractable = isRetractable;
        indicatorData.flapsDeflection = forceFeedbackData.flapsDeflection;
        indicatorData.gearDeflection[0] = forceFeedbackData.gearDeflection[0];
        indicatorData.gearDeflection[1] = forceFeedbackData.gearDeflection[1];
        indicatorData.gearDeflection[2] = forceFeedbackData.gearDeflection[2];
        indicatorData.reverserOn = reverserOn;
    }
}

/*
 * changes the yoke function mode
 */
void Yoke::changeMode(int8_t changeValue)
{
    yokeMode = static_cast<YokeMode>((yokeMode + YokeMode::YM_end + changeValue) % YokeMode::YM_end);
    sendDataToIndicators(true);
}
;

/*
 * sets joystick forces by driving electromagnets
 */
void Yoke::setJoystickForces(void)
{
    const float ZeroAttraction = 0.15f;   // force for zero electromagnet attraction
    float pitchForce = 0.0f;
    float rollForce = 0.0f;
    static EMA pitchFilter(0.05f);
    static EMA rollFilter(0.05f);

    if(yokeMode == YokeMode::YM_force_feedback)
    {
        // yoke in 'force feedback' mode
        pitchForce = (theta - forceFeedbackData.totalPitch) * forceFeedbackData.airSpeed;
        rollForce = (phi - forceFeedbackData.totalRoll) * forceFeedbackData.airSpeed * 2.0f;
    }
    else if(yokeMode == YokeMode::YM_spring)
    {
        // yoke in 'spring' mode or in auto without active FF data
        const float Amplitude = 2.5f;
        pitchForce = sin(theta);
        pitchForce *= pitchForce * Amplitude;
        rollForce = sin(phi);
        rollForce *= rollForce * Amplitude;
    }
    else
    {
        // yoke in demo mode
        static uint32_t cnt = 0;
        float angle = cnt++ / 5.0f;
        float amplitude = scaleValue<uint16_t, float>(0, 0xFFF, -1.0f, 1.0f, adc.getConvertedValues()[2]);
        pitchForce = 0.5f * amplitude * sin(angle);
        rollForce = amplitude * cos(angle);
    }

    float northForce = -pitchForce * cos(theta) + rollForce * sin(phi); // north electromagnet force value
    float eastForce = pitchForce * sin(theta) - rollForce * cos(phi);   // east electromagnet force value
    float southForce = pitchForce * cos(theta) + rollForce * sin(phi);  // south electromagnet force value
    float westForce = pitchForce * sin(theta) + rollForce * cos(phi);   // west electromagnet force value
    float centralForce = pitchForce * sin(theta) + rollForce * sin(phi) + ZeroAttraction;   // central electromagnet force value
    bool disableEM = (yokeMode == YokeMode::YM_force_feedback) && !ffchannelActive;
    electromagnet[0].setForce(disableEM ? 0.0f : northForce);
    electromagnet[1].setForce(disableEM ? 0.0f : eastForce);
    electromagnet[2].setForce(disableEM ? 0.0f : southForce);
    electromagnet[3].setForce(disableEM ? 0.0f : westForce);
    electromagnet[4].setForce(disableEM ? 0.0f : centralForce);
}
