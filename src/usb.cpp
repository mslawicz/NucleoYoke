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
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_conf.h"

USBD_HandleTypeDef hUsbDeviceFS;

namespace USB
{

Device::Device()
{
    USBD_StatusTypeDef usbdStatus;
    usbdStatus = USBD_Init(&hUsbDeviceFS, &Class_Desc, DEVICE_FS);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB Device initialized");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB Device initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB CDC class registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB CDC class initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = static_cast<USBD_StatusTypeDef>(USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS));
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB CDC interface registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB CDC interface registration failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = USBD_Start(&hUsbDeviceFS);
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

}

} /* namespace USB */
