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
    static uint8_t cnt = 0;
    if(buffer[0] == 0x03)
    {
        memcpy(&forceFeedbackData, buffer+1, sizeof(ForceFeedbackData));
        if(cnt++ > 20)
        {
            cnt = 0;
            System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "FF: " + std::to_string(forceFeedbackData.pitchForce) + "," + std::to_string(forceFeedbackData.rollForce) + "," + std::to_string(forceFeedbackData.yawForce));
        }
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Received unexpected HID report id=" + std::to_string(static_cast<int>(buffer[0])));
    }
}


