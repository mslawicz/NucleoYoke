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
#include "LSM6DS3.h"
			
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

    LSM6DS3 imu(I2cBus::pI2c1); //XXX
    imu.readRequest(DeviceAddress::LSM6DS3_ADD, 0x0F, 1); //XXX what to do with this device address?

    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();

        System::getInstance().getConsole()->handler();
        I2cBus::pI2c1->handler();

        System::getInstance().getYoke()->getInterface().test(); //XXX
//        System::getInstance().getDisplay()->test();
//        System::getInstance().getDisplay()->handler();
    }

    System::getInstance().terminate();
}
