/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
#include "System.h"
#include "Display.h"
#include "Timer.h"//XXX

			
int main(void)
{
    // HAL and clock initialization
    System::configMCU();
    // create system singleton object and initialize hardware
    System::getInstance().config();


    // start reception of the first character
    System::getInstance().getConsole()->getInterface().startReception();

    // send first prompt
    System::getInstance().getConsole()->sendPrompt();

    // reset essential yoke parameters before first handler call
    System::getInstance().getYoke()->resetParameters();

    Timer tm; //XXX
    tm.reset();

    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();
        System::getInstance().getConsole()->handler();
        I2cBus::pI2c1->handler();
        I2cBus::pI2c2->handler();
        SpiBus::pSpi3->handler();
        System::getInstance().getYoke()->handler();
        System::getInstance().getDisplay()->handler();

        if((System::getInstance().systemPushbutton.read()==GPIO_PinState::GPIO_PIN_SET) && (tm.elapsed(1000000)))
        {
            tm.reset();
            System::getInstance().getDisplay()->print(0, 0, "Nucleo Yoke", FontTahoma16b);
            System::getInstance().getDisplay()->print(10, 22, "by Marcin Slawicz", FontTahoma11);
            System::getInstance().getDisplay()->getController().requestUpdate();
        }
    }

    System::getInstance().terminate();
}
