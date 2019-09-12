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

extern USBD_HandleTypeDef hUsbDeviceFS; //XXX
USBD_HandleTypeDef lastUsbDeviceFS = hUsbDeviceFS; //XXX
			
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

    static Timer tm;
    tm.reset();
    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();
        System::getInstance().getConsole()->handler();
        I2cBus::pI2c1->handler();
        I2cBus::pI2c2->handler();
        System::getInstance().getYoke()->handler();

//        System::getInstance().getDisplay()->test();
//        System::getInstance().getDisplay()->handler();

        if((hUsbDeviceFS.dev_config_status != lastUsbDeviceFS.dev_config_status) ||     //XXX
                (hUsbDeviceFS.ep0_state != lastUsbDeviceFS.ep0_state) ||
                (hUsbDeviceFS.dev_state != lastUsbDeviceFS.dev_state) ||
                (hUsbDeviceFS.dev_connection_status != lastUsbDeviceFS.dev_connection_status))
        {
            lastUsbDeviceFS = hUsbDeviceFS;
            std::string stat = "st: ";
            stat += Console::toHex(lastUsbDeviceFS.dev_config_status, 8, false) + "  ";
            stat += Console::toHex(lastUsbDeviceFS.ep0_state, 8, false) + "  ";
            stat += Console::toHex(lastUsbDeviceFS.dev_state, 2, false) + "  ";
            stat += Console::toHex(lastUsbDeviceFS.dev_connection_status,2,false) + "  ";
            stat += std::to_string(tm.elapsed());
            System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB,stat);
        }
    }

    System::getInstance().terminate();
}
