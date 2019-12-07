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

extern USBD_HandleTypeDef hUsbDeviceFS;

Yoke::Yoke() :
    interface(),
    mixtureFilter(0.1f),
    propellerFilter(0.1f),
    autoRudderGainFilter(0.1f),
    flapsUp(GPIOC, GPIO_PIN_11, GPIO_PinState::GPIO_PIN_SET),
    flapsDown(GPIOC, GPIO_PIN_10, GPIO_PinState::GPIO_PIN_SET),
    gearUp(GPIOF, GPIO_PIN_5, GPIO_PinState::GPIO_PIN_SET),
    gearDown(GPIOF, GPIO_PIN_4, GPIO_PinState::GPIO_PIN_SET),
    elevatorTrim(GPIOD, GPIO_PIN_4, GPIOD, GPIO_PIN_5, RotaryEncoderType::RET_single_slope, 3000),
    hatUp(GPIOG, GPIO_PIN_11, GPIO_PinState::GPIO_PIN_SET),
    hatDown(GPIOG, GPIO_PIN_13, GPIO_PinState::GPIO_PIN_SET),
    hatLeft(GPIOG, GPIO_PIN_10, GPIO_PinState::GPIO_PIN_SET),
    hatRight(GPIOG, GPIO_PIN_15, GPIO_PinState::GPIO_PIN_SET),
    hatMiddle(GPIOE, GPIO_PIN_6, GPIO_PinState::GPIO_PIN_SET),
    yokePitchServo(&Servo::hTim, TIM_CHANNEL_1, GPIOA, GPIO_PIN_6, GPIO_AF2_TIM3, 1000)
{
    forceFeedbackDataTimer.reset();
    forceFeedbackData = {0, {0, 0, 0}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    yokeMode = YokeMode::YM_force_feedback;
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
    // update encoders
    updateEncoders();

    if(loopTimer.elapsed(loopPeriod))
    {
        loopTimer.reset();

        // send yoke data to PC
        if(interface.isActive())
        {
            if(pcDataReceived)
            {
                // send buffered data to PC
                sendYokeData();
                pcDataReceived = false;
            }
//            else XXX joystick (report ID 1) disabled
//            {
//                // TODO joystick data is to be replaced entirely by the buffered data (report ID 3) in the future
//                sendJoystickData();
//            }
        }

        // update servo position
        setServos();
        // start new AD conversion set
        adc.startConversions();
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

/*
 * sends yoke data to PC using USB HID joystick report
 */
void Yoke::sendJoystickData(void)
{
    //XXX check hat switch
    uint8_t hat = System::getInstance().systemPushbutton.read() == GPIO_PinState::GPIO_PIN_SET ? 3 : 0; //XXX

    int16_t deflectionX = 0;
    int16_t deflectionY = 0;
    int16_t deflectionZ = 0;
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
            0, 0, 0, 0  //buttons
    };
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, reportBuffer, sizeof(reportBuffer));
}

/*
 * sends yoke data to PC using USB HID undefined buffer data
 */
void Yoke::sendYokeData(void)
{
    float fParameter;
    uint8_t sendBuffer[64] = {0x00};
    // bytes 8-11 for yoke pitch
    fParameter = 0.0f;
    memcpy(sendBuffer+8, &fParameter, sizeof(fParameter));
    // bytes 12-15 for yoke roll
    fParameter = 0.0f;
    memcpy(sendBuffer+12, &fParameter, sizeof(fParameter));
    // bytes 16-19 for rudder control
    fParameter = 0.0f;
    memcpy(sendBuffer+16, &fParameter, sizeof(fParameter));
    // bytes 20-23 for throttle control
    fParameter = 0.0f;
    memcpy(sendBuffer+20, &fParameter, sizeof(fParameter));
    // bytes 24-27 for mixture control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, mixtureFilter.getFilteredValue(adc.getConvertedValues()[2]));
    memcpy(sendBuffer+24, &fParameter, sizeof(fParameter));
    // bytes 28-31 for propeller control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, propellerFilter.getFilteredValue(adc.getConvertedValues()[3]));
    memcpy(sendBuffer+28, &fParameter, sizeof(fParameter));

    // bytes 4-7 is the bitfield data register (buttons, switches, encoders)
    uint32_t buttons = 0;
    buttons |= (static_cast<int>(flapsUp.hasChangedTo0()) << 0);    // bit 0 - flaps up (one shot switch)
    buttons |= (static_cast<int>(flapsDown.hasChangedTo0()) << 1);  // bit 1 - flaps down (one shot switch)
    buttons |= (static_cast<int>(gearUp.hasChangedTo0()) << 2);  // bit 2 - gear up (one shot switch)
    buttons |= (static_cast<int>(gearDown.hasChangedTo0()) << 3);  // bit 3 - gear down (one shot switch)
    auto trimInput = elevatorTrim.getState();
    buttons |= (static_cast<int>(trimInput == -1) << 4);  // bit 4 - elevator trim up (one shot switch)
    buttons |= (static_cast<int>(trimInput == 1) << 5);  // bit 5 - elevator trim down (one shot switch)
    buttons |= (static_cast<int>(hatUp.getState()) << 6);  // bit 6 - hat switch up (press and hold switch)
    buttons |= (static_cast<int>(hatDown.getState()) << 7);  // bit 7 - hat switch down (press and hold switch)
    buttons |= (static_cast<int>(hatRight.getState()) << 8);  // bit 8 - hat switch right (press and hold switch)
    buttons |= (static_cast<int>(hatLeft.getState()) << 9);  // bit 9 - hat switch left (press and hold switch)
    buttons |= (static_cast<int>(hatMiddle.hasChangedTo0()) << 10);  // bit 10 - hat switch middle (one shot switch)
    buttons |= (static_cast<int>(hatRight.doubleChangedTo0()) << 11);  // bit 11 - double click of hat switch right (one shot switch)
    buttons |= (static_cast<int>(hatLeft.doubleChangedTo0()) << 12);  // bit 12 - double click of hat switch left (one shot switch)
    memcpy(sendBuffer+4, &buttons, sizeof(buttons));

    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, sendBuffer, sizeof(sendBuffer));

    //XXX test
    static uint32_t lastButtons = 0;
    if(buttons != lastButtons)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Debug,LogChannel::LC_SYSTEM, "buttons=" + toHex(buttons, 8, true));
        lastButtons = buttons;
    }
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
 * updating rotary encoder states without clearing flags
 */
void Yoke::updateEncoders(void)
{
    elevatorTrim.handler();
}

/*
 * set control servos in desired position
 */
void Yoke::setServos(void)
{
    static uint32_t counter = 0;
    switch(yokeMode)
    {
    case YM_force_feedback:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, forceFeedbackData.totalPitch));
        break;
    case YM_spring:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, 0.0f));  // TODO add spring function here
        break;
    case YM_demo:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, 0.2f * sin(counter * 0.02f)));
        break;
    default:
        break;
    }
    counter++;
}
