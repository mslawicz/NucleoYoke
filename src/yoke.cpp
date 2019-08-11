/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "yoke.h"
#include "system.h"

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
    case 0x0C:  // PID Device control
        deviceControl();
        break;
    case 0x0D:  //set overall force feedback gain
        forceFeedbackGain = receivedData[1];
        break;
    default:
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Unsupported report ID: " + Console::toHex(receivedData[0],2));
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
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Unsupported device control: " + Console::toHex(receivedData[1],2));
        break;
    }
}
