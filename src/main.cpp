/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "system.h"
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
			
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

    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();

        System::getInstance().getConsole()->handler();
        System::getInstance().getYoke()->getInterface().test(); //XXX
//        System::getInstance().getDisplay()->test();
//        System::getInstance().getDisplay()->handler();
    }

    System::getInstance().terminate();
}
