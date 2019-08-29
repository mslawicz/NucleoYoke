/*
 * usb.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "USB.h"

#include "usbd_desc.h"
//#include "usbd_hid.h" XXX enable for HID
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
//#include "usbd_cdc.h" XXX enable it for CDC
//#include "usbd_cdc_if.h" XXX enable it for CDC
#include "usbd_conf.h"
#include "GPIO.h"
#include "System.h"
#include "Timer.h" //XXX test

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
//end of custom HID device section */
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
        uint8_t state = (uint8_t)(currentButtonState);
        // joystick test
        uint16_t X = state * (1000 - rand() % 2001);
        uint16_t Y = 0 * state * (2000 * (2 - rand() % 5));
        uint16_t Z = 0 * state * (2000 * (2 - rand() % 5));
        uint8_t Rx = 0 * state * (63 * (1 + rand() % 4));
        uint8_t Ry = 0 * state * (63 * (1 + rand() % 4));
        uint8_t Rz = 0 * state * (63 * (1 + rand() % 4));
        uint8_t buf[] =
        {
                0x01, // report id
                (uint8_t)(X & 0xFF),
                (uint8_t)((X >> 8) & 0xFF),
                (uint8_t)(Y & 0xFF),
                (uint8_t)((Y >> 8) & 0xFF),
                (uint8_t)(Z & 0xFF),
                (uint8_t)((Z >> 8) & 0xFF),
                Rx,
                Ry,
                Rz,

                (uint8_t)(0 * state * (1 + (rand() % 8))), // HAT 0-7

                (uint8_t)(0 * state * (rand() & 0xFF)), // buttons 1-8
                (uint8_t)(0 * state * (rand() & 0xFF)), // buttons 9-16
                (uint8_t)(0 * state * (rand() & 0xFF)), // buttons 17-24
                (uint8_t)(0 * state * (rand() & 0xFF)) // buttons 25-32
        };

        // keyboard test
//        uint8_t buf[9] = {0x02, 0x00};
//        buf[3] = state * (0x04 + rand() % 20);
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buf, sizeof(buf));


        // buffer test
//        if(state)
//        {
//            uint8_t buf[64] = {0x03, 0x00};
//            uint8_t length = (rand() % 4) + 1;
//            buf[1] = length;
//            for(uint8_t k=0; k < length; k++)
//            {
//                buf[2+k] = k+1;
//            }
//            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buf, sizeof(buf));
//        }

        //USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buf, sizeof(buf));
        lastButtonState = currentButtonState;
    }
}

} /* namespace USB */
