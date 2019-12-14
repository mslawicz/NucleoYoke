/*
 * system.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Console.h"
#include "GPIO.h"
#include "Yoke.h"
#include "I2C.h"
#include "SPI.h"
#include "Display.h"
#include "RGB.h"
#include "Servo.h"

#define TEST1_PORT   GPIOD
#define TEST1_PIN    GPIO_PIN_7
#define TEST2_PORT   GPIOE
#define TEST2_PIN    GPIO_PIN_3

enum class ErrorCode
{
    OK = 0,
    RccOscConfig,
    RccClockConfig,
    RccPeriphClkConfig,
    TimBase,
    TimConfigClockSource,
    TimMasterConfigSynchronization,
    UartInit,
    TimPwmInit
};

class System
{
public:
    ~System();
    static void configMCU(void);
    static System& getInstance(void);
    void errorHandler(ErrorCode code) { errorLED.write(GPIO_PinState::GPIO_PIN_SET); };
    void config(void);
    void terminate(void);
    void blinkSystemLED(void);
    Console* getConsole(void) const { return pConsole; }
    Yoke* getYoke(void) const { return pYoke; }
    Display* getDisplay(void) const { return pDisplay; }
    RGB* getRGBLeds(void) const { return pRGBLeds; }
    void displayStatus(void);
    void demoHandler(void);
    static ErrorCode initErrorCode;
    GPIO systemLED;
    GPIO errorLED;
    GPIO dataLED;
    GPIO systemPushbutton;
    GPIO testPin1; //XXX
    GPIO testPin2; //XXX
private:
    System();
    Console* pConsole {nullptr};
    Yoke* pYoke {nullptr};
    SpiBus* pSPI2 {nullptr};
    SpiBus* pSPI4 {nullptr};
    Display* pDisplay {nullptr};
    RGB* pRGBLeds {nullptr};
    const uint8_t NumberOfRgbLeds = 11;
};

#endif /* SYSTEM_H_ */
