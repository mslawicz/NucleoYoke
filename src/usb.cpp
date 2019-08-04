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
//#include "usbd_hid.h" XXX enable for HID
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
//#include "usbd_cdc.h" XXX enable it for CDC
//#include "usbd_cdc_if.h" XXX enable it for CDC
#include "usbd_conf.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

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
/* XXX this section is for HID device
    usbdStatus = USBD_RegisterClass(&hUsbDeviceFS, &USBD_HID);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB HID class registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB HID class initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
end of HID device section */
/* XXX this section is for custom HID device */
    usbdStatus = USBD_RegisterClass(&hUsbDeviceFS, &USBD_CUSTOM_HID);
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB custom HID class registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB custom HID class initialization failed, code=" + Console::toHex(usbdStatus));
        return;
    }
    usbdStatus = static_cast<USBD_StatusTypeDef>(USBD_CUSTOM_HID_RegisterInterface(&hUsbDeviceFS, &USBD_CustomHID_fops_FS));
    if(usbdStatus == USBD_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "USB custom HID interface registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_USB, "USB custom HID interface registration failed, code=" + Console::toHex(usbdStatus));
        return;
    }
//end of CDC device section */
/* XXX this section is for CDC device
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
end of CDC device section */

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
    static GPIO_PinState lastButtonState = GPIO_PinState::GPIO_PIN_RESET;
    GPIO_PinState currentButtonState = System::getInstance().systemPushbutton.read();
    if(currentButtonState != lastButtonState)
    {
        lastButtonState = currentButtonState;
    }
}

} /* namespace USB */
