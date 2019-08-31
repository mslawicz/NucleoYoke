/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "usbd_customhid.h"
#include "Yoke.h"
#include "System.h"


Yoke::Yoke() :
    interface()
{

}

Yoke::~Yoke()
{
    // TODO Auto-generated destructor stub
}

/*
 * called in USB data reception callback function
 */
void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    if(buffer[0] == 0x03)
    {
        memcpy(&forceFeedbackData, buffer+1, sizeof(ForceFeedbackData));
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Received unexpected HID report id=" + std::to_string(static_cast<int>(buffer[0])));
    }
}


