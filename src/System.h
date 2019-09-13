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

#define TEST1_PORT   GPIOD
#define TEST1_PIN    GPIO_PIN_0
#define TEST2_PORT   GPIOD
#define TEST2_PIN    GPIO_PIN_1


class System
{
public:
    ~System();
    static void configMCU(void);
    static System& getInstance(void);
    void errorHandler(void) { errorLED.write(GPIO_PinState::GPIO_PIN_SET); };
    void config(void);
    void terminate(void);
    void blinkSystemLED(void);
    Console* getConsole(void) const { return pConsole; }
    Yoke* getYoke(void) const { return pYoke; }
//    Display* getDisplay(void) const { return pDisplay; }
    GPIO systemLED;
    GPIO errorLED;
    GPIO systemPushbutton;
    GPIO testPin1; //XXX
    GPIO testPin2; //XXX
private:
    System();
    Console* pConsole;
    Yoke* pYoke;
    I2cBus* pI2C1;
    I2cBus* pI2C2;
    SpiBus* pSPI3;
};

#endif /* SYSTEM_H_ */
