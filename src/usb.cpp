/*
 * usb.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "system.h"
#include "gpio.h"
#include "usb.h"
#include "timer.h" //XXX test
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "usbd_conf.h"

namespace USB
{

USBD_HandleTypeDef Device::deviceHandle; // device handle structure

Device::Device()
{
    USBD_StatusTypeDef usbdStatus;
    usbdStatus = USBD_Init(&Device::deviceHandle, &HID_Desc, DEVICE_FS);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB Device initialized");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB Device initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = USBD_RegisterClass(&Device::deviceHandle, USBD_HID_CLASS);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB HID class registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB HID class initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = USBD_Start(&Device::deviceHandle);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB device started");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB device not started, code=" + Console::toHex(usbdStatus));
        return;
    }
}

Device::~Device()
{
    // TODO Auto-generated destructor stub
}


void Device::test(void)
{
    static Timer tm;
    static uint8_t x = 0;
    static uint8_t y = 0;
    if(tm.elapsed(10000) && System::getInstance().systemPushbutton.read())
    {
        tm.reset();
        uint8_t buffer[4] = {0, x, y, 0}; // buttons, X, Y, wheel
        USBD_HID_SendReport(&Device::deviceHandle, buffer, 4);
    }
    if(tm.elapsed(1000000))
    {
        x = rand() % 7 - 3;
        y = rand() % 5 - 2;
    }
}

} /* namespace USB */
