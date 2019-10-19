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
    forceFeedbackDataTimer.reset();
    forceFeedbackData = {0, 0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    buttons = 0;
    buttonCleanMask = 0x00000000;
    buttonCleanRequest = false;
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
        // update joystick button data
        updateButtons();
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
        // switch data LED off if no force feedback data are being received during 0.5 sec
        if(forceFeedbackDataTimer.elapsed(500000))
        {
            System::getInstance().dataLED.write(GPIO_PinState::GPIO_PIN_RESET);
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
        forceFeedbackData.booleanFlags = *reinterpret_cast<uint32_t*>(buffer+2);
        forceFeedbackData.pitchForce = *reinterpret_cast<float*>(buffer+6);
        forceFeedbackData.rollForce = *reinterpret_cast<float*>(buffer+10);
        forceFeedbackData.yawForce = *reinterpret_cast<float*>(buffer+14);
        forceFeedbackData.flapsDeflection = *reinterpret_cast<float*>(buffer+18);
        forceFeedbackData.gearDeflection[0] = *reinterpret_cast<float*>(buffer+22);
        forceFeedbackData.gearDeflection[1] = *reinterpret_cast<float*>(buffer+26);
        forceFeedbackData.gearDeflection[2] = *reinterpret_cast<float*>(buffer+30);

        if((forceFeedbackData.pitchForce != 0.0f) || (forceFeedbackData.rollForce != 0.0f))
        {
            // yoke force data is received
            System::getInstance().dataLED.write(GPIO_PinState::GPIO_PIN_SET);
            forceFeedbackDataTimer.reset();
        }
        else
        {
            // force data is zero (not supported in this aircraft)
            if(forceFeedbackDataTimer.elapsed(250000))
            {
                // data LED is blinking in this scenario
                System::getInstance().dataLED.toggle();
                forceFeedbackDataTimer.reset();
            }
        }
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
}

/*
 * register yoke button decoders
 */
void Yoke::registerButtonDecoders(void)
{
//    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new RotaryEncoder(0, 1, 10, 11, buttons)); // example of rotary encoder
//    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(2, 12, buttons));     // example of toggle switch
//    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(3, 13));     // example of direct button (not inverted)
//    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(4, 14, true));     // example of direct button (inverted)

    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(0, 0));   // gear up
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(1, 1));   // gear down
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(8, 2));   // flaps up
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new DirectButton(9, 3));   // flaps down
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(10, 4, 5, buttonCleanMask));     // toggle left of 3
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(11, 6, 7, buttonCleanMask));     // toggle centre of 3
    System::getInstance().getGpioExpanders()[0]->getDecoders().push_back(new ToggleSwitch(12, 8, 9, buttonCleanMask));     // toggle right of 3

    System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new ToggleSwitch(14, 24, 25, buttonCleanMask));     // toggle left of 2
    System::getInstance().getGpioExpanders()[1]->getDecoders().push_back(new ToggleSwitch(15, 26, 27, buttonCleanMask));     // toggle right of 2
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
            LOBYTE(deflectionX),    //joystick axis X
            HIBYTE(deflectionX),
            LOBYTE(deflectionY),    //joystick axis Y
            HIBYTE(deflectionY),
            LOBYTE(deflectionZ),    //joystick axis Z
            HIBYTE(deflectionZ),
            0,    //joystick axis Rx
            0,    //joystick axis Ry
            0,    //joystick axis Rz
            LOBYTE(scaleValue<int16_t>(0, 0xFFF, 0, 255, adc.getConvertedValues()[1])),    //joystick slider
            0,    //joystick dial
            0,    //joystick wheel
            0,    // HAT switch 1-8, 0=neutral
            static_cast<uint8_t>(buttons & 0xFF),         // buttons 0-7
            static_cast<uint8_t>((buttons >> 8) & 0xFF),  // buttons 8-15
            static_cast<uint8_t>((buttons >> 16) & 0xFF), // buttons 16-23
            static_cast<uint8_t>((buttons >> 24) & 0xFF)  // buttons 24-31
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

/*
 * displays force feedback data in the console window
 */
void Yoke::displayForceFeedbackData(void)
{
    System::getInstance().getConsole()->getInterface().send("pitch force = " + std::to_string(forceFeedbackData.pitchForce) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("roll force = " + std::to_string(forceFeedbackData.rollForce) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("yaw force = " + std::to_string(forceFeedbackData.yawForce) + "\r\n");
    System::getInstance().getConsole()->getInterface().send("flaps deflection = " + std::to_string(forceFeedbackData.flapsDeflection) + "\r\n");
    std::string isRetractableString = forceFeedbackData.booleanFlags & 0x00000001 ? "yes" : "no";
    System::getInstance().getConsole()->getInterface().send("is retractable? = " + isRetractableString + "\r\n");
    System::getInstance().getConsole()->getInterface().send("gear deflection = " + std::to_string(forceFeedbackData.gearDeflection[0]) + ", "
         + std::to_string(forceFeedbackData.gearDeflection[1]) + ", "
         + std::to_string(forceFeedbackData.gearDeflection[2]) + "\r\n");
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


