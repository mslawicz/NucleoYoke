/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "yoke.h"
#include "system.h"
#include "usbd_customhid.h"

Yoke::Yoke() :
    interface()
{
    receivedData = nullptr;
}

Yoke::~Yoke()
{
    // TODO Auto-generated destructor stub
}

void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    receivedData = buffer;
    System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "Received from host:" + getBufferData(6));
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
