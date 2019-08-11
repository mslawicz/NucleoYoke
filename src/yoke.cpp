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
    // TODO Auto-generated constructor stub

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
    System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "Device control: " + Console::toHex(receivedData[1],2));
}
