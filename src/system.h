/*
 * system.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "gpio.h"
#include "console.h"
#include "yoke.h"
//#include "display.h"

#define TEST_PORT   GPIOA
#define TEST_PIN    GPIO_PIN_1

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
    GPIO testPin;
    GPIO systemLED;
    GPIO errorLED;
    GPIO systemPushbutton;
private:
    System();
    Console* pConsole;
    Yoke* pYoke;
//    Display* pDisplay;
//    SpiBus* pSpi3;
};

#endif /* SYSTEM_H_ */
