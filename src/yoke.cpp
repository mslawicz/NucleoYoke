/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "yoke.h"
#include "system.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

Yoke::Yoke() :
    interface()
{
    receivedData = nullptr;
    forceFeedbackGain = 0xFF;
}

Yoke::~Yoke()
{
    // TODO Auto-generated destructor stub
}

void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    receivedData = buffer;
    // take the action depending on the report_id (see the report descriptor)
    switch(receivedData[0])
    {
    case 0x05:  // set force report
        setEffect();
        break;
    case 0x0C:  // PID Device control
        deviceControl();
        break;
    case 0x0D:  //set overall force feedback gain
        forceFeedbackGain = receivedData[1];
        break;
    default:
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Unsupported report:" + getBufferData(6));
        break;
    }
}

/*
 * device control from the host
 */
void Yoke::deviceControl(void)
{
    switch(receivedData[1])
    {
    case 0x03:
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "Stop all effects request");
        break;
    case 0x04:
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "Reset request");
        break;
    default:
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Unsupported device control:" + getBufferData(4));
        break;
    }
}

/*
 * make string of buffer content
 */
std::string Yoke::getBufferData(uint8_t length)
{
    std::string data;
    for(uint8_t k=0; k<length; k++)
    {
        data += " ";
        data += Console::toHex(receivedData[k], 2, false);
    }
    return data;
}

/*
 * this function acknowledges the handled effect to the host
 */
void Yoke::setEffect(void)
{
    uint8_t buffer[] = {0x02, 0x1F, 0x00};
    switch(receivedData[1])
    {
    case 0x01:  // constant force
    case 0x04:  // periodic
    case 0x08:  // spring
    case 0x09:  // damper
    case 0x0B:  // friction
        buffer[2] = 0x80 | receivedData[1];
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buffer, sizeof(buffer));
        break;
    default:
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Unsupported effect inquired:" + getBufferData(4));
        break;
    }
}
