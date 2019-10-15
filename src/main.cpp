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
#include "WS2812.h"

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

    Timer tm, tl; //XXX
    tm.reset();
    std::vector<uint32_t> ledData;

    // main loop
    while(1)
    {
        System::getInstance().blinkSystemLED();
        System::getInstance().getConsole()->handler();
        I2cBus::pI2c1->handler();
        I2cBus::pI2c2->handler();
        SpiBus::pSpi2->handler();
        SpiBus::pSpi4->handler();
        System::getInstance().getYoke()->handler();
        System::getInstance().getDisplay()->handler();
        for(size_t index = 0; index < System::getInstance().getGpioExpanders().size(); index++)
        {
            bool updateRequested = System::getInstance().getGpioExpanders()[index]->handler();
            if(updateRequested)
            {
                // a new GPIO input data in this expander
                System::getInstance().getYoke()->updateButtons(index, System::getInstance().getGpioExpanders()[index]->getInputRegister());
            }
        }

        if((System::getInstance().systemPushbutton.read()==GPIO_PinState::GPIO_PIN_SET) || (tm.elapsed(500000))) //XXX
        {
            tm.reset();
            System::getInstance().getDisplay()->print(0, 0, "Nucleo Yoke", FontTahoma16b);
//            System::getInstance().getDisplay()->print(10, 22, "1234567890 +-=*", FontTahoma11);
//            System::getInstance().getDisplay()->print(0, 50, "my inverted system font", FontArial9, true);
            System::getInstance().getDisplay()->getController().requestUpdate();
        }
        if(tl.elapsed(50000)) //XXX
        {
            static uint8_t step = 0;
            tl.reset();
            ledData =
            {
                    System::getInstance().getRGBLeds()->getCycledValue(step, 0),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 3),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 6),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 9),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 12),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 15),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 18),
                    System::getInstance().getRGBLeds()->getCycledValue(step, 21)
            };
            System::getInstance().getRGBLeds()->send(ledData);
            step = (step+1) % 24;
        }
    }

    System::getInstance().terminate();
}
