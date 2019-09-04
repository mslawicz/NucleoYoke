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
			
int main(void)
{
    // HAL and clock initialization
    System::configMCU();
    // create system singleton object and initialize hardware
    System::getInstance().config();


    //I2cDevice testDevice(I2cBus::pI2c1, DeviceAddress::LSM9DS1_AG_ADD); //XXX


    // start reception of the first character
    System::getInstance().getConsole()->getInterface().startReception();

    // send first prompt
    System::getInstance().getConsole()->sendPrompt();

    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();

        System::getInstance().getConsole()->handler();
        I2cBus::pI2c1->handler();
        System::getInstance().getYoke()->handler();

//        System::getInstance().getDisplay()->test();
//        System::getInstance().getDisplay()->handler();
    }

    System::getInstance().terminate();
}
