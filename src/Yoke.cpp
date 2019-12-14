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
    yokePitchServo(&Servo::hTim, TIM_CHANNEL_1, GPIOC, GPIO_PIN_6, GPIO_AF2_TIM3, 1500, 1000, 2050),
    yokeRollServo(&Servo::hTim, TIM_CHANNEL_2, GPIOB, GPIO_PIN_5, GPIO_AF2_TIM3, 1500, 850, 2150),
    throttleFilter(0.2f),    // XXX temporary solution
    centerView(GPIOA, GPIO_PIN_10, GPIO_PinState::GPIO_PIN_SET),
    yokePitchTensometer(GPIOF, GPIO_PIN_14, GPIOD, GPIO_PIN_15),
    viewJoystickRefX(2126, 100, 20, 100000),
    viewJoystickRefY(2116, 100, 20, 100000)
{
    forceFeedbackDataTimer.reset();
    forceFeedbackData = {0, {0, 0, 0}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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
    // call all tensometer handlers
    tensometerHandlers();

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

    forceFeedbackData.booleanFlags = *(buffer+1);
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
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, throttleFilter.getFilteredValue(adc.getConvertedValues()[1])); //XXX to be replaced with tensometer input
    memcpy(sendBuffer+20, &fParameter, sizeof(fParameter));
    // bytes 24-27 for mixture control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, mixtureFilter.getFilteredValue(adc.getConvertedValues()[2]));
    memcpy(sendBuffer+24, &fParameter, sizeof(fParameter));
    // bytes 28-31 for propeller control
    fParameter = scaleValue<float, float>(0.0f, 4096.0f, 0.0f, 1.0f, propellerFilter.getFilteredValue(adc.getConvertedValues()[3]));
    memcpy(sendBuffer+28, &fParameter, sizeof(fParameter));
    // bytes 32-35 for analog joystick Y (pilot's view yaw)
    viewJoystickRefY.calculateReference(adc.getConvertedValues()[5]);
    float inputY = adc.getConvertedValues()[5];
    fParameter = inputY > viewJoystickRefY.getReference() ? scaleValue<float, float>(viewJoystickRefY.getReference() + 20.0f, 4095.0f, 0.0f, 1.0f, inputY) :
            scaleValue<float, float>(0.0f, viewJoystickRefY.getReference() - 20.0f, -1.0f, 0.0f, inputY);
    memcpy(sendBuffer+32, &fParameter, sizeof(fParameter));
    // bytes 36-39 for analog joystick X (pilot's view pitch)
    viewJoystickRefX.calculateReference(adc.getConvertedValues()[6]);
    float inputX = adc.getConvertedValues()[6];
    fParameter = inputX > viewJoystickRefX.getReference() ? scaleValue<float, float>(viewJoystickRefX.getReference() + 20.0f, 4095.0f, 0.0f, 1.0f, inputX) :
            scaleValue<float, float>(0.0f, viewJoystickRefX.getReference() - 20.0f, -1.0f, 0.0f, inputX);
    memcpy(sendBuffer+36, &fParameter, sizeof(fParameter));

    // bytes 4-7 is the bitfield data register (buttons, switches, encoders)
    uint32_t buttons = 0;
    buttons |= (static_cast<int>(flapsUp.hasChangedTo0()) << 0);    // bit 0 - flaps up (one shot switch)
    buttons |= (static_cast<int>(flapsDown.hasChangedTo0()) << 1);  // bit 1 - flaps down (one shot switch)
    buttons |= (static_cast<int>(gearUp.hasChangedTo0()) << 2);  // bit 2 - gear up (one shot switch)
    buttons |= (static_cast<int>(gearDown.hasChangedTo0()) << 3);  // bit 3 - gear down (one shot switch)
    buttons |= (static_cast<int>(centerView.hasChangedTo0()) << 4);  // bit 4 - center pilot's view (analog joystick pushbutton) (one shot switch)
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
            (yokeMode != YokeMode::demo))
    {
        // data has changed
        // set flaps indicators
        System::getInstance().getRGBLeds()->setValue(0, forceFeedbackData.flapsDeflection > 0.0f ? WS2812Color::green : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(1, forceFeedbackData.flapsDeflection > 0.125f ? WS2812Color::yellow : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(2, forceFeedbackData.flapsDeflection > 0.25f ? WS2812Color::orange : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(3, forceFeedbackData.flapsDeflection > 0.375f ? WS2812Color::red : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(4, forceFeedbackData.flapsDeflection > 0.5f ? WS2812Color::magenta : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(5, forceFeedbackData.flapsDeflection > 0.625f ? WS2812Color::blue : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(6, forceFeedbackData.flapsDeflection > 0.75f ? WS2812Color::cyan : WS2812Color::off);
        System::getInstance().getRGBLeds()->setValue(7, forceFeedbackData.flapsDeflection == 1.0f ? WS2812Color::white : WS2812Color::off);

        // set gear indicators
        if(reverserOn)
        {
            System::getInstance().getRGBLeds()->setValue(9, WS2812Color::blue);     // nose gear indication
            System::getInstance().getRGBLeds()->setValue(10, WS2812Color::blue);    // left gear indication
            System::getInstance().getRGBLeds()->setValue(8, WS2812Color::blue);     // right gear indication
        }
        else if(isRetractable)
        {
            // this aircraft has retractable gear
            auto getGearColor = [](uint8_t deflection)
            {
                WS2812Color color;
                if(deflection == 0)
                {
                    color = WS2812Color::off;
                }
                else if(deflection == 2)
                {
                    color = WS2812Color::green;
                }
                else
                {
                    color = WS2812Color::red;
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
            System::getInstance().getRGBLeds()->setValue(9, WS2812Color::gray);     // nose gear indication
            System::getInstance().getRGBLeds()->setValue(10, WS2812Color::gray);    // left gear indication
            System::getInstance().getRGBLeds()->setValue(8, WS2812Color::gray);     // right gear indication
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
    yokeMode = static_cast<YokeMode>((static_cast<uint8_t>(yokeMode) + static_cast<uint8_t>(YokeMode::end) + changeValue) % static_cast<uint8_t>(YokeMode::end));
    sendDataToIndicators(true);
}
;

/*
 * updating rotary encoder states without clearing flags
 */
void Yoke::updateEncoders(void)
{
    //elevatorTrim.handler(); XXX not used any more
}

/*
 * set control servos in desired position
 */
void Yoke::setServos(void)
{
    static uint32_t counter = 0;
    switch(yokeMode)
    {
    case YokeMode::force_feedback:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, forceFeedbackData.totalPitch));
        yokeRollServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, -forceFeedbackData.totalRoll));
        break;
    case YokeMode::spring:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, 0.0f));  // TODO add spring function here
        break;
    case YokeMode::demo:
        yokePitchServo.setValue(scaleValue<float, float>(-1.0f, 1.0f, 0.0f, 1.0f, 0.2f * sin(counter * 0.02f)));
        break;
    default:
        break;
    }
    counter++;
}

/*
 * call all tensometer handlers here
 */
void Yoke::tensometerHandlers(void)
{
    yokePitchTensometer.handler();
}
